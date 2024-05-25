//
// Created by lukeh on 02/11/2019.
//
#include <AdDb.h>
#include <iostream>
#include "../internal_includes/ConfigParser.h"
#include "../internal_includes/DbJSON.h"

namespace {

    template<class ItemGroup>
    std::vector<IndexConfig::Item> LoadItems(Config::DbConfig& config) {
        std::vector<IndexConfig::Item> items;
        auto &cons = config.Get<ItemGroup>();
        items.reserve(cons.size());

        for (auto &pItem: cons) {
            if (!pItem->template Supplied<Config::id>()) {
                throw AdDb::InvalidConfigError{"Every consituency must have an id"};
            } else if (!pItem->template Supplied<Config::keys>()) {
                std::string err = "Missing keys list for consituency: " + pItem->template Get<Config::id>();
                throw AdDb::InvalidConfigError{std::move(err)};
            } else {
                bool found = false;
                const std::string& id = pItem->template Get<Config::id>();
                for (auto it = items.begin(); !found && it != items.end(); ++it) {
                    IndexConfig::Item& item = *it;
                    if (item.id == id) {
                        found = true;
                        for  (auto& k: pItem->template Get<Config::keys>()) {
                            item.keys.push_back(std::move(k));
                        }
                    }
                }
                if (!found) {
                    items.push_back({id, pItem->template Get<Config::keys>()});
                }
            }
        }

        return items;
    }


    std::unique_ptr<AdDb::DbConfig> ParseConfig(const char* cfg) {
        auto config = std::make_unique<AdDb::DbConfig>();
        std::string error;
        Config::DbConfig configParser;
        if (!configParser.Parse(cfg, error)) {
            throw AdDb::InvalidConfigError {error};
        }
        if (!configParser.Supplied<Config::consituencies>()) {
            throw AdDb::InvalidConfigError {"'consituencies' list was not provided!"};
        } else {
            config->consituencies = std::make_shared<IndexConfig>(
                    LoadItems<Config::consituencies>(configParser));
        }

        config->startTimeCutOff = nstimestamp::Time(std::string(nstimestamp::Time::EpochTimestamp));
        if (configParser.Supplied<Config::startingCutOff>()) {
            const std::string timeString = configParser.Get<Config::startingCutOff>();
            config->startTimeCutOff.InitialiseFromString(timeString.c_str(), timeString.size());
        }

        config->endTimeCutOff = nstimestamp::Time(std::string(nstimestamp::Time::EpochTimestamp));
        if (configParser.Supplied<Config::endCutOff>()) {
            const std::string timeString = configParser.Get<Config::endCutOff>();
            config->endTimeCutOff.InitialiseFromString(timeString.c_str(), timeString.size());
        }

        if (configParser.Supplied<Config::issues>()) {
            config->issues = std::make_shared<IndexConfig>(
                    LoadItems<Config::issues>(configParser));
        } else {
            config->issues = std::make_shared<IndexConfig>(std::vector<IndexConfig::Item>{});
        }


        return config;
    }

}

AdDb::AdDb(const std::string& cfg) {
    store = std::make_unique<FacebookAdStore>();
    std::shared_ptr<FacebookAdKey> facebookKey = std::make_shared<FacebookAdKey>();
    config = ParseConfig(cfg.c_str());
    consituencies = std::make_unique<FacebookAdIndex>(config->consituencies, facebookKey);
    issues = std::make_unique<FacebookAdIndex>(config->issues, facebookKey);
}

void AdDb::Store(std::unique_ptr<FacebookAd> ad) {
    if ( (ad->deliveryStartTime.DiffSecs(config->startTimeCutOff) >= 0) &&
         (ad->deliveryEndTime.DiffSecs(config->endTimeCutOff) >= 0))
    {
        const auto& storedAd = store->Store(std::move(ad));
        consituencies->Update(storedAd);
        issues->Update(storedAd);
    }

}
AdDb::FacebookAdList AdDb::Get(const FacebookAdIndex& idx, const std::string& name) const {
    FacebookAdList results;
    const auto& keys = idx.Get(name);
    results.reserve(keys.size());
    for(const auto& key: keys) {
        const auto& storedAd = store->Get(key);
        if (!storedAd.IsNull()) {
            results.emplace_back(storedAd.NewSharedRef());
        }
    }
    return results;

}

AdDb::FacebookAdList AdDb::GetConstituency(const std::string &name) const {
    return Get(*consituencies, name);
}

AdDb::FacebookAdList AdDb::GetIssue(const std::string &name) const {
    return Get(*issues, name);
}

void AdDb::ForEachAdByConstituency(const AdDb::ForEachGroupedFacebookAd &cb) const {
    bool continueScan = true;
    const auto& cons = config->consituencies->items;
    for (auto it = cons.begin(); continueScan && it != cons.end(); ++it) {
        const auto& con = *it;
        const auto& keys = consituencies->Get(con.id);
        for (auto kit = keys.begin(); continueScan && kit!=keys.end(); ++kit) {
            const auto& key = *kit;
            const auto& storedAd = store->Get(key);
            if (!storedAd.IsNull()) {
                switch(cb(con, storedAd.NewSharedRef())) {
                    case DbScanOp::CONTINUE:
                        continueScan = true;
                        break;
                    case DbScanOp::STOP:
                        continueScan = false;
                        break;
                }
            }
        }
    }
}

void AdDb::ForEach(IndexConfig& idx, const AdDb::ForEachItemDefn &cb) const {
    for (auto& cfg: idx.items) {
        switch(cb(cfg.id)) {
            case DbScanOp::CONTINUE:
                break;
            case DbScanOp::STOP:
                return;
        }
    }
}

void AdDb::ForEachConsituency(const AdDb::ForEachItemDefn &cb) const {
    ForEach(*config->consituencies, cb);
}

void AdDb::ForEachIssue(const AdDb::ForEachItemDefn &cb) const {
    ForEach(*config->issues, cb);
}

AdDb::Serialization AdDb::Serialize() const {
    thread_local DbJSON::JSON encoder;
    encoder.Clear();
    encoder.Get<DbJSON::store>() = std::move(store->Serialize().data);
    encoder.Get<DbJSON::issues>() = issues->Serialize();
    encoder.Get<DbJSON::cons>() = consituencies->Serialize();

    Serialization serialization;
    serialization.json = encoder.GetJSONString();

    return serialization;
}


AdDb::AdDb(const std::string &cfg, const AdDb::Serialization &data, const AdDb::DeSerialMode& mode)
{
    DbJSON::JSON decoder;
    std::string error;
    if (decoder.Parse(data.json.c_str(), error)) {
        FacebookAdStore::Serialization storeSerial;
        storeSerial.data = std::move(decoder.Get<DbJSON::store>());
        store = std::make_unique<FacebookAdStore>(std::move(storeSerial));

        std::shared_ptr<FacebookAdKey> facebookKey = std::make_shared<FacebookAdKey>();
        config = ParseConfig(cfg.c_str());

        if (mode == DeSerialMode::FORCE_REINDEX) {
            issues = std::make_unique<FacebookAdIndex>(config->issues, facebookKey);
            consituencies = std::make_unique<FacebookAdIndex>(config->consituencies, facebookKey);
            store->ForEach([&] (const StoredFacebookAd& ad) -> auto {
                issues->Update(ad);
                consituencies->Update(ad);
                return FacebookAdStore::ScanOp::CONTINUE;
            });
        } else {
            issues = std::make_unique<FacebookAdIndex>(
                    config->issues,
                    facebookKey,
                    decoder.Get<DbJSON::issues>());

            consituencies = std::make_unique<FacebookAdIndex>(
                    config->consituencies,
                    facebookKey,
                    decoder.Get<DbJSON::cons>());
        }


    } else {
        throw InvalidSerializationError{};
    }
}

void AdDb::ForEachAd(const AdDb::ForEachFacebookAd& cb) const {
    store->ForEach([&] (const StoredFacebookAd& ad) -> FacebookAdStore::ScanOp {
        switch(cb(ad.ItemRef())) {
            case DbScanOp::CONTINUE:
                return FacebookAdStore::ScanOp::CONTINUE;
            case DbScanOp::STOP:
                return FacebookAdStore::ScanOp::STOP;
        }
        return FacebookAdStore::ScanOp::STOP;
    });
}

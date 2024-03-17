//
// Created by lukeh on 01/11/2019.
//
#include <FacebookAd.h>
#include <FacebookAdStore.h>
#include <FacebookParser.h>
#include <locale>
#include "../internal_includes/StoredItemJSON.h"
#include "../internal_includes/StoreJSON.h"

namespace {
    constexpr StoredFacebookAd::KeyType CalculateKey(const FacebookAd& ad) {
        return ad.id;
    }

    std::string ToUpper(const std::string& target) {
        std::string upper(target);
        thread_local auto& f = std::use_facet<std::ctype<char>>(std::locale());
        f.toupper(&upper[0], &upper[0] + upper.size());

        return upper;
    }

    void DeSerialize(const std::string& ser,
                     std::unique_ptr<FacebookAd>& ad,
                     StoredFacebookAd::UpperStrings& uppers)
    {
        using namespace StoredItem;
        thread_local FacebookAdParser parser;
        thread_local JSON encoder;
        encoder.Clear();

        std::string error;
        if (encoder.Parse(ser.c_str(), error)) {
            parser.DeSerialize(std::move(encoder.Get<item>()), ad);
            uppers.fundingEntity = std::move(encoder.Get<upperFundingEntity>());
            uppers.pageName = std::move(encoder.Get<upperPageName>());
            uppers.bodies = std::move(encoder.Get<upperBodies>());
            uppers.linkCaptions = std::move(encoder.Get<upperCaptions>());
            uppers.linkTitles = std::move(encoder.Get<upperTitles>());
            uppers.linkDescriptions = std::move(encoder.Get<upperDescriptions>());
        }
    }

    std::string Serialize(const StoredFacebookAd& ad) {
        using namespace StoredItem;
        thread_local FacebookAdParser parser;
        thread_local JSON encoder;
        SimpleJSONBuilder builder;
        encoder.Clear();
        encoder.Get<item>() = parser.Serialize(ad.ItemRef());
        encoder.Get<upperFundingEntity>() = ad.CachedUppers().fundingEntity;
        encoder.Get<upperPageName>() = ad.CachedUppers().pageName;
        encoder.Get<upperBodies>() = ad.CachedUppers().bodies;
        encoder.Get<upperTitles>() = ad.CachedUppers().linkTitles;
        encoder.Get<upperDescriptions>() = ad.CachedUppers().linkDescriptions;
        encoder.Get<upperCaptions>() = ad.CachedUppers().linkCaptions;

        return encoder.GetJSONString();
    }

}

StoredFacebookAd::StoredFacebookAd(std::shared_ptr<FacebookAd> ad)
    : item(std::move(ad))
{
    key = CalculateKey(*item);

    upperCase.bodies.reserve(item->bodies.size());
    for (const std::string& body: item->bodies) {
        upperCase.bodies.push_back(ToUpper(body));
    }
    for (const std::string& title: item->linkTitles) {
        upperCase.linkTitles.push_back(ToUpper(title));
    }
    for (const std::string& caption: item->linkCaptions) {
        upperCase.linkCaptions.push_back(ToUpper(caption));
    }
    for (const std::string& description: item->linkDescriptions) {
        upperCase.linkDescriptions.push_back(ToUpper(description));
    }
    upperCase.fundingEntity = ToUpper(item->fundingEntity);
    upperCase.pageName = ToUpper(item->pageName);
}

StoredFacebookAd::StoredFacebookAd(const StoredFacebookAd::Serialization &ad)
{
    std::unique_ptr<FacebookAd> uad;
    DeSerialize(ad.json, uad, upperCase);
    item = std::move(uad);
    if (item.get()) {
        key = CalculateKey(*item);
    }
}

StoredFacebookAd::Serialization StoredFacebookAd::Serialize() const {
    return {::Serialize(*this)};
}


void StoredFacebookAd::PatchStoredValues(FacebookAd &&ad) {
    if (!IsNull()) {
        *item = std::move(ad);
        auto newKey = CalculateKey(*item);
        if (newKey != key) {
            throw KeyChangeError{};
        }
    }
}

const StoredFacebookAd &FacebookAdStore::Get(const StoredFacebookAd::KeyType& key) const {
    auto it = ads.find(key);

    if (it != ads.end()) {
        return *it->second;
    } else {
        return this->NullAdd;
    }
}

const StoredFacebookAd &FacebookAdStore::Store(std::unique_ptr<FacebookAd> ad) {
    StoredFacebookAd* result = &NullAdd;
    auto storedAd = std::make_unique<StoredFacebookAd>(std::move(ad));
    auto it = ads.find(storedAd->Key());
    if (it != ads.end()) {
        it->second->PatchStoredValues(std::move(*(storedAd->item)));
    } else {
        result = ads.try_emplace(storedAd->Key(), std::move(storedAd)).first->second.get();
    }
    return *result;
}

FacebookAdStore::Serialization FacebookAdStore::Serialize() const {
    thread_local Store::JSON encoder;
    encoder.Clear();
    for (auto it = ads.begin(); it != ads.end(); ++it) {
        auto& jitem = encoder.Get<Store::store>().emplace_back();

        jitem->Get<Store::key>() = it->first;
        jitem->Get<Store::ad>() = std::move(it->second->Serialize().json);
    }

    return {encoder.GetJSONString()};

}

FacebookAdStore::FacebookAdStore(FacebookAdStore::Serialization data) {
    thread_local Store::JSON decoder;
    decoder.Clear();
    std::string error;

    if (decoder.Parse(data.data.c_str(), error)) {
        for (auto& jitem: decoder.Get<Store::store>()) {
            StoredFacebookAd::Serialization adData;
            adData.json = std::move(jitem->Get<Store::ad>());
            ads[jitem->Get<Store::key>()] =
                    std::make_unique<StoredFacebookAd>(adData);
        }
    } else {
        throw InvalidSerialization{};
    }

}

void FacebookAdStore::ForEach(const ForEachFn& cb) const {
    bool cont = true;
    for (auto it = ads.cbegin(); it != ads.cend() && cont; ++it) {
        cont = (cb(*it->second) == ScanOp::CONTINUE);
    }
}



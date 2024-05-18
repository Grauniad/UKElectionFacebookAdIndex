//
// Created by lukeh on 01/11/2019.
//
#include "FacebookAdKey.h"
#include "../internal_includes/DbUtils.h"
#include "../internal_includes/IndexJSON.h"

using namespace DbUtils;

bool FacebookAdKey::HasKey(const StoredFacebookAd &item, const std::string &key) const {
    bool match = false;
    if (!item.IsNull()) {
        if (Search(item.CachedUppers().fundingEntity, key)) {
            match = true;
        } else if (Search(item.CachedUppers().pageName, key )) {
        match = true;
        } else {
            for (size_t i = 0;
                 !match && i < item.CachedUppers().linkDescriptions.size();
                 ++i)
            {
                match = Search(item.CachedUppers().linkDescriptions[i], key);
            }

            for (size_t i = 0;
                 !match && i < item.CachedUppers().linkCaptions.size();
                 ++i)
            {
                match = Search(item.CachedUppers().linkCaptions[i], key);
            }

            for (size_t i = 0;
                 !match && i < item.CachedUppers().linkTitles.size();
                 ++i)
            {
                match = Search(item.CachedUppers().linkTitles[i], key);
            }

            for (size_t i = 0;
                 !match && i < item.CachedUppers().bodies.size();
                 ++i)
            {
                match = Search(item.CachedUppers().bodies[i], key);
            }
        }
    }
    return match;
}

std::string
FacebookAdKey::Serialize(const std::vector<std::string> &names, const std::vector<std::vector<KeyType>> &ids) {
    thread_local IndexJSON::JSON encoder;
    encoder.Clear();

    encoder.Get<IndexJSON::items>().resize(names.size());
    for (size_t i = 0; i < names.size() && i < ids.size(); ++i) {
        auto& next = *encoder.Get<IndexJSON::items>()[i];
        next.Get<IndexJSON::name>() = std::move(names[i]);
        next.Get<IndexJSON::keys>() = std::move(ids[i]);
    }

    return encoder.GetJSONString();
}

void FacebookAdKey::DeSerialize(
        const std::string &serialization,
        std::vector<std::string> &names,
        std::vector<std::vector<KeyType>> &ids)
{
    thread_local IndexJSON::JSON decoder;
    decoder.Clear();

    std::string error;
    names.clear();
    ids.clear();
    names.reserve(decoder.Get<IndexJSON::items>().size());
    ids.reserve(decoder.Get<IndexJSON::items>().size());
    if (decoder.Parse(serialization.c_str(), error)) {
        for (size_t i = 0; i < decoder.Get<IndexJSON::items>().size(); ++i) {
            auto& item = *decoder.Get<IndexJSON::items>()[i];
            names.emplace_back(std::move(item.Get<IndexJSON::name>()));
            ids.emplace_back(std::move(item.Get<IndexJSON::keys>()));
        }
    }

}

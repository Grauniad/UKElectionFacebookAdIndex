//
// Created by lukeh on 01/11/2019.
//

#ifndef ELECTIONDATAANAL_FACEBOOKADSTORE_H
#define ELECTIONDATAANAL_FACEBOOKADSTORE_H
#include <memory>
#include <map>
#include <FacebookAd.h>
#include <functional>

class StoredFacebookAd {
public:
    using KeyType = size_t;

    struct UpperStrings {
        std::vector<std::string> bodies;
        std::vector<std::string> linkTitles;
        std::vector<std::string> linkDescriptions;
        std::vector<std::string> linkCaptions;
        std::string fundingEntity;
        std::string pageName;
    };

    struct Serialization {
        std::string json;
    };

    StoredFacebookAd() = default;
    explicit StoredFacebookAd(std::shared_ptr<FacebookAd> ad);
    explicit StoredFacebookAd(const Serialization& ad);

    [[nodiscard]] const FacebookAd& ItemRef() const { return *item;}
    [[nodiscard]] std::shared_ptr<const FacebookAd> NewSharedRef() const { return item;}
    [[nodiscard]] const KeyType& Key() const { return key;}

    [[nodiscard]] const UpperStrings& CachedUppers() const { return upperCase;}

    [[nodiscard]] bool IsNull() const { return (item == nullptr); }

    void PatchStoredValues(FacebookAd&& patchFrom);

    Serialization Serialize() const;

    struct KeyChangeError: std::exception {
        [[nodiscard]] const char* what() const noexcept override {
            return "Stored Value Patch would cause a Key Change";
        }
    };

private:
    friend class FacebookAdStore;

    UpperStrings upperCase;
    std::shared_ptr<FacebookAd> item;
    KeyType key;
};

class FacebookAdStore {
public:
    struct Serialization {
        std::string data;
    };
// INITIALISATION
    FacebookAdStore() = default;
    explicit FacebookAdStore(Serialization data);

// Store individual items
    const StoredFacebookAd& Store(std::unique_ptr<FacebookAd> ad);

// Access indvidual items
    [[nodiscard]] const StoredFacebookAd& Get(const StoredFacebookAd::KeyType & key) const;

// Loop Access
    enum class ScanOp {
        CONTINUE,
        STOP
    };
    using ForEachFn = std::function<ScanOp (const StoredFacebookAd& ad)>;
    void ForEach(const ForEachFn& cb) const;

// SERIALIZATION
    [[nodiscard]] Serialization Serialize() const;

    struct InvalidSerialization: public std::exception {
        [[nodiscard]] const char* what() const noexcept override {
            return "Could not initialize Ad Store from serialization";
        }
    };

private:
    std::map<const StoredFacebookAd::KeyType, std::unique_ptr<StoredFacebookAd>> ads;
    StoredFacebookAd NullAdd;
};

#endif //ELECTIONDATAANAL_FACEBOOKADSTORE_H

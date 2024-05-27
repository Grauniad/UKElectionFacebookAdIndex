//
// Created by lukeh on 02/11/2019.
//

#ifndef ELECTIONDATAANAL_ADDB_H
#define ELECTIONDATAANAL_ADDB_H
#include <memory>
#include <FacebookAd.h>
#include <FacebookAdKey.h>
#include <Index.h>
#include <memory>
#include <set>
#include <functional>

class AdDb {
public:
    enum class DbScanOp {
        CONTINUE,
        STOP
    };
    struct Serialization {
        Serialization() = default;
        Serialization(const Serialization& rhs) = delete;
        Serialization(Serialization&& rhs) = default;
        Serialization& operator=(const Serialization& rhs) = delete;
        Serialization& operator=(Serialization&& rhs) = default;

        std::string json;
    };
    enum class DeSerialMode {
        NO_REINDEX,
        FORCE_REINDEX
    };
    AdDb(const std::string& cfg);
    AdDb(const std::string& cfg, const Serialization& data, const DeSerialMode& mode = DeSerialMode::NO_REINDEX);

    using FacebookAdIndex = Index<FacebookAdKey>;
    using FacebookAdList  = std::vector<std::shared_ptr<const FacebookAd>>;
    using ForEachGroupedFacebookAd =
            std::function<DbScanOp (const IndexConfig::Item&, std::shared_ptr<const FacebookAd> ad)>;
    using ForEachFacebookAd =
            std::function<DbScanOp (const FacebookAd& ad)>;
    using ForEachItemDefn = std::function<DbScanOp (const std::string& name)>;
    void Store(std::unique_ptr<FacebookAd> ad);

    [[nodiscard]] FacebookAdList GetConstituency(const std::string& name) const;
    [[nodiscard]] FacebookAdList GetIssue(const std::string& name) const;
    [[nodiscard]] FacebookAdList GetFunder(const std::string& name) const;

    void ForEachAdByConstituency(const ForEachGroupedFacebookAd& cb) const;
    void ForEachAd(const ForEachFacebookAd& cb) const;
    void ForEachConsituency(const ForEachItemDefn& cb) const;
    void ForEachIssue(const ForEachItemDefn& cb) const;
    void ForEachFunder(const ForEachItemDefn& cb) const;

    Serialization Serialize() const;

    struct InvalidConfigError: std::exception {
        InvalidConfigError(std::string err): error(std::move(err)) {}
        const char * what() const noexcept {return error.c_str();}
        std::string error;
    };

    struct InvalidSerializationError: std::exception {
        const char * what() const noexcept {
            return "Failed to de-serialize database!";
        }
    };

    struct DbConfig {
        using ItemList = std::vector<IndexConfig::Item>;
        std::shared_ptr<IndexConfig> consituencies;
        std::shared_ptr<IndexConfig> issues;
        nstimestamp::Time startTimeCutOff;
        nstimestamp::Time endTimeCutOff;
    };
private:
    [[nodiscard]] std::string SerializeFunders() const;
    void DeserializeFunders(const std::string& data);
    [[nodiscard]] FacebookAdList Get(const FacebookAdIndex& idx, const std::string& key) const;
    void ForEach(IndexConfig& idx, const AdDb::ForEachItemDefn &cb) const;
    std::unique_ptr<FacebookAdStore> store;
    std::unique_ptr<FacebookAdIndex> consituencies;
    std::unique_ptr<FacebookAdIndex> issues;
    std::map<std::string, std::set<StoredFacebookAd::KeyType>> funders;

    std::unique_ptr<DbConfig> config;
};

#endif //ELECTIONDATAANAL_ADDB_H

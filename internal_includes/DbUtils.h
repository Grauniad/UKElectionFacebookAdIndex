//
// Created by lukeh on 03/11/2019.
//

#ifndef ELECTIONDATAANAL_DBUTILS_H
#define ELECTIONDATAANAL_DBUTILS_H
#include <memory>
#include <AdDb.h>
#include <Reports.h>

namespace DbUtils {
    struct NoSuchCfgFile: public std::exception {
        NoSuchCfgFile(const std::string fname) {
            error = "Could not load cfg file: ";
            error += fname;
        }
        const char* what() const noexcept override {
            return error.c_str();
        }
        std::string error;
    };
    struct NoData {};
    struct BadData {};

    void WriteDbToDisk(AdDb& db, const std::string& fname);

    std::unique_ptr<AdDb> LoadDb(const std::string& cfgFile, const std::string& dataDir);
    std::unique_ptr<AdDb> LoadDb(const std::string& cfgFile, const std::string& dataDir, const std::string& dbStartState);

    constexpr const char * const REDACTED_TEXT =
            "\"Creative Fields\" redacted due to Facebook Terms & Conditions. To review ad details please acquire a FACEBOOK API Token and re-run the report locally";
    enum class WriteMode {
        FULL,
        REDACTED
    };
    void WriteReport(Reports::Report& report, const std::string& basePath, WriteMode = WriteMode::FULL);
    void WriteTimeSeries(Reports::TimeSeriesReport & report, const std::vector<std::string> timeStamps, const std::string& path);
    void ExportAdsToAWSFormat(AdDb &db, const std::string &fname);

    bool Search(const std::string& toSearch, const std::string& key);
}


#endif //ELECTIONDATAANAL_DBUTILS_H

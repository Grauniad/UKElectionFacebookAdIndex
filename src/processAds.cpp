//
// Created by lukeh on 04/11/2019.
//
#include <iostream>
#include "../internal_includes/DbUtils.h"
#include <Reports.h>
#include <iomanip>
#include <util_time.h>

namespace {
    void Usage() {
        std::cout << "Usage: processAds [options] <cfg> <directory to read> <directory to write>" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "   --redacted          :  Remove ad 'creative' details, producing a report suitable for distribution" << std::endl;
        std::cout << "   --loadDb [dbfile]   :  Initialize from an existing database" << std::endl;
        std::cout << "   --secureDb [dbfile] :  Secure the index to disk" << std::endl;
        std::cout << "   --ignoreBlankFunders:  Don't report on ads with no declared funder" << std::endl;
        std::cout << "   --funder [funder]   :  Only process from the specified funder ('--' for blank funder)" << std::endl;
        std::cout << "   --skipParse         :  Don't load the ads directory" << std::endl;
        std::cout << "   --reindex           :  Discard existing indexes" <<
        std::endl;
    }
}


int main(int argc, const char* argv[]) {
    DbUtils::WriteMode writeMode = DbUtils::WriteMode::FULL;
    if (argc < 4) {
        Usage();
        return 1;
    }
    size_t startIdx = argc - 3;

    std::string startDb = "";
    std::string resultDb = "";
    std::string funder = "";
    bool ignoreBlankFunder = false;
    bool skipParse = false;
    bool funderFilter = false;
    AdDb::DeSerialMode indexMode  = AdDb::DeSerialMode::NO_REINDEX;

    for (size_t i = 1; i < startIdx; ++i) {
        const std::string arg = argv[i];
        if (arg == "--redacted") {
            std::cout << "--redacted: Cretive details will be removed" << std::endl;
            writeMode = DbUtils::WriteMode::REDACTED;
        } else if (arg == "--skipParse") {
                std::cout << "--skipParse: will not load ads" << std::endl;
                skipParse = true;
        } else if (arg == "--reindex") {
            std::cout << "--reindex: will rebuild our indexes" << std::endl;
            indexMode  = AdDb::DeSerialMode::FORCE_REINDEX;
        } else if (arg == "--loadDb") {
            ++i;
            startDb = argv[i];
            std::cout << "--loadDb: Initial state will be loaded from " << startDb << std::endl;
        } else if (arg == "--secureDb") {
            ++i;
            resultDb = argv[i];
            std::cout << "--secureDb: Final state will be secured to " << resultDb << std::endl;
        } else if (arg == "--funder") {
            ++i;
            funder = argv[i];
            if (funder == "--") {
                funder = "";
            }
            std::cout << "--funder: Only ads funded by: '" << funder << "'" << std::endl;
            funderFilter = true;
        } else if (arg == "--ignoreBlankFunders") {
            std::cout << "--ignoreBlankFunders: ads with no declared funder will be ignored" << std::endl;
            ignoreBlankFunder = true;
        } else {
            Usage();
            return 1;
        }

        if (i > startIdx) {
            Usage();
            return 1;
        }
    }

    std::string cfg = argv[startIdx];
    std::string dataDir = argv[startIdx+1];

    std::string reportDir = argv[startIdx+2];

    if (skipParse) {
        dataDir = "";
    }

    auto db = DbUtils::LoadDb(cfg, dataDir, startDb, indexMode);

    const auto AdFilter = [&] (const FacebookAd& ad) -> bool {
        bool include = true;

        if (ignoreBlankFunder && ad.fundingEntity.empty()) {
            include = false;
        }

        if (funderFilter && ad.fundingEntity != funder) {
            include = false;
        }

        return include;
    };

    auto report = Reports::DoConsituencyReport(*db, AdFilter);

    DbUtils::WriteReport(*report, reportDir + "/Cons", writeMode);

    report = Reports::DoIssueReport(*db, AdFilter);
    DbUtils::WriteReport(*report, reportDir + "/Issues", writeMode);

    report = Reports::DoFundersReport(*db, AdFilter);
    DbUtils::WriteReport(*report, reportDir + "/Funders", writeMode);

    if (resultDb != "") {
        std::cout << "Securing database to disk... ";
        nstimestamp::Time secureStart;
        DbUtils::WriteDbToDisk(*db, resultDb);

        nstimestamp::Time secureEnd;
        std::cout << "done [" <<  secureEnd.DiffUSecs(secureStart) / 1000 << "ms]" << std::endl;
    }


    return 0;
}


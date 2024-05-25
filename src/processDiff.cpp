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
        std::cout << "Usage: processAds [options] <cfg> <startDb> <endDb> <directory to write>" << std::endl;
    }
}


int main(int argc, const char* argv[]) {
    DbUtils::WriteMode writeMode = DbUtils::WriteMode::FULL;
    if (argc < 5) {
        Usage();
        return 1;
    }
    size_t startIdx = argc - 4;

    std::string cfg = argv[startIdx];
    std::string startDb = argv[startIdx+1];
    std::string endDb = argv[startIdx+2];
    std::string reportDir = argv[startIdx+3];

    auto start_db = DbUtils::LoadDb(cfg, "", startDb);
    auto end_db = DbUtils::LoadDb(cfg, "", endDb);

    auto report = Reports::DoConDiffReport(*start_db, *end_db);
    DbUtils::WriteReport(*report, reportDir + "/Cons", writeMode);

    report = Reports::DoIssuesDiffReport(*start_db, *end_db);
    DbUtils::WriteReport(*report, reportDir + "/Issues", writeMode);

    return 0;
}


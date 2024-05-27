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
        std::cout << "Usage: generateAWSExport <cfg> <startDb> <export file>" << std::endl;
    }
}


int main(int argc, const char* argv[]) {
    DbUtils::WriteMode writeMode = DbUtils::WriteMode::FULL;
    if (argc < 4) {
        Usage();
        return 1;
    }
    size_t startIdx = argc - 3;

    std::string cfg = argv[startIdx];
    std::string startDb = argv[startIdx+1];
    std::string export_file = argv[startIdx+2];

    auto start_db = DbUtils::LoadDb(cfg, "", startDb);

    DbUtils::ExportAdsToAWSFormat(*start_db, export_file);

    return 0;
}


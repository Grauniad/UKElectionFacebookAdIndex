//
// Created by lukeh on 03/11/2019.
//
#include "../internal_includes/test_utils.h"
#include <AdDb.h>
#include <Reports.h>

const std::string dbConfig = R"JSON(
{
  "consituencies": [
    {
         "id": "Search#0",
         "keys": ["entity#0"]
    }, {
         "id": "Search#1",
         "keys": ["entity#1"]
    }, {
         "id": "Search#2",
         "keys": ["entity#DOES_NOT_EXIST"]
    }],

  "consituencies": [
    {
         "id": "Search#1",
         "keys": ["entity##1"]
    }],

    "issues": [{
         "id": "Brexit",
         "keys": ["brexit", "EU", "European Union", "350 million", "350,000,000", "350000000"]
    }
    ]
}
)JSON";

class ReportTest: public ::testing::Test {
public:
    ReportTest()
            : theDb(dbConfig)
    {
        FacebookAd& ad = ads.emplace_back();
        ad.id = 0;
        ad.creationTime = nstimestamp::Time("2019-10-29T16:15:59+0000");
        ad.fundingEntity = "Entity#0";
        ad.spend.lower_bound = 0;
        ad.spend.upper_bound = 99;
        ad.impressions.lower_bound = 0;
        ad.impressions.upper_bound = 99;

        FacebookAd& ad1 = ads.emplace_back();
        ad1.id = 1;
        ad1.creationTime = nstimestamp::Time("2019-10-29T18:16:59+0000");
        ad1.deliveryEndTime = nstimestamp::Time("2020-10-29T18:18:60+0000");
        ad1.fundingEntity = "Entity#1";
        ad1.spend.lower_bound = 100;
        ad1.spend.upper_bound = 199;
        ad1.impressions.lower_bound = 100;
        ad1.impressions.upper_bound = 199;

        FacebookAd& ad2 = ads.emplace_back();
        ad2.id = 2;
        ad2.creationTime = nstimestamp::Time("2019-10-29T18:17:59+0000");
        ad2.deliveryEndTime = nstimestamp::Time("2020-10-29T18:18:60+0000");
        ad2.fundingEntity = "Entity##1";
        ad2.spend.lower_bound = 1000;
        ad2.spend.upper_bound = 1200;
        ad2.impressions.lower_bound = 1000;
        ad2.impressions.upper_bound = 1200;

        FacebookAd& ad3 = ads.emplace_back();
        ad3.id = 3;
        ad3.creationTime = nstimestamp::Time("2019-10-29T18:17:60+0000");
        ad3.deliveryEndTime = nstimestamp::Time("2020-10-29T18:18:60+0000");
        ad3.fundingEntity = "Entity#1";
        ad3.spend.lower_bound = 0;
        ad3.spend.upper_bound = 99;
        ad3.impressions.lower_bound = 0;
        ad3.impressions.upper_bound = 99;

        FacebookAd& ad4 = ads.emplace_back();
        ad4.id = 4;
        ad4.creationTime = nstimestamp::Time("2019-10-29T18:18:60+0000");
        ad4.deliveryEndTime = nstimestamp::Time("2020-10-29T18:18:60+0000");
        ad4.fundingEntity = "Brexit";
        ad4.spend.lower_bound = 0;
        ad4.spend.upper_bound = 99;
        ad4.impressions.lower_bound = 0;
        ad4.impressions.upper_bound = 99;
    }
protected:
    void SetUp() {
        for (const auto& ad: ads) {
            theDb.Store(std::make_unique<FacebookAd>(ad));
        }
    }

    AdDb                    theDb;
    std::vector<FacebookAd> ads;
};

TEST_F(ReportTest, ConsituencyMentions_Summary) {
    auto report = *Reports::DoConsituencyReport(theDb);

    ASSERT_EQ(report.size(), 3);

    ASSERT_EQ(report["Search#0"].summary.name, "Search#0");
    ASSERT_EQ(report["Search#0"].summary.count, 1);
    ASSERT_EQ(report["Search#0"].summary.estImpressions, 1);
    ASSERT_EQ(report["Search#0"].summary.estSpend, 1);

    ASSERT_EQ(report["Search#1"].summary.name, "Search#1");
    ASSERT_EQ(report["Search#1"].summary.count, 3);
    ASSERT_EQ(report["Search#1"].summary.estImpressions, 1 + 149 + 1100);
    ASSERT_EQ(report["Search#1"].summary.estSpend, 0 + 149 + 1100);

    ASSERT_EQ(report["Search#2"].summary.name, "Search#2");
    ASSERT_EQ(report["Search#2"].summary.count, 0);
    ASSERT_EQ(report["Search#2"].summary.estImpressions, 0);
    ASSERT_EQ(report["Search#2"].summary.estSpend, 0);
}

TEST_F(ReportTest, ConsituencyMentions_AdList) {
    auto report = *Reports::DoConsituencyReport(theDb);

    ASSERT_EQ(report.size(), 3);

    ASSERT_EQ(report["Search#0"].ads.size(), 1);
    ASSERT_EQ(report["Search#0"].ads[0].ad->fundingEntity, "Entity#0");

    ASSERT_EQ(report["Search#1"].ads.size(), 3);
    ASSERT_EQ(report["Search#1"].ads[0].ad->fundingEntity, "Entity#1");
    ASSERT_EQ(report["Search#1"].ads[1].ad->fundingEntity, "Entity##1");
    ASSERT_EQ(report["Search#1"].ads[2].ad->fundingEntity, "Entity#1");

    ASSERT_EQ(report["Search#2"].ads.size(), 0);
}

TEST_F(ReportTest, ConsituencyMentions_predicate_Summary) {
    auto report = *Reports::DoConsituencyReport(theDb, [] (const FacebookAd& ad) -> bool {
        return (ad.spend.lower_bound > 0);
    });

    ASSERT_EQ(report.size(), 3);

    ASSERT_EQ(report["Search#0"].summary.name, "Search#0");
    ASSERT_EQ(report["Search#0"].summary.count, 0);
    ASSERT_EQ(report["Search#0"].summary.estImpressions, 0);
    ASSERT_EQ(report["Search#0"].summary.estSpend, 0);

    ASSERT_EQ(report["Search#1"].summary.name, "Search#1");
    ASSERT_EQ(report["Search#1"].summary.count, 2);
    ASSERT_EQ(report["Search#1"].summary.estImpressions, 149 + 1100);
    ASSERT_EQ(report["Search#1"].summary.estSpend, 149 + 1100);

    ASSERT_EQ(report["Search#2"].summary.name, "Search#2");
    ASSERT_EQ(report["Search#2"].summary.count, 0);
    ASSERT_EQ(report["Search#2"].summary.estImpressions, 0);
    ASSERT_EQ(report["Search#2"].summary.estSpend, 0);
}

TEST_F(ReportTest, ConsituencyMentions_AdList_Spend) {
    auto report = *Reports::DoConsituencyReport(theDb);

    ASSERT_EQ(report.size(), 3);

    ASSERT_EQ(report["Search#0"].ads.size(), 1);
    ASSERT_EQ(report["Search#0"].ads[0].guestimateSpend, 1);

    ASSERT_EQ(report["Search#1"].ads.size(), 3);
    ASSERT_EQ(report["Search#1"].ads[0].guestimateSpend, 149);
    ASSERT_EQ(report["Search#1"].ads[1].guestimateSpend, 1100);
    ASSERT_EQ(report["Search#1"].ads[2].guestimateSpend, 1);

    ASSERT_EQ(report["Search#2"].ads.size(), 0);
}

TEST_F(ReportTest, ConsituencyMentions_AdList_Spend_predicate) {
    auto report = *Reports::DoConsituencyReport(theDb, [] (const FacebookAd& ad) -> bool {
        return (ad.spend.lower_bound > 0);
    });

    ASSERT_EQ(report.size(), 3);

    ASSERT_EQ(report["Search#0"].ads.size(), 0);

    ASSERT_EQ(report["Search#1"].ads.size(), 2);
    ASSERT_EQ(report["Search#1"].ads[0].guestimateSpend, 149);
    ASSERT_EQ(report["Search#1"].ads[1].guestimateSpend, 1100);

    ASSERT_EQ(report["Search#2"].ads.size(), 0);
}

TEST_F(ReportTest, ConsituencyMentions_AdList_Impressions) {
    auto report = *Reports::DoConsituencyReport(theDb);

    ASSERT_EQ(report.size(), 3);

    ASSERT_EQ(report["Search#0"].ads.size(), 1);
    ASSERT_EQ(report["Search#0"].ads[0].guestimateImpressions, 1);

    ASSERT_EQ(report["Search#1"].ads.size(), 3);
    ASSERT_EQ(report["Search#1"].ads[0].guestimateImpressions, 149);
    ASSERT_EQ(report["Search#1"].ads[1].guestimateImpressions, 1100);
    ASSERT_EQ(report["Search#1"].ads[2].guestimateImpressions, 1);

    ASSERT_EQ(report["Search#2"].ads.size(), 0);
}

TEST_F(ReportTest, Issues_Summary) {
    auto report = *Reports::DoIssueReport(theDb);

    ASSERT_EQ(report.size(), 1);

    ASSERT_EQ(report["Brexit"].summary.name, "Brexit");
    ASSERT_EQ(report["Brexit"].summary.count, 1);
    ASSERT_EQ(report["Brexit"].summary.estImpressions, 1);
    ASSERT_EQ(report["Brexit"].summary.estSpend, 1);
}

class DeltaReportTest: public ::testing::Test {
public:
    DeltaReportTest() : startDb(dbConfig), endDb(dbConfig) {}
protected:
    enum class CATEGORY  {
        SEARCH_0,
        SEARCH_1,
        SEARCH_2,
    };

    std::string GetKey(const CATEGORY& cat) {
        switch (cat) {
            case CATEGORY::SEARCH_0:
                return "Search#0";
            case CATEGORY::SEARCH_1:
                return "Search#1";
            case CATEGORY::SEARCH_2:
                return "Search#2";
        }
        throw "Shouldn't be here";
    }
    using KeyType = size_t;
    struct AdDelta {
        BoundedQuantity startImpressions;
        BoundedQuantity endImpressions;

        BoundedQuantity startSpend;
        BoundedQuantity endSpend;
    };

    AdDelta NewFlatDelta(const BoundedQuantity& base, const size_t& spendDiff, const size_t& impressDiff) {
        AdDelta delta;
        delta.startSpend = base;
        delta.endSpend = delta.startSpend;
        delta.endSpend.lower_bound += spendDiff;
        delta.endSpend.upper_bound += spendDiff;

        delta.startImpressions = base;
        delta.endImpressions = delta.startImpressions;
        delta.endImpressions.lower_bound += impressDiff;
        delta.endImpressions.upper_bound += impressDiff;

        return delta;
    }

    std::unique_ptr<FacebookAd> NewAd( const CATEGORY& cat) {
        auto ad = std::make_unique<FacebookAd>();
        ad->id = nextId++;
        struct UnknownCat{ };
        switch (cat) {
            case CATEGORY::SEARCH_0:
                ad->pageName = "entity#0";
                break;
            case CATEGORY::SEARCH_1:
                ad->pageName = "entity#1";
                break;
            default:
                throw UnknownCat{};
        }

        return ad;
    }

    KeyType NewAddDiff(const CATEGORY& cat, const AdDelta& diff) {
        auto ad = NewAd(cat);

        auto startAd = std::make_unique<FacebookAd>(*ad);
        startAd->spend = diff.startSpend;
        startAd->impressions = diff.startImpressions;
        startDb.Store(std::move(startAd));

        auto endAd = std::make_unique<FacebookAd>(*ad);
        endAd->spend = diff.endSpend;
        endAd->impressions = diff.endImpressions;
        endDb.Store(std::move(endAd));

        return ad->id;
    }

    std::unique_ptr<Reports::Report> DoDiff() {
        return Reports::DoConDiffReport(startDb, endDb);
    }

    struct AdCheck {
        KeyType  id;
        long     estSpend;
        long     estImpressions;
    };

    struct ToCheck {
        CATEGORY cat;
        Reports::SummaryItem summary;
        std::vector<AdCheck> ads;
    };
    std::vector<ToCheck> EmptyChecks() {
        std::vector<ToCheck> checks = {
            { CATEGORY::SEARCH_0, {GetKey(CATEGORY::SEARCH_0), 0, 0, 0}, {} },
            { CATEGORY::SEARCH_1, {GetKey(CATEGORY::SEARCH_1), 0, 0, 0}, {} },
            { CATEGORY::SEARCH_2, {GetKey(CATEGORY::SEARCH_2), 0, 0, 0}, {} }
        };

        return checks;
    }

    void DoChecks(const std::vector<ToCheck>& checks, Reports::Report& report) {
        ASSERT_EQ(report.size(), checks.size());

        for (const auto& check: checks) {
            const std::string key = GetKey(check.cat);
            ASSERT_EQ(report[key].summary.name, check.summary.name);
            ASSERT_EQ(report[key].summary.count, check.summary.count);
            ASSERT_EQ(report[key].summary.estSpend, check.summary.estSpend);
            ASSERT_EQ(report[key].summary.estImpressions, check.summary.estImpressions);

            auto& reportAds = report[key].ads;

            ASSERT_EQ(reportAds.size(), check.ads.size());
            for (size_t i = 0; i < check.ads.size(); ++i) {
                ASSERT_EQ(reportAds[i].ad->id, check.ads[i].id);
                ASSERT_EQ(reportAds[i].guestimateImpressions, check.ads[i].estImpressions);
                ASSERT_EQ(reportAds[i].guestimateSpend, check.ads[i].estSpend);
            }

        }
    }

    AdDb startDb;
    AdDb endDb;
private:

    KeyType nextId = 0;
};


TEST_F( DeltaReportTest, NoAds) {
    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_EQ(report.size(), 3);

    auto checks = EmptyChecks();

    ASSERT_NO_FATAL_FAILURE(
        DoChecks(checks, report)
    );
}

TEST_F( DeltaReportTest, NoDiff) {
    auto checks = EmptyChecks();

    // 1 add, same in both
    auto delta = NewFlatDelta({100, 200}, 0, 0);
    auto key = NewAddDiff(CATEGORY::SEARCH_0, delta);
    checks[0].ads.push_back({key, 0, 0});

    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_NO_FATAL_FAILURE(
            DoChecks(checks, report)
    );
}

TEST_F( DeltaReportTest, MoreImpressions) {
    auto checks = EmptyChecks();

    // 1 add, same in both
    auto delta = NewFlatDelta({100, 200}, 0, 150);
    auto key = NewAddDiff(CATEGORY::SEARCH_0, delta);
    checks[0].ads.push_back({key, 0, 150});
    checks[0].summary.estImpressions = 150;

    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_NO_FATAL_FAILURE(
            DoChecks(checks, report)
    );
}

TEST_F( DeltaReportTest, MoreSpend) {
    auto checks = EmptyChecks();

    // 1 add, same in both
    auto delta = NewFlatDelta({100, 200}, 50, 0);
    auto key = NewAddDiff(CATEGORY::SEARCH_0, delta);
    checks[0].ads.push_back({key, 50, 0});
    checks[0].summary.estSpend = 50;

    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_NO_FATAL_FAILURE(
            DoChecks(checks, report)
    );
}

TEST_F( DeltaReportTest, NewDb) {
    auto checks = EmptyChecks();

    auto newAd = NewAd(CATEGORY::SEARCH_0);
    newAd->spend.lower_bound = 100;
    newAd->spend.upper_bound = 200;
    newAd->impressions.lower_bound = 100;
    newAd->impressions.upper_bound = 200;
    const auto id = newAd->id;

    endDb.Store(std::move(newAd));
    checks[0].ads.push_back({id, 150, 150});
    checks[0].summary.estSpend += 150;
    checks[0].summary.estImpressions += 150;
    checks[0].summary.count++;

    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_NO_FATAL_FAILURE(
            DoChecks(checks, report)
    );
}

TEST_F( DeltaReportTest, NewZeroBandAd) {
    auto checks = EmptyChecks();

    auto newAd = NewAd(CATEGORY::SEARCH_0);
    newAd->spend.lower_bound = 0;
    newAd->spend.upper_bound = 99;
    newAd->impressions.lower_bound = 0;
    newAd->impressions.upper_bound = 99;
    const auto id = newAd->id;

    endDb.Store(std::move(newAd));
    checks[0].ads.push_back({id, 1, 1});
    checks[0].summary.estSpend = 1;
    checks[0].summary.estImpressions = 1;
    checks[0].summary.count++;

    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_NO_FATAL_FAILURE(
        DoChecks(checks, report)
    );
}

TEST_F( DeltaReportTest, AdditionalAd) {
    auto checks = EmptyChecks();

    // 1 new Ad...
    auto newAd = NewAd(CATEGORY::SEARCH_0);
    newAd->spend.lower_bound = 100;
    newAd->spend.upper_bound = 200;
    newAd->impressions.lower_bound = 100;
    newAd->impressions.upper_bound = 200;
    const auto id = newAd->id;
    endDb.Store(std::move(newAd));
    checks[0].ads.push_back({id, 150, 150});
    checks[0].summary.estSpend += 150;
    checks[0].summary.estImpressions += 150;
    checks[0].summary.count++;

    // ... and 1 unchanged ad...
    auto delta = NewFlatDelta({100, 200}, 0, 0);
    auto key = NewAddDiff(CATEGORY::SEARCH_0, delta);
    checks[0].ads.push_back({key, 0, 0});


    auto ptr = DoDiff();
    auto report = *ptr;

    ASSERT_NO_FATAL_FAILURE(
            DoChecks(checks, report)
    );
}

TEST_F(DeltaReportTest, RemovedAd) {
    auto checks = EmptyChecks();

    // 1 original ad...
    auto oldAd = NewAd(CATEGORY::SEARCH_0);
    oldAd->spend.lower_bound = 100;
    oldAd->spend.upper_bound = 200;
    oldAd->impressions.lower_bound = 100;
    oldAd->impressions.upper_bound = 200;
    const auto id = oldAd->id;
    startDb.Store(std::move(oldAd));
    checks[0].ads.push_back({id, 150, 150});
    checks[0].summary.estSpend += 150;
    checks[0].summary.estImpressions += 150;
    checks[0].summary.count++;

    // ... and 1 unchanged ad...
    auto delta = NewFlatDelta({100, 200}, 0, 0);
    auto key = NewAddDiff(CATEGORY::SEARCH_0, delta);
    checks[0].ads.push_back({key, 0, 0});


    ASSERT_THROW(DoDiff(), Reports::DbHasRegressed);
}

class TimeSeriesTest: public ::testing::Test {
public:
protected:
    enum class CATEGORY  {
        SEARCH_0,
        SEARCH_1,
        SEARCH_2
    };

    enum class TIME_SERIES  {
        BOTH
    };

    std::string GetKey(const CATEGORY& cat) {
        switch (cat) {
            case CATEGORY::SEARCH_0:
                return "Search#0";
            case CATEGORY::SEARCH_1:
                return "Search#1";
            case CATEGORY::SEARCH_2:
                return "Search#2";
        }
        throw "Shouldn't be here";
    }

    std::string GetKeyForCatt(const CATEGORY& cat) {
        switch (cat) {
            case CATEGORY::SEARCH_0:
                return "entity#0";
            case CATEGORY::SEARCH_1:
                return "entity#1";
            default:
                throw "Shouldn't be here";
        }
        throw "Shouldn't be here";
    }

    void ForEachCategory(const std::function<void (const CATEGORY& c, std::string name)>& cb) {
        cb(CATEGORY::SEARCH_0, GetKey(CATEGORY::SEARCH_0));
        cb(CATEGORY::SEARCH_1, GetKey(CATEGORY::SEARCH_1));
        cb(CATEGORY::SEARCH_2, GetKey(CATEGORY::SEARCH_2));
    }

    static constexpr size_t NO_AD = std::numeric_limits<size_t>::max();

    void AddAd(const CATEGORY& cat,
               const TIME_SERIES& series,
               const std::string& funder,
               const std::vector<size_t>& vals)
    {
        ASSERT_EQ(vals.size(), dbs.size());

        FacebookAd ad;
        ad.id = ++nextId;
        ad.fundingEntity = funder;
        ad.pageName = GetKeyForCatt(cat);

        for (size_t i = 0; i < vals.size(); ++i) {
            const size_t& val = vals[i];
            if (val != NO_AD) {
                auto item = std::make_unique<FacebookAd>(ad);
                item->spend.lower_bound = val-1;
                item->spend.upper_bound = val+1;

                item->impressions.lower_bound = val-1;
                item->impressions.upper_bound = val+1;

                dbs[i].Store(std::move(item));
            }
        }


    }

    void InitializeDbs(const size_t& dps) {
        dbs.clear();
        dbs.reserve(dps);
        for (size_t i = 0; i < dps; ++i) {
            dbs.emplace_back(dbConfig);
        }
    }

    std::unique_ptr<Reports::TimeSeriesReport> GetReport(Reports::TimeSeriesMode mode = Reports::TimeSeriesMode::STANDARD) {
        std::vector<std::unique_ptr<Reports::Report>> rawReports;
        std::vector<Reports::Report*> reports;
        rawReports.reserve(dbs.size());
        reports.reserve(dbs.size());
        for (size_t i = 0; i < dbs.size(); ++i) {
            auto& storedReport = rawReports.emplace_back(Reports::DoConsituencyReport(dbs[i]));
            reports.emplace_back(storedReport.get());
        }
        return Reports::DoTimeSeries(reports, mode);
    }

    std::vector<AdDb> dbs;
private:
    size_t nextId = 0;
    std::vector<std::string> stamps;

};

TEST_F(TimeSeriesTest, NoResults) {
    InitializeDbs(0);
    auto preport = GetReport();
    const auto& report = *preport;

    ASSERT_EQ(0, report.size());
}

TEST_F(TimeSeriesTest, EmptyResults) {
    InitializeDbs(2);
    auto preport = GetReport();
    const auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto&, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
        ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        ASSERT_EQ(it->second.residualSpend.size(), 2);
        ASSERT_EQ(it->second.residualImpressions.size(), 2);
    });
}

TEST_F(TimeSeriesTest, SingleAd) {
    InitializeDbs(2);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {100, 200});

    auto preport = GetReport();
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 1);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0], 100);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1], 200);

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 1);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0], 100);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1], 200);
        }

    });
}

TEST_F(TimeSeriesTest, NewAd) {
    InitializeDbs(3);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {NO_AD, 100, 200});

    auto preport = GetReport();
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 1);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 3);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0], 0);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1], 100);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][2], 200);

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 1);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 3);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0], 0);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1], 100);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][2], 200);
        }

    });
}

TEST_F(TimeSeriesTest, MultipleAds_SingelFunder) {
    InitializeDbs(2);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {100, 200});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {200, 300});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {300, 400});

    auto preport = GetReport();
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 1);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0], 100 + 200 + 300);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1], 200 + 300 + 400);

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  0 );
            ASSERT_EQ(it->second.residualSpend[1],  0 );

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 1);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0], 100 + 200 + 300);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1], 200 + 300 + 400);

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  0 );
            ASSERT_EQ(it->second.residualImpressions[1],  0 );
        }

    });
}

TEST_F(TimeSeriesTest, MultipleFunders) {
    InitializeDbs(2);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {100, 200});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test2", {200, 300});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test3", {300, 400});

    auto preport = GetReport();
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 3);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0],  100);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1],  200);

            ASSERT_EQ(it->second.guestimatedSpend["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][0],  200 );
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][1],  300 );

            ASSERT_EQ(it->second.guestimatedSpend["Test3"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test3"][0],  300);
            ASSERT_EQ(it->second.guestimatedSpend["Test3"][1],  400);

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  0 );
            ASSERT_EQ(it->second.residualSpend[1],  0 );

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 3);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0],  100);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1],  200);

            ASSERT_EQ(it->second.guestimatedImpressions["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][0],  200 );
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][1],  300 );

            ASSERT_EQ(it->second.guestimatedImpressions["Test3"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test3"][0],  300);
            ASSERT_EQ(it->second.guestimatedImpressions["Test3"][1],  400);

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  0 );
            ASSERT_EQ(it->second.residualImpressions[1],  0 );
        }

    });
}

TEST_F(TimeSeriesTest, IgnoreSmall) {
    InitializeDbs(2);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {100, 100});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test2", {1, 300});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test3", {1, 1});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test4", {1, 1});

    auto preport = GetReport();
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0],  100);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1],  100);

            ASSERT_EQ(it->second.guestimatedSpend["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][1],  300 );

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  3 );
            ASSERT_EQ(it->second.residualSpend[1],  2 );

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0],  100);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1],  100);

            ASSERT_EQ(it->second.guestimatedImpressions["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][1],  300 );

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  3 );
            ASSERT_EQ(it->second.residualImpressions[1],  2 );
        }

    });
}

TEST_F(TimeSeriesTest, CountCummulative) {
    InitializeDbs(2);

    for (size_t i = 0; i < 1000; ++i) {
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {100, 100});
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test2", {1, 300});
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test3", {1, 1});
    }

    auto preport = GetReport();
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0],  100 * 1000);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1],  100 * 1000);

            ASSERT_EQ(it->second.guestimatedSpend["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][1],  300 * 1000);

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  2 * 1000 );
            ASSERT_EQ(it->second.residualSpend[1],  1  * 1000);

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0],  100 * 1000);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1],  100 * 1000);

            ASSERT_EQ(it->second.guestimatedImpressions["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][1],  300 * 1000);

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  2 * 1000 );
            ASSERT_EQ(it->second.residualImpressions[1],  1  * 1000);
        }

    });
}

TEST_F(TimeSeriesTest, BaselineSeries) {
    InitializeDbs(3);

    for (size_t i = 0; i < 1000; ++i) {
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test0", {1000, 1000, 1000});
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {50, 100, 100});
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test2", {NO_AD, 1, 300});
        AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test3", {NO_AD, 1, 1});
    }

    auto preport = GetReport(Reports::TimeSeriesMode::REMOVE_BASELINE);
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 3);

            // TODO: We want to remove this noise
            ASSERT_EQ(it->second.guestimatedSpend["Test0"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test0"][0],  0 * 1000);
            ASSERT_EQ(it->second.guestimatedSpend["Test0"][1],  0 * 1000);

            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0],  50 * 1000);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1],  50 * 1000);

            ASSERT_EQ(it->second.guestimatedSpend["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][1],  300 * 1000);

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  2 * 1000 );
            ASSERT_EQ(it->second.residualSpend[1],  1  * 1000);

            ASSERT_EQ(it->second.guestimatedImpressions["Test0"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test0"][0],  0 * 1000);
            ASSERT_EQ(it->second.guestimatedImpressions["Test0"][1],  0 * 1000);

            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0],  50 * 1000);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1],  50 * 1000);

            ASSERT_EQ(it->second.guestimatedImpressions["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][1],  300 * 1000);

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  2 * 1000 );
            ASSERT_EQ(it->second.residualImpressions[1],  1  * 1000);
        }

    });
}

TEST_F(TimeSeriesTest, OtherBecomesItem) {
    InitializeDbs(3);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {500, 1000, 1000});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test2", {1,   2,    300});

    auto preport = GetReport(Reports::TimeSeriesMode::REMOVE_BASELINE);
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 2);

            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0],  500);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1],  500);

            ASSERT_EQ(it->second.guestimatedSpend["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][1],  300);

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  1);
            ASSERT_EQ(it->second.residualSpend[1],  0);

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 2);

            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0],  500);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1],  500);

            ASSERT_EQ(it->second.guestimatedImpressions["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][1],  300);

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  1);
            ASSERT_EQ(it->second.residualImpressions[1],  0);
        }

    });
}

TEST_F(TimeSeriesTest, ItemRegressesToOther) {
    InitializeDbs(3);

    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test1", {500, 1000, 1000});
    AddAd(CATEGORY::SEARCH_0, TIME_SERIES::BOTH, "Test2", {6, 7,  7});

    auto preport = GetReport(Reports::TimeSeriesMode::REMOVE_BASELINE);
    auto& report = *preport;

    ASSERT_EQ(3, report.size());

    ForEachCategory([&] (const auto& cat, std::string catName ) {
        auto it = report.find(catName);
        ASSERT_NE(it, report.end());

        if (cat != CATEGORY::SEARCH_0) {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 0);
            ASSERT_EQ(it->second.guestimatedImpressions.size(), 0);
        } else {
            ASSERT_EQ(it->second.guestimatedSpend.size(), 2);

            ASSERT_EQ(it->second.guestimatedSpend["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][0],  500);
            ASSERT_EQ(it->second.guestimatedSpend["Test1"][1],  500);

            ASSERT_EQ(it->second.guestimatedSpend["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedSpend["Test2"][1],  0);

            ASSERT_EQ(it->second.residualSpend.size(), 2);
            ASSERT_EQ(it->second.residualSpend[0],  7);
            ASSERT_EQ(it->second.residualSpend[1],  7);

            ASSERT_EQ(it->second.guestimatedImpressions.size(), 2);

            ASSERT_EQ(it->second.guestimatedImpressions["Test1"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][0],  500);
            ASSERT_EQ(it->second.guestimatedImpressions["Test1"][1],  500);

            ASSERT_EQ(it->second.guestimatedImpressions["Test2"].size(), 2);
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][0],  0 );
            ASSERT_EQ(it->second.guestimatedImpressions["Test2"][1],  0);

            ASSERT_EQ(it->second.residualImpressions.size(), 2);
            ASSERT_EQ(it->second.residualImpressions[0],  7);
            ASSERT_EQ(it->second.residualImpressions[1],  7);
        }

    });
}

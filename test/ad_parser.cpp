#include <FacebookParser.h>
#include "../internal_includes/test_utils.h"
#include <gtest/gtest.h>
#include <fstream>
#include <OSTools.h>

const std::string json = R"JSON(
{
    "data": [
        {},
        {
            "ad_snapshot_url": "https://www.facebook.com/ads/archive/render_ad/?id=2556409461271942&access_token=EAAi1Yrtc0qIBOxpW58loZA841eceB8LKyyXSmrEyjLX2dLQ4F3hNFZCjFspykugzSXn5UcZCFfrPZCQPpziPMeijyVDU7husrXKAQfpUORbkM0Yv6BR0MpRaxwN75gyzET9JzfZAxuKRbgWZCoykgaULDrFxvDzu0FTFWCgPpi6FUZBvJmfYtrYHyRjvMl84jq3HyqdpOT5YTyZBYpZBVOXOUVZAo906RmIddgtBEdZBrurLyhm8WteTrpK0gXutoCYazEZD",
            "bylines": "Woking Liberal Democrats",
            "page_name": "Test Page",
            "delivery_by_region": [
                {
                    "percentage": "0.984687",
                    "region": "England"
                },
                {
                  "percentage": "0.010652",
                  "region": "Wales"
                },
                {
                  "percentage": "0.003995",
                  "region": "Scotland"
                },
                {
                  "percentage": "0.000666",
                  "region": "Northern Ireland"
                }
            ],
            "ad_delivery_start_time": "2019-10-29",
            "ad_delivery_stop_time": "2019-11-01",
            "ad_creative_link_descriptions" : [
                 "<TEST>", "<TEST> 2"
            ],
            "ad_creative_link_titles": [
                "We will stop Brexit, invest in public services like our NHS and take action to fight climate change",
                 "Test Title"
            ],
            "ad_creative_link_captions": [
                "wokinglibdems.org.uk",
                 "Test caption"
            ],
            "ad_creative_bodies": [
                "Local Councillor, former Mayor and life-long resident 𝗪𝗶𝗹𝗹 𝗙𝗼𝗿𝘀𝘁𝗲𝗿 is the Lib Dem candidate 𝘁𝗼 𝗯𝗲 𝗪𝗼𝗸𝗶𝗻𝗴'𝘀 𝗻𝗲𝘄 𝗠𝗣!  👏👏👏",
                 "Test body"
            ],
            "ad_creation_time": "2019-10-29",
            "spend": {
                "lower_bound": "0",
                "upper_bound": "99"
            },
            "impressions": {
                "lower_bound": "10000",
                "upper_bound": "14999"
            },
            "demographic_distribution": [
                {
                    "percentage": "0.063901",
                    "age": "45-54",
                    "gender": "female"
                },
                {
                    "percentage": "0.004338",
                    "age": "35-44",
                    "gender": "unknown"
                },
                {
                    "percentage": "0.002711",
                    "age": "25-34",
                    "gender": "unknown"
                },
                {
                    "percentage": "0.000362",
                    "age": "18-24",
                    "gender": "unknown"
                },
                {
                    "percentage": "0.002892",
                    "age": "45-54",
                    "gender": "unknown"
                },
                {
                    "percentage": "0.000452",
                    "age": "65+",
                    "gender": "unknown"
                },
                {
                    "percentage": "0.01419",
                    "age": "65+",
                    "gender": "female"
                },
                {
                    "percentage": "0.011388",
                    "age": "65+",
                    "gender": "male"
                },
                {
                    "percentage": "0.01898",
                    "age": "55-64",
                    "gender": "male"
                },
                {
                    "percentage": "0.14633",
                    "age": "18-24",
                    "gender": "female"
                },
                {
                    "percentage": "0.085683",
                    "age": "35-44",
                    "gender": "male"
                },
                {
                    "percentage": "0.047632",
                    "age": "45-54",
                    "gender": "male"
                },
                {
                    "percentage": "0.107466",
                    "age": "18-24",
                    "gender": "male"
                },
                {
                    "percentage": "0.151392",
                    "age": "25-34",
                    "gender": "male"
                },
                {
                    "percentage": "0.031363",
                    "age": "55-64",
                    "gender": "female"
                },
                {
                    "percentage": "0.193239",
                    "age": "25-34",
                    "gender": "female"
                },
                {
                    "percentage": "0.116594",
                    "age": "35-44",
                    "gender": "female"
                },
                {
                    "percentage": "0.001085",
                    "age": "55-64",
                    "gender": "unknown"
                }
            ],
            "publisher_platforms": [
                "facebook",
                "instagram"
            ],
            "currency":"GBP",
            "id": "2556409461271942"
        },
        {
            "impressions": {
                "lower_bound": "1000000"
            }
        }
    ],
    "paging": {
        "cursors": {
            "after": "c2NyYXBpbmdfY3Vyc29yOk1UVTNNak0zTWpBMU5qb3lOVFUyTkRBNU5EWXhNamN4T1RReQZDZD"
        },
        "next": "https://graph.facebook.com/v19.0/ads_archive?access_token=EAAi1Yrtc0qIBOxpW58loZA841eceB8LKyyXSmrEyjLX2dLQ4F3hNFZCjFspykugzSXn5UcZCFfrPZCQPpziPMeijyVDU7husrXKAQfpUORbkM0Yv6BR0MpRaxwN75gyzET9JzfZAxuKRbgWZCoykgaULDrFxvDzu0FTFWCgPpi6FUZBvJmfYtrYHyRjvMl84jq3HyqdpOT5YTyZBYpZBVOXOUVZAo906RmIddgtBEdZBrurLyhm8WteTrpK0gXutoCYazEZD&ad_reached_countries=[%27GB%27]&search_terms=%27Will%20Forster%27&ad_type=POLITICAL_AND_ISSUE_ADS&ad_active_status=ALL&ad_delivery_date_min=2019-10-29&ad_delivery_date_max=2019-11-01&fields=ad_snapshot_url,bylines,delivery_by_region,ad_delivery_start_time,ad_delivery_stop_time,ad_creative_link_descriptions,ad_creative_link_titles,ad_creative_link_captions,ad_creative_bodies,ad_creation_time,spend,impressions,demographic_distribution,publisher_platforms,estimated_audience_size&limit=250&after=c2NyYXBpbmdfY3Vyc29yOk1UVTNNak0zTWpBMU5qb3lOVFUyTkRBNU5EWXhNamN4T1RReQZDZD"
    }
}
)JSON";
const std::string url = "https://www.facebook.com/ads/archive/render_ad/?id=1428050904019116&access_token=EAAi1Yrtc0qIBAPxQq0NvUiYkKA7uZCNEGNCO4kShUVzTbmKSPrA3GAzBeimRAHJUPuIZC1ZBDB5TmTENgGkaPA3OGTLJsnnb6pHo37k4tXby9SG4wA41JEYELf4Bv5F5TVZBLmfogW6tIO3C0yOPTOMm7yNwtG8fap3Ffp84bgzNp0Qiffiad1kZCcX8zZBrcOMa9LX1sU5AZDZD";

class AdParserTest: public ::testing::Test {
protected:
    void SetUp() override {
        startTime.SetNow();
    }
    const std::string AdWithId(const std::string& id) {
        const std::string start = R"JSON(
        {
            "data": [ {
                "ad_creation_time": "2019-10-29T17:16:59+0000",
                "ad_snapshot_url": "https:\/\/www.facebook.com\/ads\/archive\/render_ad\/?id=)JSON";

        const std::string end = R"JSON(&access_token=EAAi1Yrtc0qIBAPxQq0NvUiYkKA7uZCNEGNCO4kShUVzTbmKSPrA3GAzBeimRAHJUPuIZC1ZBDB5TmTENgGkaPA3OGTLJsnnb6pHo37k4tXby9SG4wA41JEYELf4Bv5F5TVZBLmfogW6tIO3C0yOPTOMm7yNwtG8fap3Ffp84bgzNp0Qiffiad1kZCcX8zZBrcOMa9LX1sU5AZDZD"
            } ]
        }
        )JSON";

        return start + id + end;
    }

    nstimestamp::Time startTime;
    using Parser = FacebookAdParser;
    using ParseResult = FacebookAdParser::ParseResult ;
    void WithTheAd(const std::function<void (FacebookAd& ad)>& f);
    FacebookAdParser parser;

};

void AdParserTest::WithTheAd(const std::function<void(FacebookAd &ad)> &f) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ASSERT_EQ(parser.ParseFacebookAdQuery(json.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 3);
    ASSERT_NO_FATAL_FAILURE(f(*ads[1])) << "Failed on initial parse of the ad";

    auto serialization = parser.Serialize(*ads[1]);
    std::unique_ptr<FacebookAd> reParsedAd = nullptr;
    ASSERT_EQ(parser.DeSerialize(serialization, reParsedAd), ParseResult::VALID);
    ASSERT_NE(reParsedAd, nullptr);

    ASSERT_NO_FATAL_FAILURE(f(*reParsedAd)) << "Failed to re-parse ad!" << serialization;
}

TEST_F(AdParserTest, NullJSON) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ASSERT_EQ(parser.ParseFacebookAdQuery(nullptr, ads), ParseResult::INVALID_STRING);
}

TEST_F(AdParserTest, InvalidJSON) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ASSERT_EQ(parser.ParseFacebookAdQuery("{", ads), ParseResult::PARSE_ERROR);
}

TEST_F(AdParserTest, InvalidSerialization) {
    std::unique_ptr<FacebookAd> ad;
    ASSERT_EQ(parser.DeSerialize("{", ad), ParseResult::PARSE_ERROR);
}
// TODO: Publisher Platforms needs to be captured as anum value.
TEST_F(AdParserTest, ValidData) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ASSERT_EQ(parser.ParseFacebookAdQuery(json.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 3);
}

TEST_F(AdParserTest, LinkTitle) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.linkTitles.size(), 2);
        ASSERT_STREQ(ad.linkTitles[0].c_str(), "We will stop Brexit, invest in "
                                               "public services like our NHS and take action to fight climate change");
        ASSERT_STREQ(ad.linkTitles[1].c_str(), "Test Title");
    });
}

TEST_F(AdParserTest, LinkCaption) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.linkCaptions.size(), 2);
        ASSERT_STREQ(ad.linkCaptions[0].c_str(), "wokinglibdems.org.uk");
        ASSERT_STREQ(ad.linkCaptions[1].c_str(), "Test caption");
    });
}

TEST_F(AdParserTest, LinkDescription) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.linkDescriptions.size(), 2);
        ASSERT_STREQ(ad.linkDescriptions[0].c_str(), "<TEST>");
        ASSERT_STREQ(ad.linkDescriptions[1].c_str(), "<TEST> 2");
    });
}

TEST_F(AdParserTest, Body) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.bodies.size(), 2);
        ASSERT_STREQ(ad.bodies[0].c_str(), "Local Councillor, former Mayor "
                                           "and life-long resident 𝗪𝗶𝗹𝗹 "
                                           "𝗙𝗼𝗿𝘀𝘁𝗲𝗿 is the Lib Dem candidate 𝘁𝗼 𝗯𝗲 𝗪𝗼𝗸𝗶𝗻𝗴'𝘀 𝗻𝗲𝘄 𝗠𝗣!  👏👏👏");
        ASSERT_STREQ(ad.bodies[1].c_str(), "Test body");
    });
}

TEST_F(AdParserTest, CreationTime) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.creationTime.Year(), 2019);
        ASSERT_EQ(ad.creationTime.Month(), 10);
        ASSERT_EQ(ad.creationTime.MDay(), 29);
        ASSERT_EQ(ad.creationTime.Hour(), 0);
        ASSERT_EQ(ad.creationTime.Minute(), 0);
        ASSERT_EQ(ad.creationTime.Second(), 0);
    });
}

TEST_F(AdParserTest, StartTime) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.deliveryStartTime.Year(), 2019);
        ASSERT_EQ(ad.deliveryStartTime.Month(), 10);
        ASSERT_EQ(ad.deliveryStartTime.MDay(), 29);
        ASSERT_EQ(ad.deliveryStartTime.Hour(), 0);
        ASSERT_EQ(ad.deliveryStartTime.Minute(), 0);
        ASSERT_EQ(ad.deliveryStartTime.Second(), 0);
    });
}

TEST_F(AdParserTest, StopTime) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.deliveryEndTime.Year(), 2019);
        ASSERT_EQ(ad.deliveryEndTime.Month(), 11);
        ASSERT_EQ(ad.deliveryEndTime.MDay(), 1);
        ASSERT_EQ(ad.deliveryEndTime.Hour(), 0);
        ASSERT_EQ(ad.deliveryEndTime.Minute(), 00);
        ASSERT_EQ(ad.deliveryEndTime.Second(), 0);
    });
}

TEST_F(AdParserTest, StopTime_Default) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ASSERT_EQ(parser.ParseFacebookAdQuery(json.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 3);
    const auto& ad = ads[2];

    ASSERT_GE(ad->deliveryEndTime.DiffSecs(startTime), 0);
    ASSERT_GT(ad->deliveryEndTime.DiffUSecs(startTime), 0);
    ASSERT_GT(ad->deliveryEndTime.DiffNSecs(startTime), 0);
}

TEST_F(AdParserTest, Impressions) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.impressions.lower_bound, 10000);
        ASSERT_EQ(ad.impressions.upper_bound, 14999);
    });
}

TEST_F(AdParserTest, Impressions_NoUpperBound) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ASSERT_EQ(parser.ParseFacebookAdQuery(json.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 3);

    ASSERT_EQ(ads[2]->impressions.lower_bound, 1000000);
    ASSERT_EQ(ads[2]->impressions.upper_bound, 1000002);
}

TEST_F(AdParserTest, Url) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        // Don't bother storing the url - its just a waste of memory
        ASSERT_EQ(ad.pageUrl, "");
    });
}

TEST_F(AdParserTest, Id) {
    // Extract the Id from the url
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.id, 2556409461271942);
    });
}

TEST_F(AdParserTest, NoLeadingZerosInId) {
    std::stringstream buf;
    std::vector<std::unique_ptr<FacebookAd>> ads;

    std::string bigId = AdWithId("0428050904019117");
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::PARSE_ERROR);
}

TEST_F(AdParserTest, NoNonNumericIds) {
    std::vector<std::unique_ptr<FacebookAd>> ads;

    std::string bigId = AdWithId("1428A50904019117");
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::PARSE_ERROR);
}

TEST_F(AdParserTest, LargeIds) {
    std::stringstream buf;
    std::vector<std::unique_ptr<FacebookAd>> ads;

    std::string bigId = AdWithId("1428050904019117");
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 1);
    ASSERT_EQ(ads[0]->id, 1428050904019117);

    ads.clear();
    bigId = AdWithId("18446744073709551615");
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 1);
    ASSERT_EQ(ads[0]->id, 18446744073709551615UL);


    ads.clear();
    buf.clear(); buf.str("");
    buf << std::numeric_limits<size_t>::max();
    bigId = AdWithId(buf.str());
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 1);
    ASSERT_EQ(ads[0]->id, std::numeric_limits<size_t>::max());

}

/**
 * Fallback for backwards comaptibility with tests that didn't set the url...
 */
TEST_F(AdParserTest, IdFallback) {
    const std::string ad = R"JSON( {
        "data": [ {
            "ad_creation_time": "2019-10-29T17:16:59+0000"
        } ]
    }
    )JSON";

    std::vector<std::unique_ptr<FacebookAd>> ads;

    ASSERT_EQ(parser.ParseFacebookAdQuery(ad.c_str(), ads), ParseResult::VALID);
    ASSERT_EQ(ads.size(), 1);
    ASSERT_EQ(ads[0]->id, nstimestamp::Time("2019-10-29T17:16:59+0000").EpochSecs());
}


// Need to be careful not to wrap
TEST_F(AdParserTest, IdTooLong) {
    std::stringstream buf;
    std::vector<std::unique_ptr<FacebookAd>> ads;
    ads.clear();
    buf.clear(); buf.str("");
    buf << std::numeric_limits<size_t>::max();
    std::string bigId = AdWithId(buf.str() + "9");
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::PARSE_ERROR);

    ads.clear();
    buf.clear(); buf.str("");
    buf << std::numeric_limits<size_t>::max();
    std::string bigIdStr = buf.str();
    bool ready = false;
    for (size_t i = (bigIdStr.size() -1); !ready && i >= 0; --i) {
        if (bigIdStr[i] != '9') {
            bigIdStr[i] = 9;
            ready = true;
        }
    }
    ASSERT_TRUE(ready);
    bigId = AdWithId(bigIdStr);
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::PARSE_ERROR);
}

TEST_F(AdParserTest, IdTooGreat) {
    std::stringstream buf;
    std::vector<std::unique_ptr<FacebookAd>> ads;

    ads.clear();
    buf.clear(); buf.str("");
    buf << std::numeric_limits<size_t>::max();
    std::string bigIdStr = buf.str();
    bool ready = false;
    for (size_t i = (bigIdStr.size() -1); !ready && i >= 0; --i) {
        if (bigIdStr[i] != '9') {
            bigIdStr[i] = '9';
            ready = true;
        }
    }
    ASSERT_TRUE(ready);
    std::string bigId = AdWithId(bigIdStr);
    ASSERT_EQ(parser.ParseFacebookAdQuery(bigId.c_str(), ads), ParseResult::PARSE_ERROR);
}


TEST_F(AdParserTest, Spend) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.spend.lower_bound, 0);
        ASSERT_EQ(ad.spend.upper_bound, 99);

        ASSERT_STREQ(ad.currency.c_str(), "GBP");
    });
}

TEST_F(AdParserTest, FundingEntity) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_STREQ(ad.fundingEntity.c_str(), "Woking Liberal Democrats");
    });
}

TEST_F(AdParserTest, PageName) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_STREQ(ad.pageName.c_str(), "Test Page");
    });
}

TEST_F(AdParserTest, Region) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.regionDist.size(), 4);
        ASSERT_FLOAT_EQ(ad.regionDist[RegionCode::ENGLAND], 0.984687);
        ASSERT_FLOAT_EQ(ad.regionDist[RegionCode::NORTHERN_IRELAND], 0.000666);
        ASSERT_FLOAT_EQ(ad.regionDist[RegionCode::SCOTLAND], 0.003995);
        ASSERT_FLOAT_EQ(ad.regionDist[RegionCode::WALES], 0.010652);
    });
}

TEST_F(AdParserTest, Demographic) {
    WithTheAd([&] (FacebookAd& ad) -> void {
        ASSERT_EQ(ad.demographicDist.size(), 18);

        auto val = ad.demographicDist[{GenderCode::FEMALE, AgeRangeCode::AGE_18_TO_24}];
        ASSERT_FLOAT_EQ(val , 0.14633);

        val = ad.demographicDist[{GenderCode::UNKNOWN, AgeRangeCode::AGE_18_TO_24}];
        ASSERT_FLOAT_EQ(val , 0.000362);

        val = ad.demographicDist[{GenderCode::MALE, AgeRangeCode::AGE_25_TO_34}];
        ASSERT_FLOAT_EQ(val , 0.151392);

        val = ad.demographicDist[{GenderCode::MALE, AgeRangeCode::AGE_35_TO_44}];
        ASSERT_FLOAT_EQ(val , 0.085683);

        val = ad.demographicDist[{GenderCode::FEMALE, AgeRangeCode::AGE_45_TO_54}];
        ASSERT_FLOAT_EQ(val , 0.063901);

        val = ad.demographicDist[{GenderCode::MALE, AgeRangeCode::AGE_55_TO_64}];
        ASSERT_FLOAT_EQ(val , 0.01898);

        val = ad.demographicDist[{GenderCode::MALE, AgeRangeCode::AGE_65_AND_OVER}];
        ASSERT_FLOAT_EQ(val , 0.011388);
    });
}

TEST_F(AdParserTest, LoadFile) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    std::ifstream wokingFile("../test/data/woking.json");
    ASSERT_EQ(parser.Parse(wokingFile, ads), FacebookAdParser::ParseResult::VALID);
    ASSERT_EQ(ads.size(), 4);

    const auto& libDemAd = ads[0];
    const auto& conAd = ads[2];

    ASSERT_EQ(libDemAd->fundingEntity, "Woking Liberal Democrats");
    ASSERT_EQ(conAd->fundingEntity, "Woking Conservative Association");
}

TEST_F(AdParserTest, LoadDirectory) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    auto files = OS::Glob("../test/data/test_run/*");
    for (const std::string& path: files) {
        std::ifstream file(path);
        ASSERT_EQ(parser.Parse(file, ads), FacebookAdParser::ParseResult::VALID);
    }
    ASSERT_EQ(ads.size(), 4);

    const auto& libDemAd = ads[0];
    const auto& conAd = ads[2];

    // Whilst sort order is LOCALE defined, if someone's system is so obtuse as to sort 2 before 1, they're
    // welcome to pull request a fix for this test only problem
    ASSERT_EQ(libDemAd->fundingEntity, "Woking Liberal Democrats");
    ASSERT_EQ(conAd->fundingEntity, "Woking Conservative Association");
}


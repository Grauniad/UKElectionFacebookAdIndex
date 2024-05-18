#ifndef ELECTIONDATAANAL_TEST_UTILS_H
#define ELECTIONDATAANAL_TEST_UTILS_H
#include <FacebookParser.h>
#include <iostream>
#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& os, const FacebookAdParser::ParseResult& result) {
    switch (result) {
        case FacebookAdParser::ParseResult::VALID:
            os << "ParseResult::VALID";
            break;
        case FacebookAdParser::ParseResult::INVALID_STRING:
            os << "ParseResult::INVALID_STRING";
            break;
        case FacebookAdParser::ParseResult::PARSE_ERROR:
            os << "ParseResult::PARSE_ERROR";
            break;
    }

    return os;
}

void AssertEq(const FacebookAd& expected, const FacebookAd& actual) {
    ASSERT_EQ(expected.creationTime.ISO8601Timestamp(), actual.creationTime.ISO8601Timestamp());
    ASSERT_EQ(expected.deliveryStartTime.ISO8601Timestamp(), actual.deliveryStartTime.ISO8601Timestamp());
    ASSERT_EQ(expected.deliveryEndTime.ISO8601Timestamp(), actual.deliveryEndTime.ISO8601Timestamp());

    ASSERT_EQ(expected.impressions.lower_bound, actual.impressions.lower_bound);
    ASSERT_EQ(expected.impressions.upper_bound, actual.impressions.upper_bound);

    ASSERT_EQ(expected.spend.lower_bound, actual.spend.lower_bound);
    ASSERT_EQ(expected.spend.upper_bound, actual.spend.upper_bound);

    ASSERT_EQ(expected.fundingEntity, actual.fundingEntity);
    ASSERT_EQ(expected.pageName, actual.pageName);
    ASSERT_EQ(expected.currency, actual.currency);
    ASSERT_EQ(expected.linkTitles, actual.linkTitles);
    ASSERT_EQ(expected.linkDescriptions, actual.linkDescriptions);
    ASSERT_EQ(expected.bodies, actual.bodies);

    ASSERT_EQ(expected.regionDist.size(), actual.regionDist.size());
    for (auto expIt = expected.regionDist.begin(), actualIt = actual.regionDist.begin();
         expIt != expected.regionDist.end() && actualIt != actual.regionDist.end();
         ++expIt, ++actualIt)
    {
        ASSERT_EQ(expIt->first, actualIt->first);
        ASSERT_EQ(expIt->second, actualIt->second);
    }

    ASSERT_EQ(expected.demographicDist.size(), actual.demographicDist.size());
    for (auto expIt = expected.demographicDist.begin(), actualIt = actual.demographicDist.begin();
         expIt != expected.demographicDist.end() && actualIt != actual.demographicDist.end();
         ++expIt, ++actualIt)
    {
        ASSERT_EQ(expIt->first.gender, actualIt->first.gender);
        ASSERT_EQ(expIt->first.age, actualIt->first.age);
        ASSERT_EQ(expIt->second, actualIt->second);
    }
}

#endif //ELECTIONDATAANAL_TEST_UTILS_H

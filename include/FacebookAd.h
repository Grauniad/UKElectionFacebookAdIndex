#ifndef ELECTIONDATAANAL_INCLUDE_FACEBOOK_AD_H
#define ELECTIONDATAANAL_INCLUDE_FACEBOOK_AD_H

#include <string>
#include <map>
#include <vector>
#include <util_time.h>

struct BoundedQuantity {
    size_t lower_bound = 0;
    size_t upper_bound = 0;
};

enum class RegionCode {
    ENGLAND,
    WALES,
    NORTHERN_IRELAND,
    SCOTLAND,
    UNKNOWN
};

// before anyone gets cross - these are Facebook's categories
enum class GenderCode {
    FEMALE,
    MALE,
    UNKNOWN
};

enum class AgeRangeCode {
    AGE_18_TO_24,
    AGE_25_TO_34,
    AGE_35_TO_44,
    AGE_45_TO_54,
    AGE_55_TO_64,
    AGE_65_AND_OVER,
    UNKNOWN
};

struct Demographic {
    GenderCode gender;
    AgeRangeCode age;

    bool operator<(const Demographic& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const RegionCode& code);
RegionCode ParseRegionCode(const std::string& code);

std::ostream& operator<<(std::ostream& os, const GenderCode & code);
GenderCode ParseGenderCode(const std::string& code);

std::ostream& operator<<(std::ostream& os, const AgeRangeCode & code);
AgeRangeCode ParseAgeRangeCode(const std::string& code);

struct  FacebookAd {
    size_t id;

    FacebookAd() = default;
    FacebookAd(const FacebookAd& rhs) = default;
    FacebookAd(FacebookAd&& rhs) = default;
    FacebookAd& operator=(FacebookAd&& rhs) = default;
    FacebookAd& operator=(const FacebookAd& rhs) = default;

    using Time = nstimestamp::Time;

    Time creationTime;
    Time deliveryStartTime;
    Time deliveryEndTime;

    BoundedQuantity impressions;
    BoundedQuantity spend;

    std::string pageUrl;
    std::string fundingEntity;
    std::string pageName;

    std::string currency;
    std::vector<std::string> linkTitles;
    std::vector<std::string> linkDescriptions;
    std::vector<std::string> linkCaptions;
    std::vector<std::string> bodies;

    std::map<RegionCode, double> regionDist;
    std::map<Demographic, double> demographicDist;

};

#endif

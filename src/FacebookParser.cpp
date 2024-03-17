#include <FacebookParser.h>
#include <../internal_includes/FacebookAdJSON.h>
#include <logger.h>
#include <istream>
#include <limits>

using namespace FacebookAdJSON;
using namespace FacebookAdJSON::data_fields;
using namespace nstimestamp;
using Parser = FacebookAdParser;
namespace {
    struct ParserData: public FacebookAdParser::InternalDataHandle {
        QueryResultJSON parser;

        std::stringstream intermediate;

        size_t NumAds() {
            return parser.Get<FacebookAdJSON::data>().size();
        }

        template<class Field>
        auto& GetAdField(size_t adIndex) {
            return parser.Get<FacebookAdJSON::data>()[adIndex]->template Get<Field>();
        }

        template<class Field>
        bool AdFieldSupplied(size_t adIndex) {
            return parser.Get<FacebookAdJSON::data>()[adIndex]->template Supplied<Field>();
        }

        template<class SubObject, class Field>
        auto& GetAdObjectField(size_t adIndex) {
            return parser.Get<FacebookAdJSON::data>()[adIndex]->template Get<SubObject>(). template Get<Field>();
        }

        template<class SubObject, class Field, class Target>
        void ConvertAdObjectField(size_t adIndex, Target& target) {
            // TODO: This will be super painful on the profile buts its not worth solving
            //       until we've solved item - by - item array parsing...
            intermediate.clear();
            intermediate.str(GetAdObjectField<SubObject, Field>(adIndex));
            intermediate >> target;
        }

        template<class SubObject>
        void ReadBoundFields(size_t adIndex, BoundedQuantity& qty) {
            ConvertAdObjectField<SubObject, data_fields::lower_bound>( adIndex, qty.lower_bound);
            qty.upper_bound = qty.lower_bound + 2;
            ConvertAdObjectField<SubObject, data_fields::upper_bound>( adIndex, qty.upper_bound);
        }

        template<class Field, class DestinationType>
        void MoveField(size_t adIndex, DestinationType& target) {
            target = std::move(GetAdField<Field>(adIndex));
        }

        template<class Field>
        void MoveField(size_t adIndex, Time& target) {
            target = GetAdField<Field>(adIndex);
        }
    };

    ParserData& GetParser(FacebookAdParser::InternalDataHandle& hndl) {
        return dynamic_cast<ParserData&>(hndl);
    }

    void ParseDemographics(size_t adIndex, ParserData& parserData, FacebookAd& destination) {
        auto& demoList = parserData.GetAdField<demographic_distribution>(adIndex);
        for (auto& demo: demoList) {
            Demographic demoKey;
            demoKey.gender =  ParseGenderCode(demo->Get<gender>());
            demoKey.age =  ParseAgeRangeCode(demo->Get<data_fields::age>());
            std::string numericDist = demo->Get<data_fields::percentage>();

            parserData.intermediate.clear();
            parserData.intermediate.str(numericDist);
            parserData.intermediate >> destination.demographicDist[demoKey];
        }
    }

    void ParseRegions(size_t adIndex, ParserData& parserData, FacebookAd& destination) {
        auto& regionList = parserData .GetAdField<delivery_by_region>(adIndex);
        for (auto& reg: regionList) {
            RegionCode code = ParseRegionCode(reg->Get<data_fields::region>());
            std::string numericDist = reg->Get<data_fields::percentage>();

            parserData.intermediate.clear();
            parserData.intermediate.str(numericDist);
            parserData.intermediate >> destination.regionDist[code];
        }
    }

    const std::string& MaxId() {
        thread_local std::string maxId = "";
        if (maxId == "") {
            std::stringstream buf;
            buf << std::numeric_limits<size_t>::max();
            maxId = buf.str();
        }

        return maxId;
    }

    struct InvalidId{
        std::string id;

    };

    struct BadlyFormedData {
        size_t id;
    };

    size_t ExtractId(const nstimestamp::Time& createTime, const std::string& url ) {
        std::string id = "";
        size_t idStart = url.find("id=");
        if (idStart != std::string::npos) {
            idStart += 3;
            const size_t idEnd = url.find("&", idStart);
            if (idEnd != std::string::npos) {
                id = url.substr(idStart, (idEnd - idStart));
            }
        }
        const std::string& maxId = MaxId();
        if (maxId.size() < id.size()) {
            throw InvalidId{id};
        } else if (maxId.size() == id.size() && id > maxId) {
            throw InvalidId{id};
        } else if (id.size() > 0) {
            if (id[0] == '0') {
                // converting to size_t strips leading zeros
                throw InvalidId{id};
            }

            for (size_t i = 0; i < id.size(); ++i) {
                if (id[i] < '0'  || id[i] > '9') {
                    throw InvalidId{id};
                }
            }
        }

        size_t result = 0;
        if (url != "" && id == "") {
            throw InvalidId{url};
        } else if (id == "") {
            result = createTime.EpochSecs();
        } else {
            result = strtoul(id.c_str(), nullptr, 10);
        }


        return result;
    }


    void ParseAd(size_t adIndex, vector<std::unique_ptr<FacebookAd>> &into, ParserData &parserData) {
        auto& ad = *into.emplace_back(std::make_unique<FacebookAd>());
        parserData.MoveField<ad_creation_time>(adIndex, ad.creationTime);
        parserData.MoveField<ad_creative_bodies>(adIndex, ad.bodies);
        parserData.MoveField<ad_creative_link_captions>(adIndex, ad .linkCaptions);
        parserData.MoveField<ad_creative_link_descriptions>(adIndex, ad .linkDescriptions);
        parserData.MoveField<ad_creative_link_titles>(adIndex, ad.linkTitles);
        parserData.MoveField<ad_delivery_start_time>(adIndex, ad.deliveryStartTime);
        parserData.MoveField<ad_delivery_stop_time>(adIndex, ad.deliveryEndTime);
        parserData.MoveField<currency>(adIndex, ad.currency);
        parserData.MoveField<bylines>(adIndex, ad.fundingEntity);
        parserData.MoveField<page_name>(adIndex, ad.pageName);

        if (parserData.AdFieldSupplied<id>(adIndex)) {
            parserData.MoveField<id>(adIndex, ad.id);
        } else {
            parserData.MoveField<ad_snapshot_url>(adIndex, ad.pageUrl);
            ad.id = ExtractId(ad.creationTime, ad.pageUrl);
            ad.pageUrl = "";
        }

        parserData.ReadBoundFields<impressions>(adIndex, ad.impressions);
        parserData.ReadBoundFields<spend>(adIndex, ad.spend);

        ParseRegions(adIndex, parserData, ad);
        ParseDemographics(adIndex, parserData, ad);
    }

    template <class T>
    std::string ToString(const T& d) {
        std::stringstream buf;
        buf << d;

        return buf.str();
    }

}
FacebookAdParser::FacebookAdParser() {
    internalData = std::make_unique<ParserData>();
}

Parser::ParseResult Parser::ParseFacebookAdQuery(const char *qryPage, std::vector<std::unique_ptr<FacebookAd>> &ads) {
    std::string error;
    ParseResult result = ParseResult::VALID;
    auto& parserData = GetParser(*this->internalData);

    parserData.parser.Clear();
    if (qryPage == nullptr) {
        result = ParseResult::INVALID_STRING;
    } else if (!parserData.parser.Parse(qryPage, error)) {
        result = ParseResult::PARSE_ERROR;
        SLOG_FROM(LOG_WARNING, __func__,
                "Failed to parse qry json: " << qryPage << std::endl
                 << "Error was: " << error);
    } else {
        ads.reserve(ads.size() + parserData.NumAds());
        for (size_t i = 0; i < parserData.NumAds(); ++i) {
            try {
                ParseAd(i, ads, parserData);
            } catch (InvalidId& e) {
                SLOG_FROM(LOG_WARNING, __func__,
                          "The id is too long to be stored: " << e.id << std::endl);
                result = ParseResult::PARSE_ERROR;
                break;
            }
        }
    }

    return result;
}

FacebookAdParser::ParseResult FacebookAdParser::Parse(std::istream &source, std::vector<std::unique_ptr<FacebookAd>> &ads) {
    std::string result((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
    return ParseFacebookAdQuery(result.c_str(), ads);
}

std::string FacebookAdParser::Serialize(const FacebookAd &ad) {
    using namespace FacebookAdJSON;
    using namespace FacebookAdJSON::data_fields;

    auto& parserData = GetParser(*this->internalData);
    parserData.parser.Clear();
    auto& next = parserData.parser.Get<FacebookAdJSON::data>().emplace_back();
    next->Get<ad_creative_link_titles>() = ad.linkTitles;
    next->Get<ad_creative_link_captions>() = ad.linkCaptions;
    next->Get<ad_creative_link_descriptions>() = ad.linkDescriptions;
    next->Get<ad_creative_bodies>() = ad.bodies;
    next->Get<ad_creation_time>() = ad.creationTime;
    next->Get<ad_delivery_start_time>() = ad.deliveryStartTime;
    next->Get<ad_delivery_stop_time>() = ad.deliveryEndTime;
    next->Get<page_name>() = ad.pageName;
    next->Get<bylines>() = ad.fundingEntity;
    next->Get<id>() = ad.id;

    parserData.GetAdObjectField<impressions, data_fields::lower_bound>(0) = ToString(ad.impressions.lower_bound);
    parserData.GetAdObjectField<impressions, data_fields::upper_bound>(0) = ToString(ad.impressions.upper_bound);

    parserData.GetAdObjectField<spend, data_fields::lower_bound>(0) = ToString(ad.spend.lower_bound);
    parserData.GetAdObjectField<spend, data_fields::upper_bound>(0) = ToString(ad.spend.upper_bound);
    next->Get<currency>() = ad.currency;

    auto& demos = next->Get<demographic_distribution>();
    demos.reserve(ad.demographicDist.size());
    for (const auto& pair: ad.demographicDist) {
        auto& dnext = demos.emplace_back();
        dnext->Get<age>() = ToString(pair.first.age);
        dnext->Get<gender>() = ToString(pair.first.gender);
        dnext->Get<percentage>() = ToString(pair.second);
    }

    auto& regions = next->Get<delivery_by_region>();
    regions.reserve(ad.regionDist.size());
    for (const auto& pair: ad.regionDist) {
        auto& rnext = regions.emplace_back();
        rnext->Get<region>() = ToString(pair.first);
        rnext->Get<percentage>() = ToString(pair.second);
    }


    return parserData.parser.GetJSONString();
}

FacebookAdParser::ParseResult
FacebookAdParser::DeSerialize(const std::string &ad, std::unique_ptr<FacebookAd> &parsedAd) {
    std::vector<std::unique_ptr<FacebookAd>> ads;
    auto result = ParseFacebookAdQuery(ad.c_str(), ads);
    if (result == ParseResult::VALID && ads.size() == 1) {
        parsedAd = std::move(ads[0]);
    } else if (result == ParseResult::VALID) {
        result = ParseResult::PARSE_ERROR;
    }
    return result;
}


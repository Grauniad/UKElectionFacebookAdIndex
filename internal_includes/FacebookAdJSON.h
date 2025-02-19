#ifndef ELECTIONDATAANAL_FACEBOOKADJSON_H
#define ELECTIONDATAANAL_FACEBOOKADJSON_H

#include <SimpleJSON.h>
#include <util_time.h>

namespace JSONUtils {
    using namespace nstimestamp;
    struct TimeField: public FieldBase {
        typedef Time ValueType;
        ValueType value;

        virtual void Clear() {
            FieldBase::Clear();
            value.InitialiseFromString(nstimestamp::Time::EpochTimestamp, strlen(nstimestamp::Time::EpochTimestamp));
        }

        bool String(const char* str, rapidjson::SizeType length, bool copy) {
            value.InitialiseFromString(str, length);
            return true;
        }

        template <class Builder>
        void AddToJSON(Builder& builder, bool nullIfNotSupplied) {
            if (!supplied && nullIfNotSupplied) {
                builder.AddNullField(Name());
            } else {
                builder.Add(Name(), value.ISO8601Timestamp());
            }
        }
    };
    #define NewTimeField(FieldName) struct FieldName: public JSONUtils::TimeField  { const char * Name() { return #FieldName; } };

}

namespace FacebookAdJSON {
    namespace data_fields {
        NewUI64Field(id);
        NewStringField(ad_creative_body);
        NewStringField(ad_creative_link_caption);
        NewStringField(ad_creative_link_description);
        NewStringField(ad_creative_link_title);

        NewTimeField(ad_creation_time);
        NewTimeField(ad_delivery_start_time);
        NewTimeField(ad_delivery_stop_time);

        NewStringField(ad_snapshot_url);

        NewStringField(currency);
        NewStringField(funding_entity);
        NewStringField(page_name);

        NewStringField(lower_bound);
        NewStringField(upper_bound);

        NewStringField(percentage);
        NewStringField(region);
        NewStringField(age);
        NewStringField(gender);

        namespace impressions_fields {
            typedef SimpleParsedJSON<
                lower_bound,
                upper_bound
            > JSON;
        }
        NewEmbededObject(impressions, impressions_fields::JSON);

        namespace spend_fields {
            typedef SimpleParsedJSON<
                lower_bound,
                upper_bound
            > JSON;
        }
        NewEmbededObject(spend, spend_fields::JSON);
        namespace region_distribution_fields {
            typedef SimpleParsedJSON<
                    percentage,
                    region
            > JSON;
        }
        NewObjectArray(region_distribution, region_distribution_fields::JSON);

        namespace demographic_distribution_fields {
            typedef SimpleParsedJSON<
                    age,
                    gender,
                    percentage
            > JSON;
        }
        NewObjectArray(demographic_distribution, demographic_distribution_fields::JSON);

        typedef SimpleParsedJSON<
            id,
            ad_creation_time,
            ad_creative_body,
            ad_creative_link_caption,
            ad_creative_link_description,
            ad_creative_link_title,
            ad_delivery_start_time,
            ad_delivery_stop_time,
            ad_snapshot_url,
            currency,
            funding_entity,
            impressions,
            spend,
            region_distribution,
            demographic_distribution,
            page_name
        > JSON;
    }
    NewObjectArray(data, data_fields::JSON);

    namespace paging_fields {

        namespace cursors_fields {
            NewStringField(after);

            typedef SimpleParsedJSON<
                after
            > JSON;
        }
        NewEmbededObject(cursors, cursors_fields::JSON);
        NewStringField(next);

        typedef SimpleParsedJSON<
            cursors,
            next
        > JSON;
    }
    NewEmbededObject(paging, paging_fields::JSON);

    typedef SimpleParsedJSON<
        data,
        paging
    > QueryResultJSON;
}

#endif //ELECTIONDATAANAL_FACEBOOKADJSON_H

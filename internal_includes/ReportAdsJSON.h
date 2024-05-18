#ifndef ELECTIONDATAANAL_REPORT_AD_JSON_HE
#define ELECTIONDATAANAL_REPORT_AD_JSON_H

#include <SimpleJSON.h>
namespace ReportJSON {
        NewStringField(ad_creation_time);
        NewStringField(ad_delivery_end_time);
        NewStringField(funding_entity);
        NewUIntField(guestimateImpressions);
        NewUIntField(guestimateSpendGBP);
        NewStringField(ad_delivery_start_time);
        NewStringArrayField(ad_creative_bodies);
        NewStringArrayField(ad_creative_link_captions);
        NewStringArrayField(ad_creative_link_descriptions);
        NewStringArrayField(ad_creative_link_titles);

    namespace data_fields {
        typedef SimpleParsedJSON<
                ad_creative_link_captions,
                ad_creative_bodies,
                ad_creative_link_titles,
                ad_creative_link_descriptions,
                ad_creation_time,
                ad_delivery_start_time,
                ad_delivery_end_time,
                funding_entity,
                guestimateImpressions,
                guestimateSpendGBP
        > JSON;
    }
    NewObjectArray(data, data_fields::JSON);

    typedef SimpleParsedJSON<
            data
    > ReportJSON;
}

#endif

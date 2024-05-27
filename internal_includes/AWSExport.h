//
// Created by lukeh on 11/11/2019.
//
#ifndef ELECTIONDATAANAL_STOREDITEMJSON_H
#define ELECTIONDATAANAL_STOREDITEMJSON_H
#include <SimpleJSON.h>

namespace AWSExport {
    namespace Ad {
        NewI64Field(id);
        NewStringField(body);
        NewStringField(title);
        NewStringField(funder);
        NewStringField(page_name);
        NewStringField(start_date);
        NewStringField(end_date);
        NewI64Field(est_impressions)
        NewI64Field(est_spend)

        using JSON = SimpleParsedJSON<
                id,
                body,
                start_date,
                end_date,
                title,
                funder,
                page_name,
                est_impressions,
                est_spend>;
    }

    NewObjectArray(ads, AWSExport::Ad::JSON);
    using JSON = SimpleParsedJSON<ads>;
}

#endif //ELECTIONDATAANAL_STOREDITEMJSON_H

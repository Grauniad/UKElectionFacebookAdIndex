//
// Created by lukeh on 11/11/2019.
//
#ifndef ELECTIONDATAANAL_STOREDITEMJSON_H
#define ELECTIONDATAANAL_STOREDITEMJSON_H
#include <SimpleJSON.h>

namespace StoredItem {
    NewStringField(item);
    NewStringArrayField(upperTitles);
    NewStringArrayField(upperDescriptions);
    NewStringArrayField(upperCaptions);
    NewStringArrayField(upperBodies);
    NewStringField(upperFundingEntity);
    NewStringField(upperPageName);

    using JSON = SimpleParsedJSON<
            item,
            upperTitles,
            upperDescriptions,
            upperCaptions,
            upperBodies,
            upperFundingEntity,
            upperPageName>;
};

#endif //ELECTIONDATAANAL_STOREDITEMJSON_H

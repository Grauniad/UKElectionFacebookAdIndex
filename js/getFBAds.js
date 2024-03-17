#!/usr/bin/nodejs
/*global $, console */
const axios = require('axios');
const util = require('util');
const fs = require('fs');
function Proxy() {
    this._jar = {};
}
Proxy.prototype = {
    _jar: null,
    get: function(url, cb) {
        console.log(">> GET: " + url);
        axios.get(url, {
            jar: this._jar,
            withCredentials: true
        }).then(response => cb(null, response, response.data))
            .catch(error => cb(error));
    },
    post: function (url, postObj, cb) {
        axios.post(url, postObj, {
            jar: this._jar,
            withCredentials: true
        }).then(response => cb(null, response, response.data))
            .catch(error => cb(error));
    }
}
// One will suffice...
let proxy = new Proxy();

let facebookurl="https://graph.facebook.com/v19.0/ads_archive"
facebookurl+="?access_token=" + process.env.FB_API_TOKEN
facebookurl+="&ad_reached_countries=['GB']"
facebookurl+="&search_terms='Woking'"
facebookurl+="&ad_type=POLITICAL_AND_ISSUE_ADS"
facebookurl+="&ad_active_status=ALL"
//TODO: Need to programatically calculate this date
facebookurl+="&ad_delivery_date_min=2019-11-01&ad_delivery_date_max=2019-11-08"
facebookurl+="&fields=ad_snapshot_url,bylines,delivery_by_region,ad_delivery_start_time,ad_delivery_stop_time,ad_creative_link_descriptions,ad_creative_link_titles,ad_creative_link_captions,ad_creative_bodies,ad_creation_time,spend,impressions,demographic_distribution,publisher_platforms,estimated_audience_size,currency"
facebookurl+="&limit=2"
function Timestamp() {
    let date = new Date()
    let stamp = "";
    stamp += date.getFullYear();
    stamp += date.getMonth() +1;
    stamp += date.getDate();
    stamp += "T";
    if (date.getHours() < 10) {
        stamp += '0' + date.getHours();
    } else {
        stamp += date.getHours();
    }
    if (date.getMinutes() < 10) {
        stamp += '0' + date.getMinutes();
    } else {
        stamp += date.getMinutes();
    }
    if (date.getSeconds() < 10) {
        stamp += '0' + date.getSeconds();
    } else {
        stamp += date.getSeconds();
    }
    return stamp;
}
let basename="data/FacebookAds_"+ Timestamp();
function DumpFile(page, body) {
    let fname = basename + "_" + page
    fs.writeFile(fname, body, function (err, file) {
        if (err) {
            throw err
        }
    });
}
let count = 0;
let pages = 0;
function callback(error, response, body) {
    if (typeof response !== 'undefined' && response.status == 200 ) {
        if ("data" in body) {
            pages+=1;
            count+=body['data'].length;
            DumpFile(pages, JSON.stringify(body));
            console.log("Page " + pages + ": " + count);
            if ("paging" in body) {
                let next = body['paging']['next'];
                proxy.get(next, callback);
            }
        } else {
            console.log ("ERROR: Failed to get data from the Facebook Ad API");
            console.log (body);
            process.exit(1)
        }
    } else {
        console.log(error)
        console.log("FAILED TO GET PAGE!")
    }
}
proxy.get(facebookurl, callback);

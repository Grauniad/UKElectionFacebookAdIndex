#!/usr/bin/nodejs
/*global $, console */
const axios = require('axios');
const util = require('util');
const fs = require('fs');
const {request} = require("axios");
function Proxy() {
    this._jar = {};
    this.resume_cfg = null;
    this.initial_request_time = Date.now();
    this.initial_time = -1;
    this.initial_cputime = -1;
    this.request_count = 0;
    this.last_request_time = null;
    this.min_request_delay_ms = 500;
}
Proxy.prototype = {
    _jar: null,
    get: function(url, cb, on_error) {
        this._schedule_get(1, url, cb, on_error);
    },
    get_breathing_time: function() {
        const now = Date.now();
        if (this.last_request_time) {
            return Math.max(0, this.min_request_delay_ms - now + this.last_request_time);
        } else {
            return this.min_request_delay_ms;
        }
    },
    continue_get: function(url, cb, on_error, query_meta) {
        const now = Date.now();
        const response_delay = (now - this.last_request_time);
        console.log("Response processed in " + response_delay + "ms");
        if (this.initial_time < 0 || this.initial_time >= 100 ||
            this.initial_cputime < 0 || this.initial_cputime >= 100) {
            this.initial_time = query_meta.total_time;
            this.initial_cputime = query_meta.total_cputime;
            this.initial_request_time = this.last_request_time;
            this.request_count = 1;
        } else {
            this.request_count += 1;
        }
        if (query_meta.jail_time > 0) {
            console.log("We've been put in rate limit jail. Serving our time...")
            this._schedule_get(query_meta.jail_time, url, cb, on_error);
        } else if(this.request_count > 1) {
            const rate = (this.request_count) / (now - this.initial_request_time)
            const delta = Math.max(query_meta.total_time - this.initial_time,
                                            query_meta.total_cputime - this.initial_cputime);
            const delta_per_request = (delta / this.request_count);
            const headroom = Math.min(97 - query_meta.total_time,
                                               97 - query_meta.total_cputime);
            const requests_until_breach = headroom / delta_per_request;
            if (headroom <= 0) {
                console.log("Hit cut-off threshold - backing off for 5 mins");
                this.min_request_delay_ms = 1000 * 60 * 5;
            } else {

                if (delta <= 0) {
                    this.min_request_delay_ms = 0.8 * (1/rate);
                } else {
                    // we're eating up our budget, back off if we're likely to run out of requests
                    this.min_request_delay_ms = (1/rate) * Math.max(1, this.request_count / requests_until_breach);

                }
            }

            this._schedule_get(this.get_breathing_time(), url, cb, on_error);

            console.log({
                max_delta: delta,
                request_count: this.request_count,
                request_period: 1/ rate,
                requests_until_breach: requests_until_breach,
                platform: this.resume_cfg.current_platform,
                page: this.resume_cfg.page,
                min_delay: this.min_request_delay_ms
            });

        } else {
            this._schedule_get(this.get_breathing_time(), url, cb, on_error);
            console.log({
                request_count: this.request_count,
                platform: this.resume_cfg.current_platform,
                page: this.resume_cfg.page,
                min_delay: this.min_request_delay_ms
            });
        }
        console.log(query_meta);
    },
    _schedule_get: function(timeout_ms, url, cb, on_error){
        let self = this;
        console.log("Delaying GET for " + timeout_ms / 1000 + "s")
        setTimeout(() => {
            self._get(url, cb, on_error);
        }, timeout_ms);
    },
    _get: function(url, cb, on_error) {
        this.last_request_time = Date.now();
        axios.get(url, {
            jar: this._jar,
            withCredentials: true
        }).then(response => cb(response, response.data))
            .catch(error => on_error(error, url));
    }
}
// One will suffice...
let proxy = new Proxy();

function extract_facebook_meta(response) {
    if (response.headers && response.headers['x-business-use-case-usage']) {
        let status = response.status;
        let statusText = response.statusText;
        if (response['www-authenticate']) {
            statusText += ": " + response['www-authenticate']
        }
        let meta_status = JSON.parse(response.headers['x-business-use-case-usage']);
        /*
         *   0: Return key-value pair is { app_id: [{ ... usage data}]}
         *   1: Return the array of values for the app_id
         *   0: Return the first (and only) entry in the array:
         *
         * See: https://developers.facebook.com/docs/graph-api/overview/rate-limiting/
         *
         * We use:
         *    total_cputime: A % of the cpu time budget we have used. Will throttle when
         *                   we hit 100
         *    total_time:    Similar to total_cputime, but instead "total alloted time".
         *                   Meta are unclear on what this is - but we must also prevent
         *                   This from getting to 100
         *    estimated_time_to_regain_access:
         *                    Time in minutes until we are let of rate-limit jail. This
         *                    will be 0 if we are not in jail.
         */
        let usage_meta = Object.entries(meta_status)[0][1][0];
        return {
            status: status,
            statusText: statusText,
            total_cputime: usage_meta.total_cputime,
            total_time: usage_meta.total_time,
            jail_time: usage_meta.estimated_time_to_regain_access*60*1000,
        }
    } else {
        let error_block = response.data.error;
        return {
            status: error_block.code,
            statusText: error_block.message,
            total_cputime: -1,
            total_time: -1,
            jail_time: -1
        }
    }
}

function start_next_url(config) {
    if (config.platforms_to_get.length > 0) {
        facebookurl="https://graph.facebook.com/v19.0/ads_archive"
        facebookurl+="?access_token=" + process.env.FB_API_TOKEN
        facebookurl+="&ad_reached_countries=['GB']"
        facebookurl+="&search_terms=''"
        facebookurl+="&publisher_platforms=" + config.platforms_to_get[0]
        facebookurl+="&ad_delivery_date_min=2024-05-24"
        facebookurl+="&ad_type=POLITICAL_AND_ISSUE_ADS"
        facebookurl+="&ad_active_status=ALL"
        facebookurl+="&fields=page_name,ad_snapshot_url,bylines,ad_delivery_start_time,ad_delivery_stop_time,ad_creative_link_descriptions,ad_creative_link_titles,ad_creative_link_captions,ad_creative_bodies,ad_creation_time,spend,impressions"
        facebookurl+="&limit=250"
        config.next = facebookurl;
        config.page = 1;
        config.current_platform = config.platforms_to_get[0];
        config.platforms_to_get.splice(0,1)
    } else {
        config.current_platform = "";
        config.next = null
    }

}

function initialise_new_get() {

    let config = {
        base_name: "data/"+ Timestamp(),
        page: 0,
        platforms_to_get: ["FACEBOOK", "INSTAGRAM", "AUDIENCE_NETWORK", "MESSENGER", "WHATSAPP", "OCULUS"],
        next: null
    }

    start_next_url(config);
    return config;

}


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
        stamp += date.getSeconds(); }
    return stamp;
}
function DumpFile(config, body) {
    let fname = config.base_name + config.current_platform + "_" + config.page
    fs.writeFile(fname, body, function (err, file) {
        if (err) {
            throw err
        }
    });
}
function SecureResumeConfig(config, cb) {
    config.page += 1
    let fname = "resume.cfg"
    fs.writeFile(fname, JSON.stringify(config), function (err, file) {
        if (err) {
            throw err
        } else {
            cb()
        }
    });
}
function Resume(cb) {
    let fname = "resume.cfg"
    fs.readFile(fname, "", (err, data) => {
        let config = initialise_new_get();
        try {
            console.log("Found valid resume.cfg, will resume get");
            config = JSON.parse(data);
        } catch {
            console.log("No valid resume.cfg found - will start a new get");
        }
        cb(config);
    });
}
let count = 0;
function callback(response, body) {
    let query_meta = extract_facebook_meta(response)
    if (response.status == 200 ) {
        if ("data" in body) {
            count+=body['data'].length;
            DumpFile(proxy.resume_cfg, JSON.stringify(body));
            if ("paging" in body) {
                proxy.resume_cfg.next = body['paging']['next'];
            } else {
                start_next_url(proxy.resume_cfg);
            }
            if (proxy.resume_cfg.next) {
                SecureResumeConfig(proxy.resume_cfg, () => {
                    proxy.continue_get(proxy.resume_cfg.next, callback, on_error_callback, query_meta);
                });
            }
        } else {
            console.log ("ERROR: Failed to get data from the Facebook Ad API");
            console.log (body);
            process.exit(1)
        }
    } else {
        // This shouldn't happen
        console.log(extract_facebook_meta(response));
        console.log("FAILED TO GET PAGE!")
        process.exit(1)
    }
}
function on_error_callback(error, retry_url) {
    console.log("Failed to get: " + retry_url)
    query_meta = extract_facebook_meta(error.response)
    if (query_meta.jail_time > 0) {
        console.log("We've been put in rate limit jail. Serving our time...")
        proxy.continue_get(retry_url, callback, on_error_callback, query_meta);
    } else {
        console.log("UNKNOWN Error")
        console.log(query_meta)
        console.log("Will pause and re-try once before aborting...")
        setTimeout(() => {
            proxy.continue_get(retry_url, callback, on_error_callback_no_retry, query_meta);
        }, 30*1000);
    }
}

function on_error_callback_no_retry(error, retry_url) {
    console.log("Failed to get: " + retry_url)
    query_meta = extract_facebook_meta(error.response)
    if (query_meta.jail_time > 0) {
        console.log("We've been put in rate limit jail. Serving our time...")
        proxy.continue_get(retry_url, callback, on_error_callback, query_meta);
    } else {
        console.log("UNKNOWN Error")
        console.log("We error'd on the retry - giving up")
        console.log(query_meta)
        process.exit(1)
    }
}

Resume((config) => {
    console.log(config)
    proxy.resume_cfg = config;
    proxy.get(config.next, callback, on_error_callback);
});


function GetDataUrl(indexKey) {
    //return "https://raw.githubusercontent.com/LAHumphreys/UKElectionFacebookAdIndex/master/docs/FrontEnd/data/"  + GetDataSet() + "/" + indexKey + ".json";
    return "data/" + GetDataSet() + "/" + indexKey + ".json";
}
function ClearTable(table) {
    if ($.fn.dataTable.isDataTable(table)) {
        var resultsTable = table.DataTable();
        resultsTable.clear();
        resultsTable.draw();
    }
}
function LoadFilterTable(table, summary) {
    var data = [];
    summary.forEach(function (item, index) {
        let checkbox = '<div class="form-check">'
        checkbox += '<input class="form-check-input" type="checkbox" id="' + item.name + '" value="">';
        checkbox += '</div>';
        row = [checkbox];
        row.push(item.name);
        row.push(item.totalAds);
        row.push(Number(item.guestimateImpressions).toLocaleString('en', {maximumSignificantDigits: 2}));
        row.push("£" + Number(item.guestimateSpendGBP).toLocaleString('en', {maximumSignificantDigits: 2}));
        data.push(row)
    });
    if ($.fn.dataTable.isDataTable(table)) {
        var resultsTable = table.DataTable();
        resultsTable.clear();
        resultsTable.rows.add(data);
        resultsTable.draw();
    } else {
        table.DataTable({
            data: data,
            pageLength: 5,
            order: [[4, "desc"]]
        });
    }
}

function ConsolidateData(table, groupField, keys, cb) {
    let result = {};

    keys.forEach(function (key, index) {
        let ref = GetDataUrl(key);
        $.ajax({
            url: ref
        }).done(function (d) {
            d.data.forEach(function (ad, index) {
                let groupKey = ad[groupField];
                if (!(groupKey in result)) {
                    result[groupKey] = {
                        totalAds: 0,
                        guestimateImpressions: 0,
                        guestimateSpendGBP: 0,
                        ads: []
                    };
                }

                let group = result[groupKey];

                group.totalAds += 1;
                group.guestimateImpressions += ad.guestimateImpressions;
                group.guestimateSpendGBP += ad.guestimateSpendGBP;
                group.ads.push(ad);
            });
            cb(table, result);
        }).fail(function () {
            console.log("Failed to get: " + ref);
        });
    });

}

function build_ad_url(id) {
    let url = "https://www.facebook.com/ads/archive/render_ad/?id=" + id;
    url += "&access_token=" + $('#APIToken')[0].value;
    return url;
}

function GetAdList(ads) {
    let tableTemplate = `
    <table style="width:100%">
        <thead>
        <tr>
           <th> View Add </th>
           <th> Link Details </th>
           <th> Body </th>
           <th> Impressions  </th>
           <th> Spend  </th>
           <th> Ad Running </th>
           </tr>
        </thead>
    </table>
    `;
    let table = $(tableTemplate);
    let data = [];
    ads.forEach(function (ad, idx) {
        let row = [];
        let view_button = "<button type=\"button\" class=\"btn btn-primary\" id=\""
        view_button += ad.id + "\" onclick=\"window.open(build_ad_url('" + ad.id + "'), 'ad_disaply')\">View This Ad</button>"
        let view_frame = "<iframe src='" + build_ad_url(ad.id) + "'> </iframe>"
        let linkDetails = "<p>" + ad.page_name + "</p>";
        ad.ad_creative_link_titles.forEach((content, i) => {
            linkDetails += "<p>" + content + "</p>";
        });
        ad.ad_creative_link_captions.forEach((content, i) => {
            linkDetails += "<p>" + content + "</p>";
        });
        ad.ad_creative_link_descriptions.forEach((content, i) => {
            linkDetails += "<p>" + content + "</p>";
        });
        let body = "";
        ad.ad_creative_bodies.forEach((content, i) => {
            body += "<p>" + content + "</p>";
        });

        row.push(view_button);
        row.push(linkDetails);
        row.push(body);
        row.push(Number(ad.guestimateImpressions).toLocaleString('en', {maximumSignificantDigits: 2}));
        row.push("£" + Number(ad.guestimateSpendGBP).toLocaleString('en', {maximumSignificantDigits: 2}));
        let runTime = "<h5>Start</h5>"
        runTime += ad.ad_delivery_start_time.substr(0, 4);
        runTime += ad.ad_delivery_start_time.substr(5, 2);
        runTime += ad.ad_delivery_start_time.substr(8, 2);
        runTime += "<h5>End</h5>"
        runTime += ad.ad_delivery_end_time.substr(0, 4);
        runTime += ad.ad_delivery_end_time.substr(5, 2);
        runTime += ad.ad_delivery_end_time.substr(8, 2);
        row.push(runTime);
        data.push(row);
    });

    table.DataTable({
        data: data,
        order: [[2, "desc"]],
        pageLength: 500
    });

    return table;
}

function ConfigureViewAds(consolidation) {
    var resultsTable = $("#resultsTable").DataTable();

    // Replace the event handler (We don't want to fire twice!)
    let buttons = $("#resultsTable").find("button");
    buttons.off("click").click(function () {
        var tr = $(this).closest('tr');
        var row = resultsTable.row( tr );

        if ( row.child.isShown() ) {
            // This row is already open - close it
            row.child.hide();
        }
        else {
            // Open this row
            let groupKey = tr.find("td").first().next().text();
            row.child(GetAdList(consolidation[groupKey].ads));
            row.child.show();
        }
    } );
}


function DrawResults(table, consolidation) {
    var data = [];
    for (let groupKey in consolidation) {
        let row = ['<button type="button" class="btn btn-secondary btn-sm">View Ads</button>'];
        row.push(groupKey);
        let group = consolidation[groupKey];
        row.push(group.totalAds);
        row.push(Number(group.guestimateImpressions).toLocaleString('en', {maximumSignificantDigits: 2}));
        row.push("£" + Number(group.guestimateSpendGBP).toLocaleString('en', {maximumSignificantDigits: 2}));
        data.push(row);
    }

    if ($.fn.dataTable.isDataTable(table)) {
        var resultsTable = table.DataTable();
        resultsTable.clear();
        resultsTable.rows.add(data);
        resultsTable.draw();
    } else {
        var resultsTable = table.DataTable({
            data: data,
            order: [[3, "desc"]]
        });
    }
    ConfigureViewAds(consolidation);
    resultsTable.on('draw', function () {
        ConfigureViewAds(consolidation);
    });
}

function LoadResults(table, dataList) {
    let consolidation = ConsolidateData(table, 'funding_entity', dataList, DrawResults);

}

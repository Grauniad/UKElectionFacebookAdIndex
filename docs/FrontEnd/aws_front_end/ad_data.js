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

function ClearTable(table) {
    if ($.fn.dataTable.isDataTable(table)) {
        var resultsTable = table.DataTable();
        resultsTable.clear();
        resultsTable.draw();
    }
}

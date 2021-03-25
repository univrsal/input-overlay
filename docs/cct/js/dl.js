function download_json(file_name, content)
{
    var json_data = new Blob([JSON.stringify(content, null, 4)], {type: 'text/json'});
    if (window.navigator && window.navigator.msSaveOrOpenBlob) { // for IE
        window.navigator.msSaveOrOpenBlob(json_data, file_name);
    } else { // for Non-IE (chrome, firefox etc.)
        var a = document.createElement("a");
        document.body.appendChild(a);
        a.style = "display: none";
        var csvUrl = URL.createObjectURL(json_data);
        a.href = csvUrl;
        a.download = file_name;
        a.click();
        URL.revokeObjectURL(a.href)
        a.remove();
    }
}
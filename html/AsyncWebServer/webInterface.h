
#include <WiFi.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <SD.h>
#include <SPI.h>
#include <ESPmDNS.h>

// function defaults
String humanReadableSize(uint64_t bytes);
String listFiles(bool ishtml, String folder);
String processor(const String& var);
String readLineFromFile(File myFile);

void loopOptionsWebUi();

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void notFound(AsyncWebServerRequest *request);

void configureWebServer();
void startWebUi(bool mode_ap = false);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">

  <!-- CSS sample thanks to @im.nix (Discord) -->
  <style>
    .gg-rename {
      box-sizing: border-box;
      position: relative;
      display: inline-block;
      width: 20px;
      height: 16px;
      transform: scale(var(--ggs,1));
      background:
      linear-gradient(
      to left,currentColor 22px,
      transparent 0)
      no-repeat 6px center/2px 22px
    }

    .gg-rename::after,
      .gg-rename::before {
      content: "";
      display: block;
      box-sizing: border-box;
      position: absolute;
      width: 6px;
      height: 12px;
      border: 2px solid;
      top: 2px
    }

    .gg-rename::before {
      border-right: 0;
      border-top-left-radius: 3px;
      border-bottom-left-radius: 3px
    }

    .gg-rename::after {
      width: 10px;
      border-left: 0;
      border-top-right-radius: 3px;
      border-bottom-right-radius: 3px;
      right: 0
    } 
    .gg-folder {
      cursor: pointer;
      transform: scale(var(--ggs,1))
    }
    .gg-folder,
    .gg-folder::after {
        box-sizing: border-box;
        position: relative;
        display: inline-block;
        width: 22px;
        height: 16px;
        border: 2px solid;
        border-radius: 3px
    }
    .gg-folder::after {
        content: "";
        position: absolute;
        width: 10px;
        height: 4px;
        border-bottom: 0;
        border-top-left-radius: 2px;
        border-top-right-radius: 4px;
        border-bottom-left-radius: 0;
        border-bottom-right-radius: 0;
        top: -5px
    }
    .gg-trash {
      box-sizing: border-box;
      position: relative;
      display: inline-block;
      transform: scale(var(--ggs,1));
      width: 10px;
      height: 12px;
      border: 2px solid transparent;
      box-shadow:
          0 0 0 2px,
          inset -2px 0 0,
          inset 2px 0 0;
      border-bottom-left-radius: 1px;
      border-bottom-right-radius: 1px;
      margin-top: 4px;
      margin-bottom: 2px;
    cursor: pointer;
    }
    .gg-trash::after,
    .gg-trash::before {
        content: "";
        display: block;
        box-sizing: border-box;
        position: absolute
    }
    .gg-trash::after {
        background: currentColor;
        border-radius: 3px;
        width: 16px;
        height: 2px;
        top: -4px;
        left: -5px
    }
    .gg-trash::before {
        width: 10px;
        height: 4px;
        border: 2px solid;
        border-bottom: transparent;
        border-top-left-radius: 2px;
        border-top-right-radius: 2px;
        top: -7px;
        left: -2px
    }
    .gg-arrow-down-r {
        box-sizing: border-box;
        position: relative;
        display: inline-block;
        width: 22px;
        height: 22px;
        border: 2px solid;
        transform: scale(var(--ggs,1));
        cursor: pointer;
        border-radius: 4px
    }
    .gg-arrow-down-r::after,
    .gg-arrow-down-r::before {
        content: "";
        display: block;
        box-sizing: border-box;
        position: absolute;
        bottom: 4px
    }
    .gg-arrow-down-r::after {
        width: 6px;
        height: 6px;
        border-bottom: 2px solid;
        border-left: 2px solid;
        transform: rotate(-45deg);
        left: 6px
    }
    .gg-arrow-down-r::before {
        width: 2px;
        height: 10px;
        left: 8px;
        background: currentColor
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      margin: 0;
      padding: 5px;
      color: #ff3ec8;
      background-color: #202124;
    }

    .container {
      max-width: 800px;
      margin: 5px auto;
      padding: 0 5px;
    }

    h3 {
      margin: 0;
      padding: 10px 0;
      border-bottom: 1px solid #7b007b;
    }
 
    table {
      width: 100%%;
      border-collapse: collapse;
      border-bottom: 1px solid #7b007b;
    }
 
    th, td {
      padding: 5px;
      border-bottom: 1px solid #7b007b;
    }
 
    th {
      text-align: left;
    }
 
    a {
      color: #ffbee0;
      text-decoration: none;
    }
 
    a:hover {
      text-decoration: underline;
    }
 
    button {
      background-color: #303134;
      color: #ff3ec8;
      border: 2px solid;
      padding: 4px 8px;
      border-radius: 4px;
      border-color: #ef007b;
      cursor: pointer;
      margin: 5px;
    }
 
    button:hover {
      background-color: #ffabd7;
    }
 
    #detailsheader, #updetailsheader {
      display: flex;
      justify-content: space-between;
    }
 
    @media (max-width: 768px) {
      body {
        font-size: 14px;
      }
 
      table {
        font-size: 12px;
      }
 
      th, td {
        padding: 5px;
      }
 
      button {
        font-size: 12px;
        padding: 6px 12px;
      }
    }
    th:first-child, td:first-child {
      width: 65%%;
    }
    th:last-child, td:last-child {
      width: 100px;
      text-align: center;
    }
  .float-element {
    position: absolute;
    top: 10px; /* Ajuste conforme necessário */
    right: 10px; /* Ajuste conforme necessário */
    font-size: 16px;
  }
  </style>
</head>
<body>
  <div class="container">
    <div class="float-element"><a onclick="logoutButton()" href='javascript:void(0);'>[X]</a></div>
    <h1 align="center">BRUCE Firmware</h1>
    <p>Firmware for offensive pranks and pentest studies and analysis. For educational purposes only. Don't use in environments where you are not allowed. All responsibilities for irresponsible usage of this firmware rest on your fin, sharky. Sincerely, Bruce.</p>
    <p>Firmware version: %FIRMWARE%</p>
    <p>Free Storage: <span id="freeSD">%FREESD%</span> | Used: <span id="usedSD">%USEDSD%</span> | Total: <span id="totalSD">%TOTALSD%</span></p>
    <p>
    <form id="save" enctype="multipart/form-data" method="post"><input type="hidden" id="actualFolder" name="actualFolder" value="/"></form>
    <button onclick="rebootButton()">Reboot</button>
    <button onclick="WifiConfig()">Usr/Pass</button>
    <button onclick="listFilesButton('/')">SD Files</button>

    </p>
    <p id="detailsheader"></p>
    <p id="status"></p>
    <p id="details"></p>
    <p id="updetailsheader"></p>
    <p id="updetails"></p>
  </div>

<script>
function WifiConfig() {
  let wifiSsid = prompt("Please enter the Username of your network", "admin");
  let wifiPwd = prompt("Please enter the Password of your network", "bruce");
  if (wifiSsid == null || wifiSsid == "" || wifiPwd == null) {
    window.alert("Invalid User or Password");
  } else {
    xmlhttp=new XMLHttpRequest();
    xmlhttp.open("GET", "/wifi?usr=" + wifiSsid + "&pwd=" + wifiPwd, false);
    xmlhttp.send();
    document.getElementById("status").innerHTML = xmlhttp.responseText;
  }
}

function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 500);
}

function rebootButton() {
  if(confirm("Confirm Restart?!")) {
    xmlhttp=new XMLHttpRequest();
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/reboot", true);
    xhr.send();
    }
}

function listFilesButton(folders) {
  xmlhttp=new XMLHttpRequest();
  document.getElementById("actualFolder").value = "";
  document.getElementById("actualFolder").value = folders;

  xmlhttp.onload = function() {
      if (xmlhttp.status === 200) {
          document.getElementById("details").innerHTML = xmlhttp.responseText;
      } else {
          console.error('Erro na requisição: ' + xmlhttp.status);
      }
  };
  xmlhttp.onerror = function() {
      console.error('Erro na rede ou falha na requisição.');
  };

  xmlhttp.open("GET", "/listfiles?folder=" + folders, true);
  xmlhttp.send();  

  document.getElementById("detailsheader").innerHTML = "<h3>Files<h3>";
  document.getElementById("updetailsheader").innerHTML = "<h3>Folder Actions:  <button onclick=\"showUploadButtonFancy('" + folders + "')\">Upload File</button><button onclick=\"showCreateFolder('" + folders + "')\">Create Folder</button><h3>"
  document.getElementById("updetails").innerHTML = "";
}

function renameFile(filePath, oldName) {
  var actualFolder = document.getElementById("actualFolder").value;
  let fileName = prompt("Enter the new name: ", oldName);
  if (fileName == null || fileName == "") {
    window.alert("Invalid Name");
  } else {
    const ajax5 = new XMLHttpRequest();
    const formdata5 = new FormData();
    formdata5.append("filePath", filePath);
    formdata5.append("fileName", fileName);
    ajax5.open("POST", "/rename", false);
    ajax5.send(formdata5);
    document.getElementById("status").innerHTML = ajax5.responseText;
    
    listFilesButton(actualFolder);
  }
}

function downloadDeleteButton(filename, action) {
  var urltocall = "/file?name=" + filename + "&action=" + action;
  var actualFolder = document.getElementById("actualFolder").value;
  var option;
  if (action == "delete") {
    option = confirm("Do you really want to DELETE the file: " + filename + " ?\n\nThis action can't be undone!");
  }

  xmlhttp=new XMLHttpRequest();
  if (option == true || action=="create") {
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();
    document.getElementById("status").innerHTML = xmlhttp.responseText;
    listFilesButton(actualFolder);
  }
  if (action == "download") {
    document.getElementById("status").innerHTML = "";
    window.open(urltocall,"_blank");
  }
}

function showCreateFolder(folders) {
  //document.getElementById("updetailsheader").innerHTML = "<h3>Create new Folder<h3>"
  document.getElementById("status").innerHTML = "";
  var uploadform =
  "<p>Creating folder at: <b>" + folders + "</b>"+
  "<form id=\"create_form\" enctype=\"multipart/form-data\" method=\"post\">" +
  "<input type=\"hidden\" id=\"folder\" name=\"folder\" value=\"" + folders + "\">" + 
  "<input type=\"text\" name=\"foldername\" id=\"foldername\">" +
  "<button onclick=\"CreateFolder()\">Create Folder</button>" +
  "</form></p>";
  document.getElementById("updetails").innerHTML = uploadform;
}

function CreateFolder() {
  var folderName = ""; 
  folderName = document.getElementById("folder").value + "/" + document.getElementById("foldername").value;
  downloadDeleteButton(folderName, 'create');
}

function showUploadButtonFancy(folders) {
  //document.getElementById("updetailsheader").innerHTML = "<h3>Upload File<h3>"
  document.getElementById("status").innerHTML = "";
  var uploadform =
  "<p>Send file to " + folders + "</p>"+
  "<form id=\"upload_form\" enctype=\"multipart/form-data\" method=\"post\">" +
  "<input type=\"hidden\" id=\"folder\" name=\"folder\" value=\"" + folders + "\">" + 
  "<input type=\"file\" name=\"file1\" id=\"file1\" onchange=\"uploadFile('" + folders + "')\"><br>" +
  "<progress id=\"progressBar\" value=\"0\" max=\"100\" style=\"width:100%%;\"></progress>" +
  "<h3 id=\"status\"></h3>" +
  "<p id=\"loaded_n_total\"></p>" +
  "</form>";
  document.getElementById("updetails").innerHTML = uploadform;
}
function _(el) {
  return document.getElementById(el);
}
function uploadFile(folder) {
  var file = _("file1").files[0];
  var folder = _("folder").value;
  // alert(file.name+" | "+file.size+" | "+file.type);
  var formdata = new FormData();
  formdata.append("file1", file);
  formdata.append("folder", folder);
  var ajax = new XMLHttpRequest();
  ajax.upload.addEventListener("progress", progressHandler, false);
  ajax.addEventListener("load", completeHandler, false); // doesnt appear to ever get called even upon success
  ajax.addEventListener("error", errorHandler, false);
  ajax.addEventListener("abort", abortHandler, false);
  ajax.open("POST", "/upload");
  ajax.send(formdata);
}
function progressHandler(event) {
  _("loaded_n_total").innerHTML = "Uploaded " + event.loaded + " bytes";
  var percent = (event.loaded / event.total) * 100;
  _("progressBar").value = Math.round(percent);
  _("status").innerHTML = Math.round(percent) + "%% uploaded... please wait";
  if (percent >= 100) {
    _("status").innerHTML = "Please wait, writing file to filesystem";
  }
}
function completeHandler(event) {
  _("status").innerHTML = "Upload Complete";
  _("progressBar").value = 0;
  var actualFolder = document.getElementById("actualFolder").value
  document.getElementById("status").innerHTML = "File Uploaded";
  listFilesButton(actualFolder);
}
function errorHandler(event) {
  _("status").innerHTML = "Upload Failed";
}
function abortHandler(event) {
  _("status").innerHTML = "inUpload Aborted";
}

window.addEventListener("load", function() {
  listFilesButton("/");
});

</script>
</body>
</html>
)rawliteral";

const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <style>
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      margin: 0;
      padding: 20px;
      color: #ad007b;
      background-color: #202124;
    }
 
    h3 {
      margin: 0;
      padding: 10px 0;
      border-bottom: 1px solid rgba(255, 255, 255, 0.1);
    }
  </style>
</head>
<body>
  <h3><a href="/">Log Back In</a></h3>
</body>
</html>
)rawliteral";


const char page_404[] PROGMEM = R"rawliteral(
<script language="javascript">
<!--
document.write(unescape('%3C%68%74%6D%6C%3E%0A%3C%68%65%61%64%3E%0A%3C%74%69%74%6C%65%3E%53%69%6D%70%6C%65%20%34%30%34%20%45%72%72%6F%72%20%50%61%67%65%20%44%65%73%69%67%6E%3C%2F%74%69%74%6C%65%3E%0A%3C%6C%69%6E%6B%20%68%72%65%66%3D%22%68%74%74%70%73%3A%2F%2F%66%6F%6E%74%73%2E%67%6F%6F%67%6C%65%61%70%69%73%2E%63%6F%6D%2F%63%73%73%3F%66%61%6D%69%6C%79%3D%52%6F%62%6F%74%6F%3A%37%30%30%22%20%72%65%6C%3D%22%73%74%79%6C%65%73%68%65%65%74%22%3E%0A%3C%73%74%79%6C%65%3E%0A%68%31%7B%0A%66%6F%6E%74%2D%73%69%7A%65%3A%38%30%70%78%3B%0A%66%6F%6E%74%2D%77%65%69%67%68%74%3A%38%30%30%3B%0A%74%65%78%74%2D%61%6C%69%67%6E%3A%63%65%6E%74%65%72%3B%0A%66%6F%6E%74%2D%66%61%6D%69%6C%79%3A%20%27%52%6F%62%6F%74%6F%27%2C%20%73%61%6E%73%2D%73%65%72%69%66%3B%0A%7D%0A%68%32%0A%7B%0A%66%6F%6E%74%2D%73%69%7A%65%3A%32%35%70%78%3B%0A%74%65%78%74%2D%61%6C%69%67%6E%3A%63%65%6E%74%65%72%3B%0A%66%6F%6E%74%2D%66%61%6D%69%6C%79%3A%20%27%52%6F%62%6F%74%6F%27%2C%20%73%61%6E%73%2D%73%65%72%69%66%3B%0A%6D%61%72%67%69%6E%2D%74%6F%70%3A%2D%34%30%70%78%3B%0A%7D%0A%70%7B%0A%74%65%78%74%2D%61%6C%69%67%6E%3A%63%65%6E%74%65%72%3B%0A%66%6F%6E%74%2D%66%61%6D%69%6C%79%3A%20%27%52%6F%62%6F%74%6F%27%2C%20%73%61%6E%73%2D%73%65%72%69%66%3B%0A%66%6F%6E%74%2D%73%69%7A%65%3A%31%32%70%78%3B%0A%7D%0A%0A%2E%63%6F%6E%74%61%69%6E%65%72%0A%7B%0A%77%69%64%74%68%3A%33%30%30%70%78%3B%0A%6D%61%72%67%69%6E%3A%20%30%20%61%75%74%6F%3B%0A%6D%61%72%67%69%6E%2D%74%6F%70%3A%31%35%25%3B%0A%7D%0A%3C%2F%73%74%79%6C%65%3E%0A%3C%2F%68%65%61%64%3E%0A%3C%62%6F%64%79%3E%0A%3C%64%69%76%20%63%6C%61%73%73%3D%22%63%6F%6E%74%61%69%6E%65%72%22%3E%0A%3C%68%31%3E%34%30%34%3C%2F%68%31%3E%0A%3C%68%32%3E%50%61%67%65%20%4E%6F%74%20%46%6F%75%6E%64%3C%2F%68%32%3E%0A%3C%70%3E%54%68%65%20%50%61%67%65%20%79%6F%75%20%61%72%65%20%6C%6F%6F%6B%69%6E%67%20%66%6F%72%20%64%6F%65%73%6E%27%74%20%65%78%69%73%74%20%6F%72%20%61%6E%20%6F%74%68%65%72%20%65%72%72%6F%72%20%6F%63%63%75%72%65%64%2E%20%47%6F%20%74%6F%20%3C%61%20%68%72%65%66%3D%22%2F%22%3E%48%6F%6D%65%20%50%61%67%65%2E%3C%2F%61%3E%3C%2F%70%3E%0A%3C%21%2D%2D%20%59%6F%75%20%6A%75%73%74%20%73%63%72%61%74%63%68%65%64%20%74%68%65%20%73%75%72%66%61%63%65%2E%2E%2E%20%73%20%68%20%61%20%72%20%6B%20%79%20%2D%2D%21%3E%0A%3C%2F%64%69%76%3E%0A%3C%2F%62%6F%64%79%3E%0A%3C%2F%68%74%6D%6C%3E'));
//-->
</script>

)rawliteral"; 
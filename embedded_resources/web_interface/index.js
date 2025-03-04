var buttonsInitialized = false;

function WifiConfig() {
  let wifiSsid = prompt("Please enter the Username of your network", "admin");
  let wifiPwd = prompt("Please enter the Password of your network", "M%L4unch3r");
  if (wifiSsid == null || wifiSsid == "" || wifiPwd == null) {
    window.alert("Invalid User or Password");
  } else {
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", "/wifi?usr=" + wifiSsid + "&pwd=" + wifiPwd, false);
    xmlhttp.send();
    document.getElementById("status").innerHTML = xmlhttp.responseText;
  }
}

function serialCmd() {
  let cmd = prompt("Enter a serial command", "");
  if (cmd == null || cmd == "" || cmd == null) {
    window.alert("empty command, nothing sent");
  } else {
    const ajax5 = new XMLHttpRequest();
    const formdata5 = new FormData();
    formdata5.append("cmnd", cmd);
    ajax5.open("POST", "/cm", false);
    ajax5.send(formdata5);
    //document.getElementById("status").innerHTML = ajax5.responseText;
    window.alert(ajax5.responseText);
  }
}

function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function () { window.open("/logged-out", "_self"); }, 500);
}

function rebootButton() {
  if (confirm("Confirm Restart?!")) {
    xmlhttp = new XMLHttpRequest();
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/reboot", true);
    xhr.send();
  }
}

function listFilesButton(folders, fs = 'LittleFS', userRequest = false) {
  xmlhttp = new XMLHttpRequest();
  document.getElementById("actualFolder").value = "";
  document.getElementById("actualFolder").value = folders;
  document.getElementById("actualFS").value = fs;

  xmlhttp.onload = function () {
    if (xmlhttp.status === 200) {
      document.getElementById("details").innerHTML = xmlhttp.responseText;
    } else {
      console.error('Requests Error: ' + xmlhttp.status);
    }
  };
  xmlhttp.onerror = function () {
    console.error('Network error or request failure.');
  };

  xmlhttp.open("GET", "/listfiles?fs=" + fs + "&folder=" + folders, true);
  xmlhttp.send();

  //must first select sd card or littlefs to get access to upload and create
  if (!buttonsInitialized) {
    if (userRequest) {
      if (fs == 'SD') {
        document.getElementById("detailsheader").innerHTML = "<h3>SD Files<h3>";
      } else if (fs == 'LittleFS') {
        document.getElementById("detailsheader").innerHTML = "<h3>LittleFS Files<h3>";
      }

      document.getElementById("updetailsheader").innerHTML = "<h3>Folder Actions:  <button onclick=\"showUploadButtonFancy('" + folders + "')\">Upload File</button><button onclick=\"showCreateFolder('" + folders + "')\">Create Folder</button><button onclick=\"showCreateFile('" + folders + "')\">Create File</button><h3>"
      document.getElementById("updetails").innerHTML = "";
      _("drop-area").style.display = "block";
      buttonsInitialized = true;
      document.getElementById("status").innerHTML = "";
    }
  } else {
    if (userRequest) {
      if (fs == 'SD') {
        document.getElementById("detailsheader").innerHTML = "<h3>SD Files<h3>";
      } else if (fs == 'LittleFS') {
        document.getElementById("detailsheader").innerHTML = "<h3>LittleFS Files<h3>";
      }
    }
  }


}

function renameFile(filePath, oldName) {
  var actualFolder = document.getElementById("actualFolder").value;
  var fs = document.getElementById("actualFS").value;
  let fileName = prompt("Enter the new name: ", oldName);
  if (fileName == null || fileName == "") {
    window.alert("Invalid Name");
  } else {
    const ajax5 = new XMLHttpRequest();
    const formdata5 = new FormData();
    formdata5.append("fs", fs);
    formdata5.append("filePath", filePath);
    formdata5.append("fileName", fileName);
    ajax5.open("POST", "/rename", false);
    ajax5.send(formdata5);
    document.getElementById("status").innerHTML = ajax5.responseText;

    var fs = document.getElementById("actualFS").value;
    listFilesButton(actualFolder, fs, true);
  }
}

function sendIrFile(filePath) {
  if (!confirm("Confirm spamming all codes inside the file?")) return;
  var actualFolder = document.getElementById("actualFolder").value;
  var fs = document.getElementById("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "ir tx_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  document.getElementById("status").innerHTML = ajax5.responseText;
  var fs = document.getElementById("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function sendSubFile(filePath) {
  if (!confirm("Confirm sending the codes inside the file?")) return;
  var actualFolder = document.getElementById("actualFolder").value;
  var fs = document.getElementById("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "subghz tx_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  document.getElementById("status").innerHTML = ajax5.responseText;
  var fs = document.getElementById("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function runJsFile(filePath) {
  if (!confirm("Confirm executing the selected JS script?")) return;
  var actualFolder = document.getElementById("actualFolder").value;
  var fs = document.getElementById("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "js " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  document.getElementById("status").innerHTML = ajax5.responseText;
  var fs = document.getElementById("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function runBadusbFile(filePath) {
  if (!confirm("Confirm executing the selected DuckyScript on the machine connected via USB?")) return;
  var actualFolder = document.getElementById("actualFolder").value;
  var fs = document.getElementById("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "badusb run_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  document.getElementById("status").innerHTML = ajax5.responseText;
  var fs = document.getElementById("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function decryptAndType(filePath) {
  if (!confirm("Type decrypted file contents on the machine connected via USB?")) return;
  if (!cachedPassword) cachedPassword = prompt("Enter decryption password: ", cachedPassword);
  if (!cachedPassword) return;  // cancelled
  var actualFolder = document.getElementById("actualFolder").value;
  var fs = document.getElementById("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "crypto type_from_file " + filePath + " " + cachedPassword);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  document.getElementById("status").innerHTML = ajax5.responseText;
  var fs = document.getElementById("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}
function downloadDeleteButton(filename, action) {
  /* fs actions: create (folder), createfile, delete, download */
  var fs = document.getElementById("actualFS").value;
  var urltocall = "/file?name=" + filename + "&action=" + action + "&fs=" + fs;
  var actualFolder = document.getElementById("actualFolder").value;
  var option;
  if (action == "delete") {
    option = confirm("Do you really want to DELETE the file: " + filename + " ?\n\nThis action can't be undone!");
  }

  xmlhttp = new XMLHttpRequest();
  if (option == true || action == "create" || action == "createfile") {
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();
    document.getElementById("status").innerHTML = xmlhttp.responseText;
    var fs = document.getElementById("actualFS").value;
    listFilesButton(actualFolder, fs, true);
  }

  if (action == "edit") {
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();

    if (xmlhttp.status === 200) {
      document.getElementById("editor").value = xmlhttp.responseText;
      document.getElementById("editor-file").innerHTML = filename;
      document.querySelector('.editor-container').style.display = 'flex';
    } else {
      console.error('Requests Error: ' + xmlhttp.status);
    }
  }

  if (action == "download") {
    document.getElementById("status").innerHTML = "";
    window.open(urltocall, "_blank");
  }
}


function cancelEdit() {
  document.querySelector('.editor-container').style.display = 'none';
  document.getElementById("editor").value = "";
  document.getElementById("status").innerHTML = "";
}

function showCreateFolder(folders) {
  var fs = document.getElementById("actualFS").value;
  var uploadform = "";
  //document.getElementById("updetailsheader").innerHTML = "<h3>Create new Folder<h3>"
  document.getElementById("status").innerHTML = "";
  uploadform =
    "<p>Creating folder at: <b>" + folders + "</b>" +
    "<input type=\"hidden\" id=\"folder\" name=\"folder\" value=\"" + folders + "\">" +
    "<input type=\"text\" name=\"foldername\" id=\"foldername\">" +
    "<button onclick=\"CreateFolder()\">Create Folder</button>" +
    "</p>";
  document.getElementById("updetails").innerHTML = uploadform;
}



function CreateFolder() {
  var folderName = "";
  folderName = document.getElementById("folder").value + "/" + document.getElementById("foldername").value;
  downloadDeleteButton(folderName, 'create');
}

function showCreateFile(folders) {
  var fs = document.getElementById("actualFS").value;
  var uploadform = "";
  //document.getElementById("updetailsheader").innerHTML = "<h3>Create new File<h3>"
  document.getElementById("status").innerHTML = "";
  uploadform =
    "<p>Creating file at: <b>" + folders + "</b>" +
    "<input type=\"hidden\" id=\"folder\" name=\"folder\" value=\"" + folders + "\">" +
    "<input type=\"text\" name=\"filename\" id=\"filename\">" +
    "<button onclick=\"CreateFile()\">Create File</button>" +
    "</p>";
  document.getElementById("updetails").innerHTML = uploadform;
}


function CreateFile() {
  var fileName = "";
  fileName = document.getElementById("folder").value + "/" + document.getElementById("filename").value;
  downloadDeleteButton(fileName, 'createfile');
}


function showUploadButtonFancy(folders) {
  //document.getElementById("updetailsheader").innerHTML = "<h3>Upload File<h3>"
  document.getElementById("status").innerHTML = "";
  var uploadform =
    "<p>Send file to " + folders + "</p>" +
    "<form id=\"upload_form\" enctype=\"multipart/form-data\" method=\"post\">" +
    "<input type=\"hidden\" id=\"folder\" name=\"folder\" value=\"" + folders + "\">" +
    "<input type=\"checkbox\" name=\"encryptCheckbox\" id=\"encryptCheckbox\"> Encrypted<br>" +
    "<input type=\"file\" name=\"file1\" id=\"file1\" onchange=\"uploadFile('" + folders + "', 'SD')\"><br>" +
    "<progress id=\"progressBar\" value=\"0\" max=\"100\" style=\"width:100%;\"></progress>" +
    "<h3 id=\"status\"></h3>" +
    "<p id=\"loaded_n_total\"></p>" +
    "</form>";
  document.getElementById("updetails").innerHTML = uploadform;
}

function _(el) {
  return document.getElementById(el);
}

var cachedPassword = "";

function uploadFile(folder) {
  var fs = document.getElementById("actualFS").value;
  var folder = _("folder").value;
  var files = _("file1").files; // Extract files from input element

  var formdata = new FormData();

  var encrypted = _("encryptCheckbox").checked;
  if (encrypted) {
    cachedPassword = prompt("Enter encryption password (do not lose it, cannot be recovered): ", cachedPassword);
    formdata.append("password", cachedPassword);
  }

  for (var i = 0; i < files.length; i++) {
    formdata.append("files[]", files[i]); // Append each file to form data
  }
  formdata.append("folder", folder);

  var ajax = new XMLHttpRequest();
  ajax.upload.addEventListener("progress", progressHandler, false);
  ajax.addEventListener("load", completeHandler, false);
  ajax.addEventListener("error", errorHandler, false);
  ajax.addEventListener("abort", abortHandler, false);
  ajax.open("POST", "/upload" + fs);
  ajax.send(formdata);
}


function saveFile() {
  var fs = document.getElementById("actualFS").value;
  var folder = document.getElementById("actualFolder").value;
  var fileName = document.getElementById("editor-file").innerText;
  var fileContent = document.getElementById("editor").value;

  const formdata = new FormData();
  formdata.append("fs", fs);
  formdata.append("name", fileName);
  formdata.append("content", fileContent);

  const ajax5 = new XMLHttpRequest();
  ajax5.open("POST", "/edit", false);
  ajax5.send(formdata);

  document.getElementById("status").innerText = ajax5.responseText;
  listFilesButton(folder, fs, true);
}


// Drag and drop event listeners
window.addEventListener("load", function () {
  var dropArea = _("drop-area");
  dropArea.addEventListener("dragenter", dragEnter, false);
  dropArea.addEventListener("dragover", dragOver, false);
  dropArea.addEventListener("dragleave", dragLeave, false);
  dropArea.addEventListener("drop", drop, false);
});

function dragEnter(event) {
  event.stopPropagation();
  event.preventDefault();
  this.classList.add("highlight");
}

function dragOver(event) {
  event.stopPropagation();
  event.preventDefault();
  this.classList.add("highlight");
}

function dragLeave(event) {
  event.stopPropagation();
  event.preventDefault();
  this.classList.remove("highlight");
}
var fileQueue = [];
var currentFileIndex = 0;

function drop(event, folder) {
  event.stopPropagation();
  event.preventDefault();
  _("drop-area").classList.remove("highlight");

  fileQueue = event.dataTransfer.files;
  currentFileIndex = 0;
  var fs = document.getElementById("actualFS").value;

  var uploadform =
    "<p>Send file to " + folder + "</p>" +
    "<form id=\"upload_form\" enctype=\"multipart/form-data\" method=\"post\">" +
    "<progress id=\"progressBar\" value=\"0\" max=\"100\" style=\"width:100%;\"></progress>" +
    "<h3 id=\"status\"></h3>" +
    "<p id=\"loaded_n_total\"></p>" +
    "</form>";
  document.getElementById("updetails").innerHTML = uploadform;

  if (fileQueue.length > 0) {
    uploadNextFile(folder, fs);
  }
}

function uploadNextFile(folder, fs) {
  if (currentFileIndex >= fileQueue.length) {
    console.log("Upload complete");
    listFilesButton(folder, fs, true);
    return;
  }

  var file = fileQueue[currentFileIndex];
  var formdata = new FormData();
  formdata.append("file", file);
  formdata.append("folder", folder);

  var ajax = new XMLHttpRequest();
  ajax.upload.addEventListener("progress", progressHandler, false);
  ajax.addEventListener("load", completeHandler, false);
  ajax.addEventListener("error", errorHandler, false);
  ajax.addEventListener("abort", abortHandler, false);
  ajax.open("POST", "/upload" + fs);
  ajax.send(formdata);
}

function progressHandler(event) {
  _("loaded_n_total").innerHTML = "Uploaded " + event.loaded + " bytes";
  var percent = (event.loaded / event.total) * 100;
  _("progressBar").value = Math.round(percent);
  if (percent >= 100) {
    _("status").innerHTML = "Please wait, writing file to filesystem";
  }
}
function completeHandler(event) {
  _("progressBar").value = 0;
  if (fileQueue.length > 0) {
    currentFileIndex++;
    if (currentFileIndex <= fileQueue.length) {
      document.getElementById("status").innerHTML = "Uploaded " + currentFileIndex + " of " + fileQueue.length + " files.";
    }
    uploadNextFile(document.getElementById("actualFolder").value, document.getElementById("actualFS").value);
  }
  else {
    _("status").innerHTML = "Upload Complete";
    var actualFolder = document.getElementById("actualFolder").value
    document.getElementById("status").innerHTML = "File Uploaded";
    var fs = document.getElementById("actualFS").value;
    listFilesButton(actualFolder, fs, true);
  }
}
function errorHandler(event) {
  _("status").innerHTML = "Upload Failed";
  if (fileQueue.length > 0) {
    currentFileIndex++;
    document.getElementById("status").innerHTML = "Uploaded " + i + " of " + files.length + " files, please wait.";
    uploadNextFile(document.getElementById("actualFolder").value, document.getElementById("actualFS").value);
  }
}
function abortHandler(event) {
  _("status").innerHTML = "inUpload Aborted";
  if (fileQueue.length > 0) {
    currentFileIndex++;
    document.getElementById("status").innerHTML = "Uploaded " + i + " of " + files.length + " files, please wait.";
    uploadNextFile(document.getElementById("actualFolder").value, document.getElementById("actualFS").value);
  }
}

function systemInfo() {
  const xmlhttp = new XMLHttpRequest();

  xmlhttp.onload = function () {
    if (xmlhttp.status === 200) {
      try {
        const data = JSON.parse(xmlhttp.responseText);

        document.getElementById("firmwareVersion").innerHTML = data.BRUCE_VERSION;
        document.getElementById("freeSD").innerHTML = data.SD.free;
        document.getElementById("usedSD").innerHTML = data.SD.used;
        document.getElementById("totalSD").innerHTML = data.SD.total;
        document.getElementById("freeLittleFS").innerHTML = data.LittleFS.free;
        document.getElementById("usedLittleFS").innerHTML = data.LittleFS.used;
        document.getElementById("totalLittleFS").innerHTML = data.LittleFS.total;

      } catch (error) {
        console.error("JSON Parsing Error: ", error);
      }
    } else {
      console.error("Request Error: " + xmlhttp.status);
    }
  };

  xmlhttp.onerror = function () {
    console.error("Network error or request failure.");
  };

  xmlhttp.open("GET", "/systeminfo", true);
  xmlhttp.send();
}

window.addEventListener("load", function () {
  var actualFolder = document.getElementById("actualFolder").value
  var fs = document.getElementById("actualFS").value;
  document.getElementById("status").innerHTML = "Please select the storage you want to manage (SD or LittleFS).";
  listFilesButton(actualFolder, fs, true);
  systemInfo();
});


document.getElementById("editor").addEventListener("keydown", function (e) {
  if (e.key === 's' && e.ctrlKey) {
    e.preventDefault();
    saveFile();
  }

  // tab
  if (e.key === 'Tab') {
    e.preventDefault();
    var cursorPos = document.getElementById("editor").selectionStart;
    var textBefore = document.getElementById("editor").value.substring(0, cursorPos);
    var textAfter = document.getElementById("editor").value.substring(cursorPos);
    document.getElementById("editor").value = textBefore + "  " + textAfter;
    document.getElementById("editor").selectionStart = cursorPos + 2;
    document.getElementById("editor").selectionEnd = cursorPos + 2;
  }

});

document.getElementById("editor").addEventListener("keyup", function (e) {
  if (e.key === 'Escape') {
    cancelEdit();
  }

  // map special characters to their closing pair
  map_chars = {
    "(": ")",
    "{": "}",
    "[": "]",
    '"': '"',
    "'": "'",
    "`": "`",
    "<": ">"
  };

  // if the key pressed is a special character, insert the closing pair
  if (e.key in map_chars) {
    var cursorPos = document.getElementById("editor").selectionStart;
    var textBefore = document.getElementById("editor").value.substring(0, cursorPos);
    var textAfter = document.getElementById("editor").value.substring(cursorPos);
    document.getElementById("editor").value = textBefore + map_chars[e.key] + textAfter;
    document.getElementById("editor").selectionStart = cursorPos;
    document.getElementById("editor").selectionEnd = cursorPos;
  }

});
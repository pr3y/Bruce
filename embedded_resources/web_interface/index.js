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
    _("status").innerHTML = xmlhttp.responseText;
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
    //_("status").innerHTML = ajax5.responseText;
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
  _("actualFolder").value = "";
  _("actualFolder").value = folders;
  _("actualFS").value = fs;
  var PreFolder = folders.substring(0, folders.lastIndexOf('/'));
  if (PreFolder == "") { PreFolder = "/"; }

  xmlhttp.onload = function () {
    console.log(xmlhttp.status);
    if (xmlhttp.status === 200) {
        var responseText = xmlhttp.responseText;
        var lines = responseText.split('\n');
        var tableContent = "<table><tr><th align='left'>Name</th><th style=\"text-align=center;\">Size</th><th></th></tr>\n";
        tableContent += "<tr><th align='left'><a onclick=\"listFilesButton('" + PreFolder + "', '" + fs + "')\" href='javascript:void(0);'>... </a></th><th align='left'></th><th></th></tr>\n";
        var folder = "";
        var foldersArray = [];
        var filesArray = [];
        lines.forEach(function (line) {
            if (line) {
                var type = line.substring(0, 2);
                var path = line.substring(3, line.lastIndexOf(':'));
                var filename = line.substring(3, line.lastIndexOf(':'));
                var size = line.substring(line.lastIndexOf(':') + 1);
                if (type === "pa") {
                    if (path !== "") folder = path + "/";
                } else if (type === "Fo") {
                    foldersArray.push({ path: folder + path, name: filename });
                } else if (type === "Fi") {
                    filesArray.push({ path: folder + path, name: filename, size: size });
                }
            }
        });
        foldersArray.sort((a, b) => a.name.localeCompare(b.name));
        filesArray.sort((a, b) => a.name.localeCompare(b.name));
        foldersArray.forEach(function (item) {
            tableContent += "<tr align='left'><td><a onclick=\"listFilesButton('" + item.path + "', '" + fs + "')\" href='javascript:void(0);'>" + item.name + "</a></td>";
            tableContent += "<td></td>\n";
            tableContent += "<td><i style=\"color: #e0d204;\" class=\"gg-folder\" onclick=\"listFilesButton('" + item.path + "', '" + fs + "')\"></i>&nbsp&nbsp";
            tableContent += "<i style=\"color: #e0d204;\" class=\"gg-rename\" onclick=\"renameFile('" + item.path + "', '" + item.name + "')\"></i>&nbsp&nbsp";
            tableContent += "<i style=\"color: #e0d204;\" class=\"gg-trash\" onclick=\"downloadDeleteButton('" + item.path + "', 'delete')\"></i></td></tr>\n\n";
        });
        filesArray.forEach(function (item) {
            tableContent += "<tr align='left'><td>" + item.name + "</td>\n";
            tableContent += "<td style=\"font-size: 10px; text-align=center;\">" + item.size + "</td>\n";
            tableContent += "<td>";

            if (item.name.substring(item.name.lastIndexOf('.') + 1).toLowerCase() === "sub") {
                tableContent += "<i class=\"gg-data\" onclick=\"sendSubFile(\'" + item.path + "\')\"></i>&nbsp&nbsp\n"
            }
            if (item.name.substring(item.name.lastIndexOf('.') + 1).toLowerCase() === "ir") {
                tableContent += "<i class=\"gg-data\" onclick=\"sendIrFile(\'" + item.path + "\')\"></i>&nbsp&nbsp\n"
            }
            if (["bjs", "js"].includes(item.name.substring(item.name.lastIndexOf('.') + 1).toLowerCase())) {
                tableContent += "<i class=\"gg-data\" onclick=\"runJsFile(\'" + item.path + "\')\"></i>&nbsp&nbsp\n"
            }
            if (["mp3", "wav", "mod", "opus", "aac", "flac"].includes(item.name.substring(item.name.lastIndexOf('.') + 1).toLowerCase())) {
                tableContent += "<i class=\"gg-data\" onclick=\"playAudioFile(\'" + item.path + "\')\"></i>&nbsp&nbsp\n"
            }
            if (item.name.substring(item.name.lastIndexOf('.') + 1).toLowerCase() === "txt") {
                tableContent += "<i class=\"gg-data\" onclick=\"runBadusbFile(\'" + item.path + "\')\"></i>&nbsp&nbsp\n"
            }
            if (item.name.substring(item.name.lastIndexOf('.') + 1).toLowerCase() === "enc") {
                tableContent += "<i class=\"gg-data\" onclick=\"decryptAndType(\'" + item.path + "\')\"></i>&nbsp&nbsp\n"
            }
            tableContent += "<i class=\"gg-arrow-down-r\" onclick=\"downloadDeleteButton('" + item.path + "', 'download')\"></i>&nbsp&nbsp\n";
            tableContent += "<i class=\"gg-rename\" onclick=\"renameFile('" + item.path + "', '" + item.name + "')\"></i>&nbsp&nbsp\n";
            tableContent += "<i class=\"gg-trash\" onclick=\"downloadDeleteButton('" + item.path + "', 'delete')\"></i>&nbsp&nbsp\n";
            tableContent += "<i class=\"gg-pen\"  onclick=\"downloadDeleteButton('" + item.path + "', 'edit')\">\n";
        });
        tableContent += "</td>\n</tr></table>";
        _("details").innerHTML = tableContent;
    } else if(xmlhttp.status>0) {
        console.error('Request Error: ' + xmlhttp.status);
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
        _("detailsheader").innerHTML = "<h3>SD Files</h3>";
      } else if (fs == 'LittleFS') {
        _("detailsheader").innerHTML = "<h3>LittleFS Files</h3>";
      }

      _("updetailsheader").innerHTML = "<div style=\"flex\" ><h3>Actions:</h3>" +
      "<input type='file' id='fil' multiple style='display:none'>" +
      "<input type='file' id='fol' webkitdirectory directory multiple style='display:none'>" +
      "<button onclick=\"_('fil').click()\">Send Files</button>" +
      "<button onclick=\"_('fol').click()\">Send Folders</button>" +
      "<button onclick=\"CreateFolder()\">Create Folder</button>" +
      "<button onclick=\"showCreateFile('" + folders + "')\">Create File</button>" +
      "<input type=\"checkbox\" id=\"encryptCheckbox\" style='display:none'>" +
      "<button id=\"encryptBtn\" onclick=\"_('encryptCheckbox').click(); toggleEncrypt()\">Encrypt uploads</button></div>";

      _("fil").onchange = e => handleFileForm(e.target.files, folders);
      _("fol").onchange = e => handleFileForm(e.target.files, folders);

      _("updetails").innerHTML = "";
      _("drop-area").style.display = "block";
      buttonsInitialized = true;
    }
  } else {
    if (userRequest) {
      if (fs == 'SD') {
        _("detailsheader").innerHTML = "<h3>SD Files<h3>";
      } else if (fs == 'LittleFS') {
        _("detailsheader").innerHTML = "<h3>LittleFS Files<h3>";
      }
      _("status").innerHTML = "";
    }
  }
}
function toggleEncrypt() {
  const encryptCheckbox = _("encryptCheckbox");
  const encryptBtn = _("encryptBtn");

  if (encryptCheckbox.checked) {
    encryptBtn.style.backgroundColor = "yellow";
  } else {
    encryptBtn.style.backgroundColor = "";
  }
}
function renameFile(filePath, oldName) {
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
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
    _("status").innerHTML = ajax5.responseText;

    var fs = _("actualFS").value;
    listFilesButton(actualFolder, fs, true);
  }
}

function sendIrFile(filePath) {
  if (!confirm("Confirm spamming all codes inside the file?")) return;
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "ir tx_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  _("status").innerHTML = ajax5.responseText;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function sendSubFile(filePath) {
  if (!confirm("Confirm sending the codes inside the file?")) return;
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "subghz tx_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  _("status").innerHTML = ajax5.responseText;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function runJsFile(filePath) {
  if (!confirm("Confirm executing the selected JS script?")) return;
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "js run_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  _("status").innerHTML = ajax5.responseText;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function runBadusbFile(filePath) {
  if (!confirm("Confirm executing the selected DuckyScript on the machine connected via USB?")) return;
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "badusb run_from_file " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  _("status").innerHTML = ajax5.responseText;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function playAudioFile(filePath) {
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "play " + filePath);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  _("status").innerHTML = ajax5.responseText;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}

function decryptAndType(filePath) {
  if (!confirm("Type decrypted file contents on the machine connected via USB?")) return;
  if (!cachedPassword) cachedPassword = prompt("Enter decryption password: ", cachedPassword);
  if (!cachedPassword) return;  // cancelled
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  const ajax5 = new XMLHttpRequest();
  const formdata5 = new FormData();
  formdata5.append("cmnd", "crypto type_from_file " + filePath + " " + cachedPassword);
  ajax5.open("POST", "/cm", false);
  ajax5.send(formdata5);
  _("status").innerHTML = ajax5.responseText;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
}
function downloadDeleteButton(filename, action) {
  /* fs actions: create (folder), createfile, delete, download */
  var fs = _("actualFS").value;
  var urltocall = "/file?name=" + filename + "&action=" + action + "&fs=" + fs;
  var actualFolder = _("actualFolder").value;
  var option;
  if (action == "delete") {
    option = confirm("Do you really want to DELETE the file: " + filename + " ?\n\nThis action can't be undone!");
  }

  xmlhttp = new XMLHttpRequest();
  if (option == true || action == "create" || action == "createfile") {
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();
    _("status").innerHTML = xmlhttp.responseText;
    var fs = _("actualFS").value;
    listFilesButton(actualFolder, fs, true);
  }

  if (action == "edit") {
    xmlhttp.open("GET", urltocall, false);
    xmlhttp.send();

    if (xmlhttp.status === 200) {
      _("editor").value = xmlhttp.responseText;
      _("editor-file").innerHTML = filename;
      document.querySelector('.editor-container').style.display = 'flex';
    } else {
      console.error('Requests Error: ' + xmlhttp.status);
    }
  }

  if (action == "download") {
    _("status").innerHTML = "";
    window.open(urltocall, "_blank");
  }
}


function cancelEdit() {
  document.querySelector('.editor-container').style.display = 'none';
  _("editor").value = "";
  _("status").innerHTML = "";
}

function CreateFolder() {
  let ff = prompt("Folder Name", "");
  if (ff == "" || ff == null) {
      window.alert("Invalid Folder Name");
  } else {
      downloadDeleteButton(_("actualFolder").value + "/" + ff, 'create');
  }
}

function showCreateFile(folders) {
  var fs = _("actualFS").value;
  var uploadform = "";
  //_("updetailsheader").innerHTML = "<h3>Create new File<h3>"
  _("status").innerHTML = "";
  uploadform =
    "<p>Creating file at: <b>" + folders + "</b>" +
    "<input type=\"hidden\" id=\"folder\" name=\"folder\" value=\"" + folders + "\">" +
    "<input type=\"text\" name=\"filename\" id=\"filename\">" +
    "<button onclick=\"CreateFile()\">Create File</button>" +
    "</p>";
  _("updetails").innerHTML = uploadform;
}


function CreateFile() {
  var fileName = "";
  fileName = _("folder").value + "/" + _("filename").value;
  downloadDeleteButton(fileName, 'createfile');
}

function _(el) {
  return document.getElementById(el);
}

function saveFile() {
  var fs = _("actualFS").value;
  var folder = _("actualFolder").value;
  var fileName = _("editor-file").innerText;
  var fileContent = _("editor").value;

  const formdata = new FormData();
  formdata.append("fs", fs);
  formdata.append("name", fileName);
  formdata.append("content", fileContent);

  const ajax5 = new XMLHttpRequest();
  ajax5.open("POST", "/edit", false);
  ajax5.send(formdata);

  _("status").innerText = ajax5.responseText;
  listFilesButton(folder, fs, true);
}

function CreateFolder(folders) {
  let ff = prompt("Folder Name", "");
  if (ff == "" || ff == null) {
      window.alert("Invalid Folder Name");
  } else {
      downloadDeleteButton(_("actualFolder").value + "/" + ff, 'create');
  }
}

window.addEventListener("load", function () {
  var dropArea = _("drop-area");
  dropArea.addEventListener("dragenter", dragEnter, false);
  dropArea.addEventListener("dragover", dragOver, false);
  dropArea.addEventListener("dragleave", dragLeave, false);
  dropArea.addEventListener("drop", drop, false);
  var actualFolder = _("actualFolder").value;
  var fs = _("actualFS").value;
  listFilesButton(actualFolder, fs, true);
  systemInfo();
});
function dragEnter(event) { event.preventDefault(); this.classList.add("highlight"); }
function dragOver(event) { event.preventDefault(); this.classList.add("highlight"); }
function dragLeave(event) { event.preventDefault(); this.classList.remove("highlight"); }
var currentFileIndex = 0;
var totalSize = 0;
var totalFiles = 0;
var totalProgress = 0;
function writeSendForm() {
  var uploadform =
      "<p>Sending files</p>" +
      "<div id=\"file-progress-container\"></div>";
  _("updetails").innerHTML = uploadform;
}
async function drop(event) {
  event.preventDefault();
  _("drop-area").classList.remove("highlight");
  const items = event.dataTransfer.items;
  const filesQ = [];
  const promises = [];
  for (let i = 0; i < items.length; i++) {
      const entry = items[i].webkitGetAsEntry();
      if (entry) {
          promises.push(FileTree(entry, "", filesQ));
      }
  }
  await Promise.all(promises);
  handleFileForm(filesQ, _("actualFolder").value);
}
function FileTree(item, path = "", filesQ) {
  return new Promise((resolve) => {
      if (item.isFile) {
          item.file(function (file) {
              const fileWithPath = new File([file], path + file.name, { type: file.type });
              filesQ.push(fileWithPath);
              resolve();
          });
      } else if (item.isDirectory) {
          const dirReader = item.createReader();
          dirReader.readEntries((entries) => {
              const entryPromises = [];
              for (let i = 0; i < entries.length; i++) {
                  entryPromises.push(FileTree(entries[i], path + item.name + "/", filesQ));
              }
              Promise.all(entryPromises).then(resolve);
          });
      } else {
          resolve();
      }
  });
}
let fileQueue = [];
let activeUploads = 0;
const maxConcurrentUploads = 3;
function handleFileForm(files, folder) {
  writeSendForm();
  var fs = _("actualFS");
  fileQueue = Array.from(files);
  totalFiles = fileQueue.length;
  completedFiles = 0;
  activeUploads = 0;
  for (let i = 0; i < maxConcurrentUploads; i++) {
      processNextUpload(fs, folder);
  }
}
function processNextUpload(fs, folder) {
  if (fileQueue.length === 0) {
      if (activeUploads === 0) {
          _("status").innerHTML = "Upload Complete";
          _("updetails").innerHTML = "";
          var actualFolder = _("actualFolder").value;
          listFilesButton(actualFolder,fs);
      }
      return;
  }
  if (activeUploads > maxConcurrentUploads) return;
  const file = fileQueue.shift();
  activeUploads++;
  uploadFile(folder, file,fs)
      .then(() => {
          activeUploads--;
          completedFiles++;
          _("status").innerHTML = `Uploaded ${completedFiles} of ${totalFiles} files.`;
          processNextUpload(fs, folder);
      })
      .catch(() => {
          activeUploads--;
          _("status").innerHTML = "Upload Failed";
          processNextUpload(fs, folder);
      });
}
var cachedPassword = "";

function uploadFile(folder, file, fs) {
  return new Promise((resolve, reject) => {
      const progressBarId = `${file.name}-progressBar`;
      if (!_(progressBarId)) {
          var fileProgressDiv = document.createElement("div");
          fileProgressDiv.innerHTML = `<p>${file.name}: <progress id="${progressBarId}" value="0" max="100" style="width:100%;"></progress></p>`;
          _("file-progress-container").appendChild(fileProgressDiv);
      }
      var formdata = new FormData();
      formdata.append("file", file, file.webkitRelativePath || file.name);
      formdata.append("fs", fs);
      formdata.append("folder", folder);
      var ajax = new XMLHttpRequest();
      ajax.upload.addEventListener("progress", function (event) {
          if (event.lengthComputable) {
              var percent = (event.loaded / event.total) * 100;
              _(progressBarId).value = Math.round(percent);
          }
      }, false);
      ajax.addEventListener("load", () => resolve(), false);
      ajax.addEventListener("error", () => reject(), false);
      ajax.addEventListener("abort", () => reject(), false);
      ajax.open("POST", "/upload");
      ajax.send(formdata);
  });
}

function systemInfo() {
  const xmlhttp = new XMLHttpRequest();

  xmlhttp.onload = function () {
    if (xmlhttp.status === 200) {
      try {
        const data = JSON.parse(xmlhttp.responseText);

        _("firmwareVersion").innerHTML = data.BRUCE_VERSION;
        _("freeSD").innerHTML = data.SD.free;
        _("usedSD").innerHTML = data.SD.used;
        _("totalSD").innerHTML = data.SD.total;
        _("freeLittleFS").innerHTML = data.LittleFS.free;
        _("usedLittleFS").innerHTML = data.LittleFS.used;
        _("totalLittleFS").innerHTML = data.LittleFS.total;

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

_("editor").addEventListener("keydown", function (e) {
  if (e.key === 's' && e.ctrlKey) {
    e.preventDefault();
    saveFile();
  }

  // tab
  if (e.key === 'Tab') {
    e.preventDefault();
    var cursorPos = _("editor").selectionStart;
    var textBefore = _("editor").value.substring(0, cursorPos);
    var textAfter = _("editor").value.substring(cursorPos);
    _("editor").value = textBefore + "  " + textAfter;
    _("editor").selectionStart = cursorPos + 2;
    _("editor").selectionEnd = cursorPos + 2;
  }

});

_("editor").addEventListener("keyup", function (e) {
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
    var cursorPos = _("editor").selectionStart;
    var textBefore = _("editor").value.substring(0, cursorPos);
    var textAfter = _("editor").value.substring(cursorPos);
    _("editor").value = textBefore + map_chars[e.key] + textAfter;
    _("editor").selectionStart = cursorPos;
    _("editor").selectionEnd = cursorPos;
  }

});

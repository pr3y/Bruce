function $(s) {return document.querySelector(s)}
const IS_DEV = (window.location.host === "127.0.0.1:8080");
const T = {
  master: $('#t'),
  fileRow: function () {
    const tmp = document.createElement('template');
    tmp.innerHTML = this.master.content.querySelector('table tr.file-row').outerHTML;
    return tmp.content;
  },
  pathRow: function () {
    const tmp = document.createElement('template');
    tmp.innerHTML = this.master.content.querySelector('table tr.path-row').outerHTML;
    return tmp.content;
  },
  uploadLoading: function () {
    const tmp = document.createElement('template');
    tmp.innerHTML = this.master.content.querySelector('.upload-loading').outerHTML;
    return tmp.content;
  }
};

const EXECUTABLE = {
  ir: "ir tx_from_file",
  sub: "subghz tx_from_file",
  js: "js run_from_file",
  bjs: "js run_from_file",
  txt: "badusb run_from_file",
  mp3: "play",
  wav: "play"
};

const Dialog = {
  _bg: function (show) {
    let bg = $(".dialog-background");
    let dialogs = document.querySelectorAll(".dialog");
    dialogs.forEach((dialog) => {
      if (!dialog.classList.contains("hidden"))
        dialog.classList.add("hidden");
    });
    if (show) {
      bg.classList.remove("hidden");
    } else {
      bg.classList.add("hidden");
    }
  },
  show: function (dialogName) {
    this._bg(true);
    let dialog = $(".dialog." + dialogName);
    dialog.classList.remove("hidden");
  },
  hide: function () {
    this._bg(false);
  },
  showOneInput: function (name) {
    const dbForm = {
      rename: {
        title: "Rename",
        label: `New Filename:`
      },
      createFolder: {
        title: "Create Folder",
        label: `Folder Name:`
      },
      createFile: {
        title: "Create File",
        label: `File Name:`
      },
      serial: {
        title: "Serial Command",
        label: `Enter command:`
      }
    };

    let config = dbForm[name];
    if (!config) {
      alert("Invalid dialog name: " + name);
      console.error("Dialog.showOneInput: Invalid dialog name", name);
      return;
    }

    let dialog = $(".dialog.oinput");
    dialog.querySelector(".oinput-title").textContent = config.title;
    dialog.querySelector(".oinput-label").textContent = config.label;
    dialog.querySelector(".oinput-file-name").textContent = "";
    this.show('oinput');
    dialog.querySelector("#oinput-input").value = "";
    dialog.querySelector("#oinput-input").focus();
    return dialog;
  }
};

async function requestGet (url, data) {
  return new Promise((resolve, reject) => {
    let req = new XMLHttpRequest();
    let realUrl = url;
    if (IS_DEV) realUrl = "/bruce" + url;
    if (data) {
      let urlParams = new URLSearchParams(data);
      realUrl += "?" + urlParams.toString();
    }
    req.open("GET", realUrl, true);
    req.onload = () => {
      if (req.status >= 200 && req.status < 300) {
        resolve(req.responseText);
      } else {
        reject(new Error(`Request failed with status ${req.status}`));
      }
    };
    req.onerror = () => {
      reject(new Error("Network error"))
    };
    req.send();
  });
}

async function requestPost (url, data) {
  return new Promise((resolve, reject) => {
    let fd = new FormData();
    for (let key in data) {
      if (data.hasOwnProperty(key)) fd.append(key, data[key]);
    }

    let realUrl = url;
    if (IS_DEV) realUrl = "/bruce" + url;
    let req = new XMLHttpRequest();
    req.open("POST", realUrl, true);
    req.onload = () => {
      if (req.status >= 200 && req.status < 300) {
        resolve(req.responseText);
      } else {
        reject(new Error(`Request failed with status ${req.status}`));
      }
    };
    req.onerror = () => reject(new Error("Network error"));
    req.send(fd);
  });
}

function stringToId(str) {
  let hash = 0, i, chr;
  if (str.length === 0) return hash.toString();
  for (i = 0; i < str.length; i++) {
    chr   = str.charCodeAt(i);
    hash  = ((hash << 5) - hash) + chr;
    hash |= 0; // Convert to 32bit integer
  }
  return 'id_' + Math.abs(hash);
}

const _queueUpload = [];
let _runningUpload = false;
function appendFileToQueue(files) {
  Dialog.show('upload');
  let d = $(".dialog.upload");
  for (let i = 0; i < files.length; i++) {
    let file = files[i];
    let filename = file.webkitRelativePath || file.name;
    let fileId = stringToId(filename);
    let progressBar = T.uploadLoading();
    progressBar.querySelector(".upload-name").textContent = filename;
    progressBar.querySelector(".upload-loading .bar").setAttribute("id", fileId);

    d.querySelector(".dialog-body").appendChild(progressBar);
  }
}
async function appendDroppedFiles(entry) {
  return new Promise((resolve, reject) => {
    if (entry.isFile) {
      entry.file((file) => {
        let fileWithPath = new File([file], entry.fullPath.substring(1), {type: file.type});
        appendFileToQueue([fileWithPath]);
        _queueUpload.push(fileWithPath);
        resolve();
      });
    } else if (entry.isDirectory) {
      let proms = [];
      let reader = entry.createReader();
      reader.readEntries((entries) => {
        for (let e of entries) proms.push(appendDroppedFiles(e));
      });

      Promise.all(proms).then(resolve);
    }
  })
}
async function uploadFile () {
  if (_queueUpload.length === 0) {
    _runningUpload = false;
    $(".dialog.upload .dialog-body").innerHTML = "";
    fetchSystemInfo();
    fetchFiles(currentDrive, currentPath);
    Dialog.hide();
    return;
  }

  return new Promise((resolve, reject) => {
    _runningUpload = true;
    let file = _queueUpload.shift();
    let fd = new FormData();
    let filename = file.webkitRelativePath || file.name;
    let fileId = stringToId(filename);
    fd.append("file", file, filename);
    fd.append("folder", currentPath);
    fd.append("fs", currentDrive);

    let realUrl = `/upload`;
    if (IS_DEV) realUrl = "/bruce" + realUrl;
    let req = new XMLHttpRequest();
    req.upload.onprogress = (e) => {
      if (e.lengthComputable) {
        var percent = (e.loaded / e.total) * 100;
        $("#" + fileId).style.width = Math.round(percent) + "%";
      }
    };
    req.onload = () => {
      uploadFile();
      if (req.status >= 200 && req.status < 300) {
        resolve(req.responseText);
      } else {
        reject();
      }
    };
    req.onabort = () => reject();
    req.onerror = () => reject();
    req.open("POST", realUrl, true);
    req.send(fd);
  });
}

async function runCommand (cmd) {
  Dialog.show('loading');
  try {
    await requestPost("/cm", {cmnd: cmd});
  } catch (error) {
    alert("Failed to run command: " + error.message);
  } finally {
    Dialog.hide();
  }
}

function renderFileRow(fileList) {
  $("table.explorer tbody").innerHTML = "";
  fileList.split("\n").forEach((line) => {
    let e;
    let [type, name, size] = line.split(":");
    if (size === undefined) return;
    let dPath = ((currentPath.endsWith("/") ? currentPath : currentPath + "/") + name).replace(/\/\//g, "/");
    if (type === "pa") {
      if (dPath === "/") return;
      e = T.pathRow();
      let preFolder = currentPath.substring(0, currentPath.lastIndexOf('/'));
      if (preFolder === "") preFolder = "/";
      e.querySelector(".path-row").setAttribute("data-path", preFolder);
      e.querySelector(".path-row td").classList.add("act-browse");
    } else if (type === "Fi") {
      e = T.fileRow();
      e.querySelector('.file-row').setAttribute("data-file", dPath);
      e.querySelector(".col-name").classList.add("act-edit-file");
      e.querySelector(".col-name").textContent = name;
      e.querySelector(".col-size").textContent = size;
      e.querySelector(".col-action").classList.add("type-file");

      let downloadUrl = `/file?fs=${currentDrive}&name=${encodeURIComponent(dPath)}&action=download`;
      if (IS_DEV) downloadUrl = "/bruce" + downloadUrl;
      e.querySelector(".act-download").setAttribute("download", name);
      e.querySelector(".act-download").setAttribute("href", downloadUrl);

      let extension = name.split('.');
      if (extension.length > 1) {
        extension = extension[extension.length - 1].toLowerCase();
        if (EXECUTABLE[extension]) {
          e.querySelector(".act-play").setAttribute("data-cmd", EXECUTABLE[extension] + " " + dPath);
          e.querySelector(".col-action").classList.add("executable");
        }
      }
    } else if (type === "Fo") {
      e = T.fileRow();
      e.querySelector(".col-name").classList.add("act-browse");
      e.querySelector('.file-row').setAttribute("data-path", dPath);
      e.querySelector(".col-name").textContent = name;
      e.querySelector(".col-action").classList.add("type-folder");
    }
    $("table.explorer tbody").appendChild(e);
  });
}

let currentDrive;
let currentPath;
async function fetchFiles(drive, path) {
  currentDrive = drive;
  currentPath = path;
  $(`.act-browse.active`)?.classList.remove("active");
  $(`.act-browse[data-drive='${drive}']`).classList.add("active");
  $(".current-path").textContent = drive + ":/" + path;
  Dialog.show('loading');
  let req = await requestGet("/listfiles", {
    fs: drive,
    folder: path
  });
  renderFileRow(req);
  Dialog.hide();
}

async function fetchSystemInfo() {
  Dialog.show('loading');
  let req = await requestGet("/systeminfo");
  let info = JSON.parse(req);
  $(".bruce-version").textContent = info.BRUCE_VERSION;
  $(".free-space .free-sd span").innerHTML = `${info.SD.used} / ${info.SD.total}`;
  $(".free-space .free-fs span").innerHTML = `${info.LittleFS.used} / ${info.LittleFS.total}`;
  Dialog.hide();
}

window.ondragenter = () => $(".upload-area").classList.remove("hidden");
$(".upload-area").ondragleave = () => $(".upload-area").classList.add("hidden");
$(".upload-area").ondragover = (e) => e.preventDefault();
$(".upload-area").ondrop = async (e) => {
  e.preventDefault();
  $(".upload-area").classList.add("hidden")
  const items = e.dataTransfer.items;
  if (!items || items.length === 0) return;

  for (let i of items) {
    let entry = i.webkitGetAsEntry();
    if (!entry) continue;
    await appendDroppedFiles(entry);
  }

  if (!_runningUpload) setTimeout(uploadFile, 100);
};

document.querySelectorAll(".inp-uploader").forEach((el) => {
  el.addEventListener("change", async (e) => {
    let files = e.target.files;
    if (!files || files.length === 0) return;

    appendFileToQueue(files);
    _queueUpload.push(...files);
    if (!_runningUpload) uploadFile();

    this.value = "";
  });
});

$(".container").addEventListener("click", async (e) => {
  let browseAction = e.target.closest(".act-browse");
  if (browseAction) {
    e.preventDefault();
    let drive = browseAction.getAttribute("data-drive")
      || currentDrive
      || "LittleFS";
    let path = browseAction.getAttribute("data-path")
      || browseAction.closest("tr").getAttribute('data-path')
      || "/";
    if (drive === currentDrive && path === currentPath) return;

    fetchFiles(drive, path);
    return;
  }

  let editFileAction = e.target.closest(".act-edit-file");
  if (editFileAction) {
    e.preventDefault();
    let file = editFileAction.closest("tr").getAttribute("data-file");
    if (!file) return;
    $(".dialog.editor .editor-file-name").textContent = file;
    $(".dialog.editor .file-content").value = "";

    // Load file content
    Dialog.show('loading');
    let r = await requestGet(`/file?fs=${currentDrive}&name=${encodeURIComponent(file)}&action=edit`);
    $(".dialog.editor .file-content").value = r;
    Dialog.show('editor');
    return;
  }

  let oActionOInput = e.target.closest(".act-oinput");
  if (oActionOInput) {
    e.preventDefault();
    let action = oActionOInput.getAttribute("data-action");
    if (!action) return;

    let filePath = currentPath;
    let d = Dialog.showOneInput(action);
    if (action === "rename") {
      filePath = oActionOInput.closest(".file-row").getAttribute("data-file");
    } else if (action === "serial") {
      filePath = "";
    }

    d.setAttribute("data-cache", `${action}|${filePath}`);
    if (filePath != "") {
      d.querySelector(".oinput-file-name").textContent = ": " + filePath;
    }

    return;
  }

  let actDeleteFile = e.target.closest(".act-delete");
  if (actDeleteFile) {
    e.preventDefault();
    let file = actDeleteFile.closest(".file-row").getAttribute("data-file")
      || actDeleteFile.closest(".file-row").getAttribute("data-path");
    if (!file) return;

    if (!confirm(`Are you sure you want to DELETE ${file}?\n\nTHIS ACTION CANNOT BE UNDONE!`)) return;

    Dialog.show('loading');
    await requestGet("/file", {
      fs: currentDrive,
      action: 'delete',
      name: file
    });
    fetchSystemInfo();
    fetchFiles(currentDrive, currentPath);
    return;
  }

  let actPlay = e.target.closest(".act-play");
  if (actPlay) {
    e.preventDefault();
    let cmd = actPlay.getAttribute("data-cmd");
    if (!cmd) return;

    actPlay.blur();
    await runCommand(cmd);
    return;
  }
});


$(".dialog-background").addEventListener("click", async (e) => {
  e.preventDefault();
  if (e.target.matches(".act-dialog-close")) {
    Dialog.hide(false);
    return;
  }
});

$(".act-save-oinput-file").addEventListener("click", async (e) => {
  let dialog = $(".dialog.oinput");
  let fileInput = $("#oinput-input");
  let fileName = fileInput.value.trim();
  if (!fileName) {
    alert("Filename cannot be empty.");
    return;
  }
  let action = dialog.getAttribute("data-cache");
  if (!action) {
    alert("No action specified.");
    return;
  }

  Dialog.show('loading');
  let refreshList = true;
  let [actionType, path] = action.split("|");
  if (actionType === "rename") {
    await requestPost("/rename", {
      fs: currentDrive,
      filePath: path,
      fileName: fileName
    });
  } else if (actionType === "createFolder") {
    let urlQuery = new URLSearchParams({
      fs: currentDrive,
      action: "create",
      name: path.trimEnd("/") + "/" + fileName,
    });
    await requestGet("/file?" + urlQuery.toString());
  } else if (actionType === "createFile") {
    let urlQuery = new URLSearchParams({
      fs: currentDrive,
      action: "createfile",
      name: path.trimEnd("/") + "/" + fileName,
    });
    await requestGet("/file?" + urlQuery.toString());
  } else if (actionType === "serial") {
    await runCommand(fileName);
    refreshList = false; // No need to refresh file list for serial commands
  }

  if (refreshList) fetchFiles(currentDrive, currentPath);
});

$(".act-save-credential").addEventListener("click", async (e) => {
  let username = $("#cred-username").value.trim();
  let password = $("#cred-password").value.trim();
  if (!username || !password) {
    alert("Username and password cannot be empty.");
    return;
  }

  Dialog.show('loading');
  await requestGet("/wifi", {
    usr: username,
    pwd: password
  });
  Dialog.hide();
  alert("Credentials saved successfully!");
});

$(".act-save-edit-file").addEventListener("click", async (e) => {
  Dialog.show('loading');
  await requestPost("/edit", {
    fs: currentDrive,
    name: $(".dialog.editor .editor-file-name").textContent,
    content: $(".dialog.editor .file-content").value
  });
  Dialog.show('editor');
  return;
});

$(".act-reboot").addEventListener("click", async (e) => {
  e.preventDefault();
  if (!confirm("Are you sure you want to REBOOT the device?")) return;
  Dialog.show('loading');
  await requestGet("/reboot");
  setTimeout(() => {
    location.reload();
  }, 1000);
});

document.addEventListener("keyup", (e) => {
  if (e.key === "Escape") {
    if ($(".dialog-background:not(.hidden)") && !$(".dialog.loading:not(.hidden),.dialog.upload:not(.hidden)")) {
      Dialog.hide();
      return;
    }
  }
});
$(".file-content").addEventListener("keyup", function (e) {
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
    var cursorPos = this.selectionStart;
    var textBefore = this.value.substring(0, cursorPos);
    var textAfter = this.value.substring(cursorPos);
    this.value = textBefore + map_chars[e.key] + textAfter;
    this.selectionStart = cursorPos;
    this.selectionEnd = cursorPos;
  }
});


(async function () {
  await fetchSystemInfo();
  await fetchFiles("LittleFS", "/");
})();

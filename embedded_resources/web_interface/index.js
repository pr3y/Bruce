function $(s) { return document.querySelector(s) }
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
  _fbg: function (dialog, show) {
    let parentDg = dialog.parentElement;
    let dialogs = parentDg.querySelectorAll(".dialog");
    dialogs.forEach((dialogH) => {
      if (!dialogH.classList.contains("hidden"))
        dialogH.classList.add("hidden");
    });
    if (show) {
      parentDg.classList.remove("hidden");
    } else {
      parentDg.classList.add("hidden");
    }
  },
  show: function (dialogName, content = '') {
    let dialog = $(".dialog." + dialogName);
    if (dialogName == 'status') {
      dialog.querySelector(".dialog-body").textContent = content;
    }
    this._fbg(dialog, true);
    dialog.classList.remove("hidden");
  },
  hide: function (dialogName = '', dialog = null) {
    if (!dialog) {
      dialog = $(".dialog." + dialogName);
    }
    if (dialogName == 'status') {
      dialog.querySelector(".dialog-body").textContent = '';
    }
    if (!dialog.classList.contains("hidden")) {
      this._fbg(dialog, false);
    }
  },
  showOneInput: function (name) {
    const dbForm = {
      renameFolder: {
        title: "Rename Folder",
        label: `New Name:`,
        action: "Rename"
      },
      renameFile: {
        title: "Rename File",
        label: `New Name:`,
        action: "Rename"
      },
      createFolder: {
        title: "Create Folder",
        label: `Folder Name:`,
        action: "Create Folder"
      },
      createFile: {
        title: "Create File",
        label: `File Name:`,
        action: "Create File"
      },
      serial: {
        title: "Serial Command",
        label: `Command:`,
        action: "Run"
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
    dialog.querySelector(".act-save-oinput-file").textContent = config.action;
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
        let fileWithPath = new File([file], entry.fullPath.substring(1), { type: file.type });
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
    Dialog.hide('upload');
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
  Dialog.show('status', 'Running...');
  try {
    await requestPost("/cm", { cmnd: cmd });
  } catch (error) {
    alert("Failed to run command: " + error.message);
  } finally {
    Dialog.hide('status');
  }
}

function getSerialCommand(fileName) {
  let extension = fileName.split('.');
  if (extension.length > 1) {
    extension = extension[extension.length - 1].toLowerCase();
    return EXECUTABLE[extension]
  }

  return undefined;
}

function calcHash(str) {
  let hash = 5381;
  str = str.replace(/\r\n/g, '\n').replace(/\r/g, '\n');
  for (let i = 0; i < str.length; i++) {
    hash = ((hash << 5) + hash) ^ str.charCodeAt(i); // djb2 xor variant
    hash = hash >>> 0; // force unsigned 32-bit
  }

  return hash.toString(16).padStart(8, '0');
}

function renderFileRow(fileList) {
  $("table.explorer tbody").innerHTML = "";
  fileList.split("\n").sort((a, b) => {
    let [aFirst, ...aRest] = a.split(':');
    let [bFirst, ...bRest] = b.split(':');

    if (aFirst !== bFirst) {
      return bFirst.localeCompare(aFirst);
    }

    let aRestStr = aRest.join(':').toLowerCase();
    let bRestStr = bRest.join(':').toLowerCase();
    return aRestStr.localeCompare(bRestStr);
  }).forEach((line) => {
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
      e.querySelector('[data-action="rename"]').setAttribute("data-action", "renameFile");
      e.querySelector(".col-name").classList.add("act-edit-file");
      e.querySelector(".col-name").textContent = name;
      e.querySelector(".col-name").setAttribute("title", name);
      e.querySelector(".col-size").textContent = size;
      e.querySelector(".col-action").classList.add("type-file");

      let downloadUrl = `/file?fs=${currentDrive}&name=${encodeURIComponent(dPath)}&action=download`;
      if (IS_DEV) downloadUrl = "/bruce" + downloadUrl;
      e.querySelector(".act-download").setAttribute("download", name);
      e.querySelector(".act-download").setAttribute("href", downloadUrl);

      let serialCmd = getSerialCommand(name);
      if (serialCmd) {
        e.querySelector(".act-play").setAttribute("data-cmd", serialCmd + " " + dPath);
        e.querySelector(".col-action").classList.add("executable");
      }
    } else if (type === "Fo") {
      e = T.fileRow();
      e.querySelector(".col-name").classList.add("act-browse");
      e.querySelector('.file-row').setAttribute("data-path", dPath);
      e.querySelector('[data-action="rename"]').setAttribute("data-action", "renameFolder");
      e.querySelector(".col-name").textContent = name;
      e.querySelector(".col-name").setAttribute("title", name);
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
  Dialog.show('status', 'Loading...');
  let req = await requestGet("/listfiles", {
    fs: drive,
    folder: path
  });
  renderFileRow(req);
  Dialog.hide('status');
}

async function fetchSystemInfo() {
  Dialog.show('status', 'Loading...');
  let req = await requestGet("/systeminfo");
  let info = JSON.parse(req);
  $(".bruce-version").textContent = info.BRUCE_VERSION;
  $(".top_line").textContent = "Bruce " + info.BRUCE_VERSION;
  $(".free-space .free-sd span").innerHTML = `${info.SD.used} / ${info.SD.total}`;
  $(".free-space .free-fs span").innerHTML = `${info.LittleFS.used} / ${info.LittleFS.total}`;
  Dialog.hide('status');
}

async function saveEditorFile(runFile = false) {
  Dialog.show('status', 'Saving...');
  let editor = $(".dialog.editor .file-content");
  let filename = $(".dialog.editor .editor-file-name").textContent.trim();
  if (isModified(editor, true)) {
    await requestPost("/edit", {
      fs: currentDrive,
      name: filename,
      content: editor.value
    });
    $(".act-save-edit-file").disabled = true;
  }

  if (runFile) {
    let serial = getSerialCommand(filename);
    if (serial !== undefined) {
      await runCommand(serial + " " + filename);
    }
  }
  Dialog.hide('status');
}

function isModified(target, updateHash = false) {
  let oldHash = target.getAttribute("data-hash");
  let newHash = calcHash(target.value);
  if (updateHash && oldHash !== newHash) {
    target.setAttribute("data-hash", newHash);
  }
  return oldHash !== newHash;
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

  if (!_runningUpload) setTimeout(() => {
    if (_queueUpload.length === 0) return;
    uploadFile();
  }, 100);
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
    let editor = $(".dialog.editor .file-content");
    let file = editFileAction.closest("tr").getAttribute("data-file");
    if (!file) return;
    $(".dialog.editor .editor-file-name").textContent = file;
    editor.value = "";

    // Load file content
    Dialog.show('status', 'Loading...');
    let r = await requestGet(`/file?fs=${currentDrive}&name=${encodeURIComponent(file)}&action=edit`);
    editor.value = r;
    editor.setAttribute("data-hash", calcHash(r));

    $(".act-save-edit-file").disabled = true;

    let serial = getSerialCommand(file);
    if (serial === undefined) {
      $(".act-run-edit-file").classList.add("hidden");
    } else {
      $(".act-run-edit-file").classList.remove("hidden");
    }

    Dialog.hide('status');
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
    if (action.startsWith("rename")) {
      let row = oActionOInput.closest("tr");
      filePath = row.getAttribute("data-file") || row.getAttribute("data-path");
    } else if (action === "serial") {
      filePath = "";
    }

    d.setAttribute("data-cache", `${action}|${filePath}`);
    if (filePath != "") {
      let fName = filePath.substring(filePath.lastIndexOf("/") + 1);
      let fNameSpan = d.querySelector(".oinput-file-name");
      fNameSpan.textContent = ": " + fName;
      fNameSpan.setAttribute("title", fName);
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

    Dialog.show('status', 'Deleting...');
    await requestGet("/file", {
      fs: currentDrive,
      action: 'delete',
      name: file
    });
    Dialog.hide('status');
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


document.addEventListener("click", async (e) => {
  if (e.target.classList.contains("act-dialog-close")) {
    e.preventDefault();
    Dialog.hide(null, e.target.closest(".dialog"));
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

  let refreshList = true;
  let [actionType, path] = action.split("|");
  if (actionType.startsWith("rename")) {
    Dialog.show('status', 'Renaming...');
    await requestPost("/rename", {
      fs: currentDrive,
      filePath: path,
      fileName: fileName
    });
  } else if (actionType === "createFolder") {
    Dialog.show('status', 'Creating Folder...');
    let urlQuery = new URLSearchParams({
      fs: currentDrive,
      action: "create",
      name: path.trimEnd("/") + "/" + fileName,
    });
    await requestGet("/file?" + urlQuery.toString());
  } else if (actionType === "createFile") {
    Dialog.show('status', 'Creating File...');
    let urlQuery = new URLSearchParams({
      fs: currentDrive,
      action: "createfile",
      name: path.trimEnd("/") + "/" + fileName,
    });
    await requestGet("/file?" + urlQuery.toString());
  } else if (actionType === "serial") {
    Dialog.show('status', 'Running Serial Command...');
    await runCommand(fileName);
    refreshList = false; // No need to refresh file list for serial commands
  }

  if (refreshList) fetchFiles(currentDrive, currentPath);
  Dialog.hide('status');
});

$(".act-save-credential").addEventListener("click", async (e) => {
  let username = $("#cred-username").value.trim();
  let password = $("#cred-password").value.trim();
  if (!username || !password) {
    alert("Username and password cannot be empty.");
    return;
  }

  Dialog.show('status', 'Saving WiFi Credentials...');
  await requestGet("/wifi", {
    usr: username,
    pwd: password
  });
  Dialog.hide('status');
  alert("Credentials saved successfully!");
});

$(".act-save-edit-file").addEventListener("click", async (e) => {
  await saveEditorFile();
});

$(".act-run-edit-file").addEventListener("click", async (e) => {
  await saveEditorFile(true);
});

$(".act-reboot").addEventListener("click", async (e) => {
  e.preventDefault();
  if (!confirm("Are you sure you want to REBOOT the device?")) return;
  Dialog.show('status', 'Rebooting...');
  await requestGet("/reboot");
  setTimeout(() => {
    location.reload();
  }, 1000);
});

window.addEventListener("keydown", async (e) => {
  let key = e.key.toLowerCase()
  if ($(".dialog.editor:not(.hidden)")) { // means editor tab is open
    if ((e.ctrlKey || e.metaKey) && key === "s") {
      e.preventDefault();
      e.stopImmediatePropagation();

      await saveEditorFile();
    } else if (e.altKey && key === "enter") {
      e.preventDefault();
      e.stopImmediatePropagation();

      await saveEditorFile(true);
    }
  }

  if (key === "escape") {
    if ($(".dialog-background:not(.hidden)") && !$(".dialog.loading:not(.hidden),.dialog.upload:not(.hidden)")) {
      if ($(".dialog.editor:not(.hidden)")) {
        let editor = $(".dialog.editor .file-content");
        if (isModified(editor)) {
          if (!confirm("You have unsaved changes. Do you want to discard them?")) {
            return;
          }
        }
      }

      Dialog.hide('editor');
      return;
    }
  }
}, true);

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

  $(".act-save-edit-file").disabled = !isModified(e.target);

});

(async function () {
  await fetchSystemInfo();
  await fetchFiles("LittleFS", "/");
})();

const main_view = document.getElementById('main_view');
const menu_options = document.getElementById('menu_options');
const sub_view = document.getElementById('sub_view');
const display = document.getElementById('display');

function menuItems(items){
	main_view.style.display='block';
	menu_options.style.display='none';
	sub_view.style.display='none';
	var menu = '<div class="main_menu">';
	for (var i = 0; i < items.options.length; i++) {
		var Menu = items.options[i];

		if (items.active == Menu.n) {
			//
			menu = menu + '<div class="menu_icon"><span>'+Menu.label+'</span></div>' + Menu.label;
		}
		console.log(Menu.n+' '+Menu.label);
	}
	menu = menu + '</div>';
	main_view.innerHTML = menu;
}

function MenuOptions(items){
	main_view.style.display='none';
	menu_options.style.display='block';
	sub_view.style.display='none';
	var menu = '<div class="menu_options"><div class="options_title">'+items.menu_title+'</div>';
	for (var i = 0; i < items.options.length; i++) {
		var Menu = items.options[i];
		if (items.active == 0 && Menu.n == 0) {
				menu = menu + '<div class="moptions_item ">'+items.options[items.options.length-1].label+'</div>';
				menu = menu + '<div class="moptions_item selected">'+Menu.label+'</div>';
				menu = menu + '<div class="moptions_item ">'+items.options[1].label+'</div>';
		} else if (items.active == items.options.length-1 && Menu.n == items.options.length-1) {
				menu = menu + '<div class="moptions_item ">'+items.options[items.options.length-2].label+'</div>';
				menu = menu + '<div class="moptions_item selected">'+Menu.label+'</div>';
				menu = menu + '<div class="moptions_item ">'+items.options[0].label+'</div>';
		} else {
			if (items.active == Menu.n) {
				var pline = Menu.n-1;
				if (Menu.n < 1) {
					pline = items.options.length;
				}
				var nline = Menu.n+1;
				if (Menu.n == items.options.length) {
					nline = 0;
				}
				menu = menu + '<div class="moptions_item ">'+items.options[pline].label+'</div>';
				menu = menu + '<div class="moptions_item selected">'+Menu.label+'</div>';
				menu = menu + '<div class="moptions_item ">'+items.options[nline].label+'</div>';
			}
		}
		console.log(Menu.n+' '+Menu.label);
	}
	menu = menu + '</div>';
	menu_options.innerHTML = menu;
}


function subMenuItems(items){
	main_view.style.display='none';
	menu_options.style.display='none';
	sub_view.style.display='block';
	var menu = '<div id="sub_menu">';
	var itemz = 0;
	for (var i = 0; i < items.options.length; i++) {
		var Menu = items.options[i];
		if (items.active == Menu.n) {
			menu = menu + '<div class="smenu_item selected">'+Menu.label+'</div>';
		} else {
			menu = menu + '<div class="smenu_item">'+Menu.label+'</div>';
		}
		console.log(Menu.n+' '+Menu.label);
		itemz = items.active;
	}
	menu = menu + '</div>';
	sub_view.innerHTML = menu;

	if (itemz > 2) {
		document.getElementById('sub_menu').scrollTop = (itemz * 17);
	} else {
		document.getElementById('sub_menu').scrollTop = 0;
	}
}

function setRes(width,height){
	display.style.width=width+'px';
	display.style.height=height+'px';
}

function runCommand(cmd) {
		document.getElementById('device_box').style.display='block';

		let realUrl = "/cm";
		let req = new XMLHttpRequest();
		req.open("POST", realUrl, true);
		req.onload = () => {
		  if (req.status >= 200 && req.status < 300) {
			getOptions();
			if (cmd == "nav sel" || cmd == "nav esc") {
				setTimeout(() => {
					getOptions();
				}, "1000");
			}
		  } else {
		//	reject(new Error(`Request failed with status ${req.status}`));
		  }
		};
	//	req.onerror = () => reject(new Error("Network error"));
		req.send('cmnd='+cmd);
}

function getOptions() {
		let realUrl = "/getoptions";
		let req = new XMLHttpRequest();
		req.open("GET", realUrl, true);
		req.onload = () => {
		  if (req.status >= 200 && req.status < 300) {
			    document.getElementById('control').style.display='block';
				var device_res = JSON.parse(req.responseText);
				setRes(device_res.width,device_res.height);
				if (device_res.menu == "main_menu") {
					menuItems(device_res);
				} else if (device_res.menu == "sub_menu") {
					MenuOptions(device_res);
				} else if (device_res.menu == "regular_menu") {
					subMenuItems(device_res);
				}
		  } else {
		//	reject(new Error(`Request failed with status ${req.status}`));
		  }
		};
	//	req.onerror = () => reject(new Error("Network error"));
		req.send();
}
var control = 0;

function rControl() {
	if (control == 0) {
		control = 1;
		runCommand('nav esc');
	} else {
		control = 0;
	}
}

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
    this.loading.hide();
  },
  loading: {
    show: function (message) {
      $(".loading-area").classList.remove("hidden");
      $(".loading-area .text").textContent = message || "Loading...";
    },
    hide: function () {
      $(".loading-area").classList.add("hidden");
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
  Dialog.loading.show('Running command...');
  try {
    await requestPost("/cm", { cmnd: cmd });
  } catch (error) {
    alert("Failed to run command: " + error.message);
  } finally {
    Dialog.loading.hide();
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
      e.querySelector('.act-rename').setAttribute("data-action", "renameFile");
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
      e.querySelector('.act-rename').setAttribute("data-action", "renameFolder");
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
  Dialog.loading.show('Fetching files...');
  let req = await requestGet("/listfiles", {
    fs: drive,
    folder: path
  });
  renderFileRow(req);
  Dialog.loading.hide();
}

async function fetchSystemInfo() {
  Dialog.loading.show('Fetching system info...');
  let req = await requestGet("/systeminfo");
  let info = JSON.parse(req);
  $(".bruce-version").textContent = info.BRUCE_VERSION;
  $(".free-space .free-sd span").innerHTML = `${info.SD.used} / ${info.SD.total}`;
  $(".free-space .free-fs span").innerHTML = `${info.LittleFS.used} / ${info.LittleFS.total}`;
  Dialog.loading.hide();
}

async function saveEditorFile(runFile = false) {
  Dialog.loading.show('Saving...');
  let editor = $(".dialog.editor .file-content");
  let filename = $(".dialog.editor .editor-file-name").textContent.trim();
  if (isModified(editor)) {
    $(".act-save-edit-file").disabled = true;
    editor.setAttribute("data-hash", calcHash(editor.value));
    await requestPost("/edit", {
      fs: currentDrive,
      name: filename,
      content: editor.value
    });
  }

  if (runFile) {
    let serial = getSerialCommand(filename);
    if (serial !== undefined) {
      await runCommand(serial + " " + filename);
    }
  }
  Dialog.loading.hide();
}

function isModified(target) {
  let oldHash = target.getAttribute("data-hash");
  let newHash = calcHash(target.value);
  return oldHash !== newHash;
}

async function openNavigator() {
  Dialog.show('navigator');
  await reloadScreen();
  autoReloadScreen();
}

let SCREEN_NAVIGATING = false;
async function runNavigation(direction) {
  if (SCREEN_NAVIGATING) return;
  SCREEN_NAVIGATING = true;
  try {
    drawCanvasLoading();
    await requestPost("/cm", { cmnd: `nav ${direction.toLowerCase()}` });
    await reloadScreen();
  } catch (error) {
    alert("Failed to run command: " + error.message);
    console.error(error)
  } finally {
    SCREEN_NAVIGATING = false;
  }
}

const btnForceReload = $("#force-reload");
let SCREEN_RELOAD = false;
async function reloadScreen() {
  if (SCREEN_RELOAD) return;
  SCREEN_RELOAD = true;
  btnForceReload.classList.add("reloading");
  try {
    let binResponse = await fetch((IS_DEV ? "/bruce" : "") + "/getscreen");
    let arrayBuffer = await binResponse.arrayBuffer();
    let screenData = new Uint8Array(arrayBuffer);
    await renderTFT(screenData);
  } catch (error) {
    console.error("Failed to reload screen:", error);
    alert("Failed to reload screen: " + error.message);
  } finally {
    btnForceReload.classList.remove("reloading");
    SCREEN_RELOAD = false;
  }
}

const eConfigAutoReload = $("#navigator-auto-reload");
let AUTO_RELOAD_SCREEN = null;
async function taskReloader() {
  let timer = parseInt(eConfigAutoReload.value);
  let navigatorOpen = $(".dialog.navigator:not(.hidden)");
  if (timer <= 0 || !navigatorOpen) {
    if (AUTO_RELOAD_SCREEN) {
      clearTimeout(AUTO_RELOAD_SCREEN);
      AUTO_RELOAD_SCREEN = null;
    }

    return;
  }


  await reloadScreen();
  setTimeout(taskReloader, timer);
  // better use setTimeout instead of setInterval to avoid overlapping calls
}
async function autoReloadScreen() {
  let timer = parseInt(eConfigAutoReload.value);

  if (AUTO_RELOAD_SCREEN) {
    clearTimeout(AUTO_RELOAD_SCREEN);
    AUTO_RELOAD_SCREEN = null;
  }

  if (timer > 0) taskReloader();
}

/// TFT RENDER
let loadingDrawn = false;
const imageCache = {}; // global
async function renderTFT(data) {
  loadingDrawn = false;
  const canvas = $("#navigator-screen");
  const ctx = canvas.getContext("2d");

  const loadImage = async (url) => {
    if (imageCache[url]) return imageCache[url];
    return new Promise((resolve, reject) => {
      const img = new Image();
      img.onload = () => {
        imageCache[url] = img;
        resolve(img);
      };
      img.onerror = (err) => reject(err);
      img.src = url;
    });
  }

  const drawImageCached = async (img_url, input) => {
    if (IS_DEV) img_url = "/bruce" + img_url;
    let img = await loadImage(img_url);
    let drawX = input.x;
    let drawY = input.y;

    if (input.center === 1) {
      drawX += (canvas.width-img.width) / 2;
      drawY += (canvas.height-img.height) / 2;
    }
    ctx.drawImage(img, drawX, drawY);
  }

  const color565toCSS = (color565) => {
    const r = ((color565 >> 11) & 0x1F) * 255 / 31;
    const g = ((color565 >> 5) & 0x3F) * 255 / 63;
    const b = (color565 & 0x1F) * 255 / 31;
    return `rgb(${r},${g},${b})`;
  };

  const drawRoundRect = (ctx, input, fill) => {
    const { x, y, w, h, r } = input;
    ctx.beginPath();
    ctx.moveTo(x + r, y);
    ctx.arcTo(x + w, y, x + w, y + h, r);
    ctx.arcTo(x + w, y + h, x, y + h, r);
    ctx.arcTo(x, y + h, x, y, r);
    ctx.arcTo(x, y, x + w, y, r);
    ctx.closePath();
    if (fill) ctx.fill(); else ctx.stroke();
  };

  let startData = 0;
  const getByteValue = (dataType) => {
    if (dataType === 'int8') {
      return data[startData++];
    } else if (dataType === 'int16') {
      let value = (data[startData] << 8) | data[startData + 1];
      startData += 2;
      return value;
    } else if (dataType.startsWith("s")) {
      let strLength = parseInt(dataType.substring(1));
      let strBytes = data.slice(startData, startData + strLength);
      startData += strLength;
      return new TextDecoder().decode(strBytes);
    }
  }

  const byteToObject = (fn, size) => {
    let keysMap = {
      0: ["fg"],                                                  // FILLSCREEN
      1: ["x", "y", "w", "h", "fg"],                              // DRAWRECT
      2: ["x", "y", "w", "h", "fg"],                              // FILLRECT
      3: ["x", "y", "w", "h", "r", "fg"],                         // DRAWROUNDRECT
      4: ["x", "y", "w", "h", "r", "fg"],                         // FILLROUNDRECT
      5: ["x", "y", "r", "fg"],                                   // DRAWCIRCLE
      6: ["x", "y", "r", "fg"],                                   // FILLCIRCLE
      7: ["x", "y", "x2", "y2", "x3", "y3", "fg"],                // DRAWTRIANGLE
      8: ["x", "y", "x2", "y2", "x3", "y3", "fg"],                // FILLTRIANGLE
      9: ["x", "y", "rx", "ry", "fg"],                            // DRAWELLIPSE
      10: ["x", "y", "rx", "ry", "fg"],                           // FILLELLIPSE
      11: ["x", "y", "x1", "y1", "fg"],                           // DRAWLINE
      12: ["x", "y", "r", "ir", "startAngle", "endAngle", "fg", "bg"],  // DRAWARC
      13: ["x", "y", "bx", "by", "wd", "fg", "bg"],               // DRAWWIDELINE
      14: ["x", "y", "size", "fg", "bg", "txt"],                  // DRAWCENTRESTRING
      15: ["x", "y", "size", "fg", "bg", "txt"],                  // DRAWRIGHTSTRING
      16: ["x", "y", "size", "fg", "bg", "txt"],                  // DRAWSTRING
      17: ["x", "y", "size", "fg", "bg", "txt"],                  // PRINT
      18: ["x", "y", "center", "ms", "fs", "file"],                // DRAWIMAGE
      20: ["x", "y", "h", "fg"],                                  // DRAWFASTVLINE
      21: ["x", "y", "w", "fg"],                                   // DRAWFASTHLINE
      99: ["w", "h", "rotation"]                                  // SCREEN_INFO
    };

    let r = {};
    let lengthLeft = size - 3;
    for (let key of keysMap[fn]) {
      if (['txt', 'file'].includes(key)) {
        r[key] = getByteValue(`s${lengthLeft}`);
      } else if (['rotation', 'fs'].includes(key)) {
        lengthLeft -= 1;
        r[key] = getByteValue('int8');
        if (key === 'fs') {
          r[key] = (r[key] === 0) ? "SD" : "FS"; // 0 for SD, 1 for FS
        }
      } else {
        lengthLeft -= 2;
        r[key] = getByteValue('int16');
      }
    }
    return r;
  }

  let offset = 0;
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  while (offset < data.length) {
    ctx.beginPath();
    if (data[offset] !== 0xAA) {
      console.warn("Invalid header at offset", offset);
      break;
    }

    startData = offset + 1;
    let size = getByteValue('int8');
    let fn = getByteValue('int8');
    offset += size;

    let input = byteToObject(fn, size);
     // reset to default before drawing again
    ctx.lineWidth = 1;
    ctx.fillStyle = "black";
    ctx.strokeStyle = "black";
    switch (fn) {
      case 99: // SCREEN_INFO
        canvas.width = input.w;
        canvas.height = input.h;
      case 0: // FILLSCREEN
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        break;

      case 1: // DRAWRECT
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.strokeRect(input.x, input.y, input.w, input.h);
        break;

      case 2: // FILLRECT
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.fillRect(input.x, input.y, input.w, input.h);
        break;

      case 3: // DRAWROUNDRECT
        ctx.strokeStyle = color565toCSS(input.fg);
        drawRoundRect(ctx, input, false);
        break;

      case 4: // FILLROUNDRECT
        ctx.fillStyle = color565toCSS(input.fg);
        drawRoundRect(ctx, input, true);
        break;

      case 5: // DRAWCIRCLE
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.arc(input.x, input.y, input.r, 0, Math.PI * 2);
        ctx.stroke();
        break;

      case 6: // FILLCIRCLE
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.arc(input.x, input.y, input.r, 0, Math.PI * 2);
        ctx.fill();
        break;
      case 7: // DRAWTRIANGLE
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.beginPath();
        ctx.moveTo(input.x, input.y);
        ctx.lineTo(input.x2, input.y2);
        ctx.lineTo(input.x3, input.y3);
        ctx.closePath();
        ctx.stroke();
        break;

      case 8: // FILLTRIANGLE
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.beginPath();
        ctx.moveTo(input.x, input.y);
        ctx.lineTo(input.x2, input.y2);
        ctx.lineTo(input.x3, input.y3);
        ctx.closePath();
        ctx.fill();
        break;
      case 9: // DRAWELLIPSE
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.beginPath();
        ctx.ellipse(input.x, input.y, input.rx, input.ry, 0, 0, Math.PI * 2);
        ctx.stroke();
        break;

      case 10: // FILLELLIPSE
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.beginPath();
        ctx.ellipse(input.x, input.y, input.rx, input.ry, 0, 0, Math.PI * 2);
        ctx.fill();
        break;

      case 11: // DRAWLINE
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.moveTo(input.x, input.y);
        ctx.lineTo(input.x1, input.y1);
        ctx.stroke();
        break;

      case 12: // DRAWARC
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.lineWidth = (input.r - input.ir) || 1;
        const sa = (input.startAngle + 90 || 0) * Math.PI / 180;
        const ea = (input.endAngle + 90 || 0) * Math.PI / 180;
        const radius = (input.r + input.ir) / 2;
        ctx.beginPath();
        ctx.arc(input.x, input.y, radius, sa, ea);
        ctx.stroke();
        break;

      case 13: // DRAWWIDELINE
        ctx.strokeStyle = color565toCSS(input.fg);
        ctx.lineWidth = input.wd || 1;
        ctx.moveTo(input.x, input.y);
        ctx.lineTo(input.bx, input.by);
        ctx.stroke();
        break;

      case 14: // DRAWCENTRESTRING
      case 15: // DRAWRIGHTSTRING
      case 16: // DRAWSTRING
      case 17: // PRINT
        // This must be enhanced to make font width be multiple of 6px, the font used here is multiple of 4.5px,
        // "\n" are not treated, and long lines do not split into multi lines..
        if (input.bg == input.fg) { input.bg = 0; }
        ctx.fillStyle = color565toCSS(input.bg);

        input.txt = input.txt.replaceAll("\\n", ""); // remove new lines
        var fw = input.size === 3 ? 13.5 : input.size === 2 ? 9 : 4.5;
        var o = 0;
        if (fn === 15) o = input.txt.length * fw;
        if (fn === 14) o = input.txt.length * fw / 2;
        // draw a rectangle at the text area, to avoid overlapping texts
        ctx.fillRect(input.x - o, input.y, input.txt.length * fw, input.size * 8);

        ctx.fillStyle = color565toCSS(input.fg);
        ctx.font = `${input.size * 8}px monospace`;
        ctx.textBaseline = "top";
        ctx.textAlign = fn === 14 ? "center" : fn === 15 ? "right" : "left";
        ctx.fillText(input.txt, input.x, input.y);
        break;

      case 18: // DRAWIMAGE
        let url = `/file?fs=${input.fs}&name=${encodeURIComponent(input.file)}&action=image`;
        await drawImageCached(url, input);
        break;

      case 19: // DRAWPIXEL
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.fillRect(input.x, input.y, 1, 1);
        break;
      case 20: // DRAWFASTVLINE
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.fillRect(input.x, input.y, 1, input.h);
        break;

      case 21: // DRAWFASTHLINE
        ctx.fillStyle = color565toCSS(input.fg);
        ctx.fillRect(input.x, input.y, input.w, 1);
        break;
    }
  }
}
function drawCanvasLoading() {
  if (loadingDrawn) return;
  loadingDrawn = true;
  const canvas = $("#navigator-screen");
  const ctx = canvas.getContext("2d");
  const width = canvas.width;
  const height = canvas.height;

  // Draw semi-transparent black background
  ctx.save();
  ctx.globalAlpha = 0.8;
  ctx.fillStyle = "#000";
  ctx.fillRect(0, 0, width, height);
  ctx.globalAlpha = 1.0;

  // Draw "Loading" text in the center
  ctx.fillStyle = "#fff";
  ctx.font = "bold 14px 'DejaVu Sans Mono', Consolas, Menlo";
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";
  ctx.fillText("Navigating...", width / 2, height / 2);
  ctx.restore();
}

let oldTimerSession = sessionStorage.getItem("autoReload") || "0";
eConfigAutoReload.querySelector(`option[value="${oldTimerSession}"]`).selected = true;
eConfigAutoReload.addEventListener("change", async (e) => {
  e.preventDefault();
  autoReloadScreen();
  sessionStorage.setItem("autoReload", eConfigAutoReload.value);
});

btnForceReload.addEventListener("click", async (e) => {
  e.preventDefault();
  drawCanvasLoading();
  await reloadScreen();
});

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
    Dialog.loading.show('Fetching content...');
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

    Dialog.loading.hide();
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

    Dialog.loading.show('Deleting...');
    await requestGet("/file", {
      fs: currentDrive,
      action: 'delete',
      name: file
    });
    Dialog.loading.hide();
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
  if (e.target.matches(".act-dialog-close")) {
    e.preventDefault();
    Dialog.hide();
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
    Dialog.loading.show('Renaming...');
    await requestPost("/rename", {
      fs: currentDrive,
      filePath: path,
      fileName: fileName
    });
  } else if (actionType === "createFolder") {
    Dialog.loading.show('Creating Folder...');
    let urlQuery = new URLSearchParams({
      fs: currentDrive,
      action: "create",
      name: path.trimEnd("/") + "/" + fileName,
    });
    await requestGet("/file?" + urlQuery.toString());
  } else if (actionType === "createFile") {
    Dialog.loading.show('Creating File...');
    let urlQuery = new URLSearchParams({
      fs: currentDrive,
      action: "createfile",
      name: path.trimEnd("/") + "/" + fileName,
    });
    await requestGet("/file?" + urlQuery.toString());
  } else if (actionType === "serial") {
    Dialog.loading.show('Running Serial Command...');
    await runCommand(fileName);
    refreshList = false; // No need to refresh file list for serial commands
  }

  if (refreshList) fetchFiles(currentDrive, currentPath);
  Dialog.hide();
});

$(".act-save-credential").addEventListener("click", async (e) => {
  let username = $("#cred-username").value.trim();
  let password = $("#cred-password").value.trim();
  if (!username || !password) {
    alert("Username and password cannot be empty.");
    return;
  }

  Dialog.loading.show('Saving WiFi Credentials...');
  await requestGet("/wifi", {
    usr: username,
    pwd: password
  });
  Dialog.loading.hide();
  alert("Credentials saved successfully!");
});

$(".act-save-edit-file").addEventListener("click", async (e) => {
  await saveEditorFile();
});

const runEditorBtn = $(".act-run-edit-file");
runEditorBtn.addEventListener("click", async (e) => {
  await saveEditorFile(true);
  runEditorBtn.blur(); // remove focus
});

$(".act-reboot").addEventListener("click", async (e) => {
  e.preventDefault();
  if (!confirm("Are you sure you want to REBOOT the device?")) return;
  Dialog.loading.show('Rebooting...');
  await requestGet("/reboot");
  setTimeout(() => {
    location.reload();
  }, 1000);
});

$(".navigator-canvas").addEventListener("click", async (e) => {
  let nav = e.target.matches(".nav") ? e.target : e.target.closest(".nav");
  if (nav === null) return;

  let direction = nav.getAttribute("data-direction");
  if (direction === "Menu") {
    direction = "Sel 500";
  }

  await runNavigation(direction.toLowerCase());
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

  if ($(".dialog.navigator:not(.hidden)")) {
    const map_navigator = {
      "arrowup": "Up",
      "arrowdown": "Down",
      "arrowleft": "Prev",
      "arrowright": "Next",
      "enter": "Sel",
      "backspace": "Esc",
      "m": "Menu",
      "pageup": "NextPage",
      "pagedown": "PrevPage",
    };

    if (key === 'r') {
      e.preventDefault();
      e.stopImmediatePropagation();
      reloadScreen();
      return;
    }

    if (key in map_navigator) {
      e.preventDefault();
      e.stopImmediatePropagation();
      $(`.navigator-canvas .nav[data-direction="${map_navigator[key]}"]`).click();
      return;
    }
  }

  if (key === "escape" && $(".dialog-background:not(.hidden)")) {
    if ($(".dialog.editor:not(.hidden)")) {
      let editor = $(".dialog.editor .file-content");
      if (isModified(editor)) {
        if (!confirm("You have unsaved changes. Do you want to discard them?")) {
          return;
        }
      }
    }

    let btnEscape = $(".dialog:not(.hidden) .act-escape");
    if (btnEscape) btnEscape.click();
    return;
  }
});

$(".file-content").addEventListener("keyup", function (e) {

  if ($(".dialog.editor:not(.hidden)")) {
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
  }
});

(async function () {
  await fetchSystemInfo();
  await fetchFiles("LittleFS", "/");
})();

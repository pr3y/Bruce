// SPDX-License-Identifier: GPL-3.0-or-later
// Blind Spot: counter-surveillance and GPS wardriving. See THIRD_PARTY.md.
var BS_OUI = [
  ["00:25:DF", "Axon", "AXON", "HIGH"],
  ["E4:05:40", "Axon-Body", "AXON", "LOW"],
  ["28:24:FF", "Axon-Signal", "AXON", "LOW"],
  ["00:1D:96", "WatchGuard", "AXON", "MED"],
  ["00:23:BD", "DigitalAlly", "AXON", "MED"],
  ["D4:11:D6", "ShotSpotter", "RAVEN", "HIGH"],
  ["B4:1E:52", "Flock-MA-L", "FLOCK", "HIGH"],
  ["24:B2:B9", "Flock-Liteo", "FLOCK", "LOW"],
  ["D0:39:57", "Flock", "FLOCK", "LOW"],
  ["00:F4:8D", "Flock", "FLOCK", "LOW"],
  ["14:5A:FC", "Flock", "FLOCK", "LOW"],
  ["80:30:49", "Flock", "FLOCK", "LOW"],
  ["E0:0A:F6", "Flock", "FLOCK", "LOW"],
  ["70:C9:4E", "Flock", "FLOCK", "LOW"],
  ["3C:91:80", "Flock", "FLOCK", "LOW"],
  ["D8:F3:BC", "Flock", "FLOCK", "LOW"],
  ["B8:35:32", "Flock", "FLOCK", "LOW"],
  ["82:6B:F2", "Flock-DeFlk", "FLOCK", "LOW"],
  ["74:4C:A1", "Flock-Liteo", "FLOCK", "LOW"],
  ["9C:2F:9D", "Flock-Liteo", "FLOCK", "LOW"],
  ["C0:35:32", "Flock-Liteo", "FLOCK", "LOW"],
  ["94:08:53", "Flock-Liteo", "FLOCK", "LOW"],
  ["E4:AA:EA", "Flock-Liteo", "FLOCK", "LOW"],
  ["F4:6A:DD", "Flock-Liteo", "FLOCK", "LOW"],
  ["E8:D0:FC", "Flock-Liteo", "FLOCK", "LOW"],
  ["B8:1E:A4", "Flock-Liteo", "FLOCK", "LOW"],
  ["70:08:94", "Flock-Liteo", "FLOCK", "LOW"],
  ["58:00:E3", "Flock-Liteo", "FLOCK", "LOW"],
  ["5C:93:A2", "Flock-Liteo", "FLOCK", "LOW"],
  ["64:6E:69", "Flock-Liteo", "FLOCK", "LOW"],
  ["14:B5:CD", "Flock-Liteo", "FLOCK", "LOW"],
  ["08:3A:88", "Flock-UGSI", "FLOCK", "LOW"],
  ["E0:4F:43", "Flock-UGSI", "FLOCK", "LOW"],
  ["58:8E:81", "Flock-SiLab", "FLOCK", "LOW"],
  ["EC:1B:BD", "Flock-SiLab", "FLOCK", "LOW"],
  ["90:35:EA", "Flock-SiLab", "FLOCK", "LOW"],
  ["00:03:7F", "Flock-QCA", "FLOCK", "LOW"],
  ["00:A0:D8", "Flock-OEM", "FLOCK", "LOW"],
  ["00:04:7D", "ALPR-Mtrla", "ALPR", "HIGH"],
  ["00:18:85", "ALPR-Mtrla", "ALPR", "HIGH"],
  ["00:1F:92", "ALPR-Mtrla", "ALPR", "HIGH"],
  ["4C:CC:34", "ALPR-Mtrla", "ALPR", "HIGH"],
  ["B8:E2:8C", "ALPR-Mtrla", "ALPR", "HIGH"],
  ["00:BF:15", "ALPR-Gentec", "ALPR", "HIGH"],
  ["0C:BF:15", "ALPR-Gentec", "ALPR", "HIGH"],
  ["20:13:00", "Skim-Linvor", "SKIMMER", "LOW"],
  ["98:D3:31", "Skim-HC05", "SKIMMER", "LOW"],
  ["00:1A:7D", "Skim-CSR", "SKIMMER", "LOW"],
  ["2C:AA:8E", "Wyze", "CAMERA", "HIGH"],
  ["D0:3F:27", "Wyze", "CAMERA", "HIGH"],
  ["7C:78:B2", "Wyze", "CAMERA", "HIGH"],
  ["B8:D7:AF", "Wyze-Mod", "CAMERA", "LOW"],
  ["34:D2:70", "Amazon", "CAMERA", "MED"],
  ["F0:27:2D", "Amazon", "CAMERA", "MED"],
  ["C0:56:E3", "Hikvision", "CAMERA", "HIGH"],
  ["44:19:B6", "Hikvision", "CAMERA", "HIGH"],
  ["28:57:BE", "Hikvision", "CAMERA", "HIGH"],
  ["00:E0:4C", "Realtek", "CAMERA", "LOW"],
  ["BC:DD:C2", "Arlo", "CAMERA", "LOW"],
  ["4C:69:05", "Blink", "CAMERA", "LOW"],
  ["A4:C1:38", "Tuya", "CAMERA", "LOW"],
  ["3C:A0:70", "Blink", "CAMERA", "MED"],
  ["70:AD:43", "Blink", "CAMERA", "MED"],
  ["74:13:48", "Blink", "CAMERA", "MED"],
  ["74:AB:93", "Blink", "CAMERA", "MED"],
  ["C8:19:D8", "Blink", "CAMERA", "MED"],
  ["F0:74:C1", "Blink", "CAMERA", "MED"],
  ["D4:2D:C5", "i-PRO", "CAMERA", "HIGH"],
  ["FC:65:DE", "Ring", "RING", "MED"],
  ["68:37:E9", "Ring", "RING", "MED"],
  ["AC:9F:C3", "Ring", "RING", "HIGH"],
  ["18:7F:88", "Ring", "RING", "HIGH"],
  ["34:3E:A4", "Ring", "RING", "HIGH"],
  ["54:E0:19", "Ring", "RING", "HIGH"],
  ["5C:47:5E", "Ring", "RING", "HIGH"],
  ["64:9A:63", "Ring", "RING", "HIGH"],
  ["90:48:6C", "Ring", "RING", "HIGH"],
  ["9C:76:13", "Ring", "RING", "HIGH"],
  ["CC:3B:FB", "Ring", "RING", "HIGH"],
  ["C4:DB:AD", "Ring", "RING", "HIGH"],
  ["24:2B:D6", "Ring", "RING", "HIGH"],
  ["00:B4:63", "Ring", "RING", "HIGH"],
  ["50:E4:67", "Ring", "RING", "HIGH"],
  ["E0:A7:00", "Verkada", "CAMERA", "HIGH"],
  ["70:1A:D5", "Avigilon", "CAMERA", "HIGH"],
  ["00:40:8C", "Axis", "CAMERA", "HIGH"],
  ["B8:A4:4F", "Axis", "CAMERA", "HIGH"],
  ["98:59:49", "Luxottica", "META", "MED"],
  ["60:60:1F", "DJI", "DRONE", "MED"],
  ["34:D2:62", "DJI", "DRONE", "MED"],
  ["48:1C:B9", "DJI", "DRONE", "MED"],
  ["E4:7A:2C", "DJI", "DRONE", "MED"],
  ["58:B8:58", "DJI", "DRONE", "MED"],
  ["04:A8:5A", "DJI", "DRONE", "MED"],
  ["8C:58:23", "DJI", "DRONE", "MED"],
  ["0C:9A:E6", "DJI", "DRONE", "MED"],
  ["88:29:85", "DJI", "DRONE", "MED"],
  ["4C:43:F6", "DJI", "DRONE", "MED"],
  ["9C:5A:8A", "DJI-BW", "DRONE", "LOW"],
  ["EC:72:F7", "DJI-BW", "DRONE", "LOW"],
  ["34:91:F0", "DJI-BW", "DRONE", "LOW"],
  ["C8:A1:62", "DJI-BW", "DRONE", "LOW"],
  ["38:1D:14", "Skydio", "DRONE", "HIGH"],
  ["90:03:B7", "Parrot", "DRONE", "MED"],
  ["90:3A:E6", "Parrot", "DRONE", "MED"],
  ["A0:14:3D", "Parrot", "DRONE", "MED"],
  ["00:12:1C", "Parrot", "DRONE", "LOW"],
  ["00:26:7E", "Parrot", "DRONE", "LOW"],
  ["02:C0:CA", "Hak5-LA", "HACKER", "LOW"],
  ["02:13:37", "Hak5-LA", "HACKER", "LOW"]
];
var BS_SSID = [
  ["AB2-", "Axon-Body2", "AXON"],
  ["AB3-", "Axon-Body3", "AXON"],
  ["AB4-", "Axon-Body4", "AXON"],
  ["AXON-", "Axon-Field", "AXON"],
  ["flock-", "Flock-Setup", "FLOCK"],
  ["Flock Camera", "Flock-Cam", "FLOCK"],
  ["Pineapple_", "Pineapple", "HACKER"],
  ["pwned", "Deauther", "HACKER"]
];
var BS_UUID = [
  [4353, "Skim-SPP", "SKIMMER"],
  [65261, "Tile", "TILE"],
  [65260, "Tile", "TILE"],
  [64900, "Tile", "TILE"],
  [64641, "Axon", "AXON"],
  [65131, "Axon", "AXON"],
  [65132, "Axon", "AXON"],
  [65207, "Meta", "META"],
  [65208, "Meta", "META"],
  [65093, "Snap", "META"],
  [64690, "Tracker", "GOOGLE_TAG"],
  [65075, "Chipolo", "GOOGLE_TAG"],
  [65125, "Chipolo", "GOOGLE_TAG"],
  [64863, "RayBanMeta", "META"],
  [12544, "Raven", "RAVEN"],
  [12800, "Raven", "RAVEN"],
  [13056, "Raven", "RAVEN"],
  [13312, "Raven", "RAVEN"],
  [13568, "Raven", "RAVEN"],
  [65530, "DroneID", "DRONE"],
  [64858, "SmartTag", "SAMSUNG_TAG"],
  [65194, "FindHub", "GOOGLE_TAG"],
  [12417, "Flipper", "HACKER"],
  [12418, "Flipper", "HACKER"],
  [12419, "Flipper", "HACKER"]
];
var BS_MFG = [
  [76, "Apple", "AIRTAG"],
  [2504, "XUNTONG", "FLOCK"],
  [427, "Meta", "META"],
  [3411, "Luxottica", "META"],
  [962, "Snap", "META"],
  [1548, "Vuzix", "META"],
  [845, "Axon", "AXON"],
  [2243, "Chipolo", "GOOGLE_TAG"]
];

/* Blind Spot 0.1.0 - GPL-3.0-or-later.
 * Signature data adapted from SquachWatch-CYD, see THIRD_PARTY.md.
 * Pure core: no radio, storage or UI dependencies. */
var BlindSpot = (function () {
  var labels = {
    FLOCK: "Flock-related",
    AXON: "Body cameras",
    META: "Camera glasses",
    SKIMMER: "Serial BT modules",
    DRONE: "Drones",
    ALPR: "ALPR vendors",
    CAMERA: "Cameras",
    RING: "Ring",
    HACKER: "Pentest devices",
    REVIEW: "Network review"
  };
  function finite(n) {
    return typeof n === "number" && isFinite(n);
  }
  function clean(s, n) {
    return String(s || "")
      .replace(/[\x00-\x1f\x7f]/g, " ")
      .slice(0, n || 96);
  }
  function mac(s, ble) {
    s = String(s || "").toUpperCase();
    return /^([0-9A-F]{2}:){5}[0-9A-F]{2}$/.test(s) &&
      (ble || !(parseInt(s.slice(0, 2), 16) & 1)) &&
      s !== "00:00:00:00:00:00"
      ? s
      : "";
  }
  function match(type, label, evidence, grade) {
    return {
      type: type,
      label: label,
      evidence: evidence,
      grade: grade,
      certainty: "possible"
    };
  }
  function identify(o) {
    var n = o.name.toLowerCase(),
      i,
      r,
      hits = [];
    if (o.kind === "WIFI") {
      if (n === "flock")
        hits.push(match("FLOCK", "Flock", "Exact SSID: Flock", "MED"));
      for (i = 0; i < BS_SSID.length; i++) {
        r = BS_SSID[i];
        if (n.indexOf(r[0].toLowerCase()) === 0)
          hits.push(match(r[2], r[1], "SSID prefix: " + r[0], "MED"));
      }
      for (i = 0; i < BS_OUI.length; i++) {
        r = BS_OUI[i];
        if (
          !(parseInt(o.mac.slice(0, 2), 16) & 2) &&
          o.mac.slice(0, 8) === r[0]
        )
          hits.push(
            match(r[2], r[1], "OUI " + r[0] + "; upstream " + r[3], r[3])
          );
      }
    } else {
      if (/^(hc-03|hc-05|hc-06|rn42|bt04-a)$/.test(n))
        hits.push(
          match(
            "SKIMMER",
            "Serial BT module",
            "Default module name; not proof of skimming",
            "LOW"
          )
        );
      if (
        n.indexOf("flock") >= 0 ||
        n.indexOf("penguin") >= 0 ||
        n.indexOf("pigvision") >= 0 ||
        n === "fs ext battery" ||
        /^FS-[a-fA-F0-9]{4,12}$/.test(o.name)
      )
        hits.push(
          match("FLOCK", "Flock-related", "BLE name pattern; spoofable", "MED")
        );
      if (/^\d{10}$/.test(n))
        hits.push(
          match(
            "FLOCK",
            "Serial-only device",
            "10-digit BLE name; very weak evidence",
            "LOW"
          )
        );
      if (n.indexOf("axon") >= 0 || n.indexOf("bodyworn") >= 0)
        hits.push(
          match(
            "AXON",
            n.indexOf("bodyworn") >= 0 ? "BodyWorn" : "Axon",
            "BLE name pattern; spoofable",
            "MED"
          )
        );
      if (n.indexOf("flipper") >= 0)
        hits.push(
          match("HACKER", "Flipper-like", "BLE name pattern; spoofable", "MED")
        );
      // Never attribute BLE OUIs: this binding does not expose address type.
    }
    return hits;
  }
  function normalize(x, kind) {
    if (!x || typeof x !== "object") return null;
    var m = mac(kind === "WIFI" ? x.MAC : x.address, kind === "BLE"),
      rss = Number(kind === "WIFI" ? x.RSSI : x.rssi),
      ch = kind === "WIFI" ? Number(x.channel) : 0;
    if (
      !m ||
      !finite(rss) ||
      rss > 0 ||
      rss < -127 ||
      (kind === "WIFI" &&
        (!finite(ch) || ch < 1 || ch > 14 || Math.floor(ch) !== ch))
    )
      return null;
    var o = {
      kind: kind,
      mac: m,
      name: clean(kind === "WIFI" ? x.SSID : x.name, 96),
      rssi: rss,
      channel: ch,
      auth: kind === "WIFI" ? clean(x.encryptionType, 32) : "",
      frequency: kind === "WIFI" ? (ch === 14 ? 2484 : 2407 + 5 * ch) : 0
    };
    o.matches = identify(o);
    return o;
  }
  function q(s) {
    return (
      '"' +
      String(s === null || s === undefined ? "" : s).replace(/"/g, '""') +
      '"'
    );
  }
  var header =
    "WigleWifi-1.6,appRelease=BlindSpot-0.1.0,model=T-Embed,release=Bruce,device=ESP32-S3,display=320x170,board=T-Embed,brand=LILYGO\r\nMAC,SSID,AuthMode,FirstSeen,Channel,Frequency,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,RCOIs,MfgrId,Type\r\n";
  function validFix(g) {
    return (
      g &&
      finite(g.lat) &&
      finite(g.lon) &&
      Math.abs(g.lat) <= 90 &&
      Math.abs(g.lon) <= 180 &&
      /^20\d\d-\d\d-\d\dT\d\d:\d\d:\d\dZ$/.test(g.ts)
    );
  }
  function csv(o) {
    if (!validFix(o.gps)) throw Error("No valid GPS position/time");
    var g = o.gps;
    return (
      [
        o.mac,
        o.name,
        o.kind === "WIFI"
          ? o.auth === "OPEN"
            ? "[ESS]"
            : "[" + o.auth + "][ESS]"
          : "",
        g.ts.replace("T", " ").replace("Z", ""),
        o.channel,
        o.frequency,
        o.rssi,
        g.lat,
        g.lon,
        finite(g.alt) ? g.alt : "",
        finite(g.accuracy) ? g.accuracy : "",
        "",
        "",
        o.kind
      ]
        .map(q)
        .join(",") + "\r\n"
    );
  }
  function asciiJSON(x) {
    return JSON.stringify(x).replace(/[\u007f-\uffff]/g, function (c) {
      return "\\u" + ("0000" + c.charCodeAt(0).toString(16)).slice(-4);
    });
  }
  function multipart(csvText, filename, boundary, donate) {
    if (
      !/^[a-zA-Z0-9_.-]+$/.test(filename) ||
      !/^[a-zA-Z0-9-]+$/.test(boundary) ||
      csvText.indexOf(boundary) >= 0
    )
      throw Error("Invalid multipart boundary/name");
    var b =
      "--" +
      boundary +
      '\r\nContent-Disposition: form-data; name="file"; filename="' +
      filename +
      '"\r\nContent-Type: text/csv; charset=utf-8\r\n\r\n' +
      csvText +
      "\r\n";
    if (donate !== null)
      b +=
        "--" +
        boundary +
        '\r\nContent-Disposition: form-data; name="donate"\r\n\r\n' +
        (donate ? "on" : "false") +
        "\r\n";
    return b + "--" + boundary + "--\r\n";
  }
  function response(service, r) {
    var b = r.body;
    if (typeof b === "string") {
      try {
        b = JSON.parse(b);
      } catch (e) {
        return {
          ok: false,
          status: "Unknown response; check account before retry"
        };
      }
    }
    if (!b || typeof b !== "object")
      return { ok: false, status: "Missing API acknowledgement" };
    var status = Number(r.status || r.response);
    if (status < 200 || status >= 300)
      return {
        ok: false,
        status:
          "HTTP " +
          status +
          (status === 401 || status === 403
            ? " - check API key"
            : status === 429
              ? " - retry later"
              : ""),
        unknown: false
      };
    var ok =
      service === "wigle"
        ? b.success === true
        : service === "wdgwars"
          ? b.ok === true || b.success === true
          : b.accepted === true || b.duplicate === true;
    return {
      ok: ok,
      status: ok ? "Accepted" : "Not acknowledged; check account",
      receipt: clean(b.transid || b.id || "", 80)
    };
  }
  function pad(n) {
    return n < 10 ? "0" + n : String(n);
  }
  function coord(s, hem, lat) {
    if (!(lat ? /^\d{4}\.\d+$/ : /^\d{5}\.\d+$/).test(s)) return null;
    var deg = Number(s.slice(0, lat ? 2 : 3)),
      min = Number(s.slice(lat ? 2 : 3));
    if (
      min >= 60 ||
      deg > (lat ? 90 : 180) ||
      (!/^[NS]$/.test(hem) && lat) ||
      (!/^[EW]$/.test(hem) && !lat)
    )
      return null;
    return (deg + min / 60) * (hem === "S" || hem === "W" ? -1 : 1);
  }
  function NMEA() {
    this.buffer = "";
    this.fix = null;
    this.lastSentence = -1;
    this.byteCount = 0;
    this.good = 0;
    this.bad = 0;
    this.lastByte = -1;
    this.lastLine = "";
    this.alt = null;
    this.altTime = "";
  }
  NMEA.prototype.line = function (s, t) {
    if (s.charAt(0) !== "$") return false;
    var star = s.indexOf("*"),
      i,
      sum = 0;
    if (star < 0 || !/^[0-9A-Fa-f]{2}$/.test(s.slice(star + 1))) return false;
    for (i = 1; i < star; i++) sum ^= s.charCodeAt(i);
    if (sum !== parseInt(s.slice(star + 1), 16)) {
      this.bad++;
      return false;
    }
    if (!/^\$[A-Z]{2}[A-Z]{3},/.test(s)) return false;
    this.lastSentence = t;
    this.good++;
    this.lastLine = s;
    var f = s.slice(1, star).split(","),
      type = f[0].slice(-3);
    if (type !== "RMC" && type !== "GGA") return false;
    this.lastSentence = t;
    if (type === "GGA") {
      if (Number(f[6]) === 0) this.fix = null;
      this.alt =
        Number(f[6]) > 0 && f[10] === "M" && f[9] !== "" ? Number(f[9]) : null;
      this.altTime = (f[1] || "").slice(0, 6);
      return true;
    }
    if (
      f[2] !== "A" ||
      !/^\d{6}(\.\d+)?$/.test(f[1]) ||
      !/^\d{6}$/.test(f[9]) ||
      f[12] === "N"
    ) {
      this.fix = null;
      return true;
    }
    var la = coord(f[3], f[4], true),
      lo = coord(f[5], f[6], false),
      d = f[9],
      tm = f[1],
      y = 2000 + Number(d.slice(4)),
      mo = Number(d.slice(2, 4)),
      day = Number(d.slice(0, 2)),
      h = Number(tm.slice(0, 2)),
      mi = Number(tm.slice(2, 4)),
      se = Number(tm.slice(4, 6));
    var days = [
      31,
      y % 4 === 0 && (y % 100 !== 0 || y % 400 === 0) ? 29 : 28,
      31,
      30,
      31,
      30,
      31,
      31,
      30,
      31,
      30,
      31
    ];
    if (
      la === null ||
      lo === null ||
      Math.abs(la) > 90 ||
      Math.abs(lo) > 180 ||
      mo < 1 ||
      mo > 12 ||
      day < 1 ||
      day > days[mo - 1] ||
      h > 23 ||
      mi > 59 ||
      se > 59
    ) {
      this.fix = null;
      return true;
    }
    this.fix = {
      lat: la,
      lon: lo,
      ts:
        y +
        "-" +
        pad(mo) +
        "-" +
        pad(day) +
        "T" +
        pad(h) +
        ":" +
        pad(mi) +
        ":" +
        pad(se) +
        "Z",
      at: t,
      alt: this.altTime === tm.slice(0, 6) ? this.alt : null,
      accuracy: null
    };
    return true;
  };
  NMEA.prototype.feed = function (bytes, t) {
    this.byteCount += bytes.length;
    if (bytes.length) this.lastByte = t;
    for (var i = 0; i < bytes.length; i++) {
      var c =
        typeof bytes === "string"
          ? bytes.charAt(i)
          : String.fromCharCode(bytes[i]);
      if (c === "$") this.buffer = "$";
      else if (c === "\n") {
        this.line(this.buffer.replace(/\r/g, ""), t);
        this.buffer = "";
      } else if (this.buffer.length < 160) this.buffer += c;
      else this.buffer = "";
    }
  };
  NMEA.prototype.current = function (t) {
    if (!this.fix || t - this.fix.at > 2000 || t < this.fix.at) return null;
    return this.fix;
  };
  function wardriftItems(obs) {
    return obs.map(function (o) {
      if (!validFix(o.gps)) throw Error("Missing GPS");
      return {
        ts: o.gps.ts,
        source_ts: o.gps.ts,
        lat: o.gps.lat,
        lon: o.gps.lon,
        kind: o.kind === "WIFI" ? "wifi" : "bluetooth",
        bssid: o.mac.toLowerCase(),
        ssid: o.name,
        rssi: o.rssi
      };
    });
  }
  return {
    labels: labels,
    clean: clean,
    mac: mac,
    identify: identify,
    normalize: normalize,
    csv: csv,
    header: header,
    validFix: validFix,
    NMEA: NMEA,
    multipart: multipart,
    response: response,
    asciiJSON: asciiJSON,
    wardriftItems: wardriftItems
  };
})();

/* Payload detection adapted from SquachWatch, pinned in THIRD_PARTY.md. */
(function (B) {
  B.labels.AIRTAG = "Find My accessories";
  B.labels.APPLE = "Apple Bluetooth";
  B.labels.TILE = "Tile";
  B.labels.SAMSUNG_TAG = "SmartTag";
  B.labels.GOOGLE_TAG = "Find Hub / trackers";
  B.labels.RAVEN = "Raven";
  B.labels.IBEACON = "iBeacons";
  function bytes(h) {
    if (typeof h !== "string" || h.length % 2 || !/^[0-9a-f]*$/i.test(h))
      return [];
    var a = [];
    for (var i = 0; i < h.length; i += 2)
      a.push(parseInt(h.slice(i, i + 2), 16));
    return a;
  }
  function hex(a) {
    return a
      .map(function (v) {
        return ("0" + v.toString(16)).slice(-2);
      })
      .join("");
  }
  function txt(a) {
    return a
      .map(function (v) {
        return String.fromCharCode(v);
      })
      .join("");
  }
  function utf8(a) {
    var out = "",
      i = 0;
    while (i < a.length) {
      var b = a[i++],
        cp = b,
        n = 0,
        min = 0;
      if (b >= 0xc2 && b <= 0xdf) {
        n = 1;
        cp = b & 31;
        min = 128;
      } else if (b >= 0xe0 && b <= 0xef) {
        n = 2;
        cp = b & 15;
        min = 2048;
      } else if (b >= 0xf0 && b <= 0xf4) {
        n = 3;
        cp = b & 7;
        min = 65536;
      } else if (b >= 128) {
        out += "?";
        continue;
      }
      var ok = i + n <= a.length;
      for (var j = 0; j < n && ok; j++) {
        if ((a[i] & 192) !== 128) {
          ok = false;
          break;
        }
        cp = cp * 64 + (a[i++] & 63);
      }
      if (!ok || cp < min || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) {
        out += "?";
        continue;
      }
      if (cp > 65535) {
        cp -= 65536;
        out += String.fromCharCode(0xd800 + (cp >> 10), 0xdc00 + (cp & 1023));
      } else out += String.fromCharCode(cp);
    }
    return out;
  }
  function rsn(d) {
    if (d.length < 10 || u16(d, 0) !== 1) return "WPA2";
    var p = 8 + 4 * u16(d, 6);
    if (p + 2 > d.length) return "WPA2";
    var count = u16(d, p);
    p += 2;
    var psk = false,
      sae = false,
      enterprise = false;
    for (var i = 0; i < count && p + 4 <= d.length; i++, p += 4) {
      if (hex(d.slice(p, p + 3)) !== "000fac") continue;
      var akm = d[p + 3];
      if (akm === 2 || akm === 4 || akm === 6) psk = true;
      if (akm === 8 || akm === 9) sae = true;
      if (akm === 1 || akm === 3 || akm === 5 || akm === 11 || akm === 12)
        enterprise = true;
    }
    return sae
      ? psk
        ? "WPA2-PSK+SAE"
        : "WPA3-SAE"
      : enterprise
        ? "WPA2-EAP"
        : psk
          ? "WPA2-PSK"
          : "WPA2";
  }
  function u16(a, i) {
    return a[i] | (a[i + 1] << 8);
  }
  function i32(a, i) {
    return a[i] | (a[i + 1] << 8) | (a[i + 2] << 16) | (a[i + 3] << 24);
  }
  function hit(o, type, label, ev, grade) {
    o.matches.push({
      type: type,
      label: label,
      evidence: ev,
      grade: grade,
      certainty: "signature"
    });
  }
  function rid(m, o) {
    if (m.length !== 25) return;
    if (!o.remote) o.remote = {};
    var r = o.remote,
      t = m[0] >> 4;
    if (t === 0) {
      r.idType = m[1] >> 4;
      r.airframe = m[1] & 15;
      r.serial = B.clean(txt(m.slice(2, 22)).split("\x00")[0], 20);
    }
    if (t === 1 || t === 4) {
      var la = i32(m, t === 1 ? 5 : 2) / 1e7,
        lo = i32(m, t === 1 ? 9 : 6) / 1e7;
      if ((la || lo) && Math.abs(la) <= 90 && Math.abs(lo) <= 180) {
        if (t === 1) {
          r.lat = la;
          r.lon = lo;
          var alt = u16(m, 15);
          if (alt !== 0) r.alt = alt / 2 - 1000;
        } else {
          r.operatorLat = la;
          r.operatorLon = lo;
        }
      }
    }
  }
  B.bleRaw = function (x) {
    var o = B.normalize(x, "BLE");
    if (!o) return null;
    o.addressType = x.addressType;
    o.companyIDs = [];
    o.serviceUUIDs = [];
    o.raw = x.payload || "";
    o.manufacturerHex = x.manufacturer || "";
    o.payloadBytes = o.raw.length / 2;
    var a = bytes(o.raw),
      i,
      j,
      d,
      t,
      l,
      u,
      r,
      metaCID = false,
      metaUUID = false;
    if (!a.length && o.manufacturerHex) {
      var mf = bytes(o.manufacturerHex);
      if (mf.length >= 2 && mf.length <= 254)
        a = [mf.length + 1, 255].concat(mf);
    }
    for (i = 0; i + 1 < a.length; ) {
      l = a[i];
      if (l === 0) break;
      if (i + 1 + l > a.length) break;
      t = a[i + 1];
      d = a.slice(i + 2, i + 1 + l);
      i += 1 + l;
      if (t === 0xff && d.length >= 2) {
        o.manufacturerHex = hex(d);
        u = u16(d, 0);
        if (o.companyIDs.indexOf(u) < 0) o.companyIDs.push(u);
        if (u === 0x0d53) metaCID = true;
        if (u === 0x004c) {
          if (d.length === 25 && d[2] === 2 && d[3] === 0x15) {
            hit(o, "IBEACON", "iBeacon", "Apple iBeacon header", "HIGH");
            o.beacon = {
              uuid: hex(d.slice(4, 20)),
              major: d[20] * 256 + d[21],
              minor: d[22] * 256 + d[23]
            };
          } else {
            var ap = 2,
              findMy = false,
              validApple = false;
            while (ap + 2 <= d.length) {
              var at = d[ap],
                al = d[ap + 1];
              if (ap + 2 + al > d.length) break;
              validApple = true;
              if (at === 0x12 && (al === 2 || al === 25)) {
                var st = d[ap + 2],
                  family = [
                    "Apple Find My",
                    "AirTag-like",
                    "Find My accessory",
                    "AirPods Find My"
                  ][(st >> 4) & 3],
                  state = al === 2 ? "nearby" : "separated";
                hit(
                  o,
                  "AIRTAG",
                  family + " / " + state,
                  "Apple 12/" +
                    al.toString(16) +
                    " " +
                    state +
                    " form; AirTag/AirPods family from status, not identity proof",
                  "HIGH"
                );
                o.findMy = {
                  state: state,
                  family: family,
                  status: st,
                  battery: ["full", "medium", "low", "very low"][(st >> 6) & 3]
                };
                findMy = true;
              }
              ap += 2 + al;
            }
            if (!findMy && validApple)
              hit(
                o,
                "APPLE",
                "Apple Bluetooth device",
                "Apple company 004C; no Find My signature in this packet",
                "MED"
              );
          }
        } else
          for (j = 0; j < BS_MFG.length; j++) {
            r = BS_MFG[j];
            if (r[0] === u)
              hit(
                o,
                r[2],
                r[1],
                "BLE manufacturer " + u.toString(16),
                u === 0x09c8 || r[2] === "META" ? "LOW" : "MED"
              );
          }
      }
      if ((t === 2 || t === 3 || t === 0x16) && d.length >= 2) {
        var us = t === 0x16 ? [u16(d, 0)] : [];
        if (t !== 0x16)
          for (j = 0; j + 1 < d.length; j += 2) us.push(u16(d, j));
        for (var k = 0; k < us.length; k++) {
          u = us[k];
          if (o.serviceUUIDs.indexOf(u) < 0) o.serviceUUIDs.push(u);
          if (u === 0xfd5f) metaUUID = true;
          if (u === 0xfffa) {
            if (t === 0x16 && d.length >= 29 && d[2] === 0x0d) {
              hit(o, "DRONE", "Remote ID", "ASTM F3411 BLE message", "HIGH");
              rid(d.slice(4, 29), o);
            }
            continue;
          }
          if (u === 0xfeaa) {
            if (t === 0x16 && d.length >= 3 && (d[2] === 0x40 || d[2] === 0x41))
              hit(o, "GOOGLE_TAG", "Find Hub", "FMDN service frame", "HIGH");
            continue;
          }
          for (j = 0; j < BS_UUID.length; j++) {
            r = BS_UUID[j];
            if (r[0] === u)
              hit(
                o,
                r[2],
                r[1],
                "BLE service " + u.toString(16),
                r[2] === "SKIMMER" || r[2] === "META"
                  ? "LOW"
                  : r[2] === "RAVEN"
                    ? "MED"
                    : "HIGH"
              );
          }
        }
      }
      if (t === 6 || t === 7) {
        for (j = 0; j + 15 < d.length; j += 16)
          if (hex(d.slice(j, j + 16)) === "6f2e17df1418e59fa846329538bbcce8")
            hit(
              o,
              "FLOCK",
              "Flock accessory",
              "128-bit Flock service UUID",
              "HIGH"
            );
      }
    }
    if (metaCID && metaUUID)
      hit(
        o,
        "META",
        "Meta / Ray-Ban composite",
        "Luxottica CID 0D53 + Meta FD5F in same advert",
        "HIGH"
      );
    else if (/ray-ban|wayfarer|oakley meta/i.test(o.name))
      hit(
        o,
        "META",
        "Possible Meta glasses",
        "Advertised name match; spoofable",
        "MED"
      );
    if (B.oui) B.oui(o);
    var text = txt(a);
    if (text.indexOf("BWCDEVICE") >= 0 || text.indexOf("ECIVEDCWB") >= 0)
      hit(o, "AXON", "Axon", "BWCDEVICE payload marker", "HIGH");
    return o;
  };
  B.wifiRaw = function (x) {
    if (!x || typeof x.payload !== "string") return null;
    var a = bytes(x.payload),
      s = a[0] & 0xfc;
    if (a.length < 24 || (s !== 0x80 && s !== 0x50 && s !== 0x40)) return null;
    var ma = a
        .slice(10, 16)
        .map(function (v) {
          return ("0" + v.toString(16)).slice(-2);
        })
        .join(":"),
      name = "",
      auth = "OPEN",
      i = s === 0x40 ? 24 : 36,
      ies = [],
      d,
      l,
      id,
      ch = x.channel;
    if (a.length < i) return null;
    if (s !== 0x40 && a[34] & 16) auth = "WEP";
    while (i + 2 <= a.length) {
      id = a[i];
      l = a[i + 1];
      if (i + 2 + l > a.length) break;
      d = a.slice(i + 2, i + 2 + l);
      ies.push([id, d]);
      if (id === 0 && l <= 32) name = utf8(d);
      if (id === 3 && l === 1) ch = d[0];
      if (id === 48) auth = rsn(d);
      if (
        id === 221 &&
        d.length >= 4 &&
        hex(d.slice(0, 4)) === "0050f201" &&
        auth.indexOf("WPA2") !== 0 &&
        auth.indexOf("WPA3") !== 0
      )
        auth = "WPA";
      i += 2 + l;
    }
    var o = B.normalize(
      { MAC: ma, SSID: name, RSSI: x.rssi, channel: ch, encryptionType: auth },
      "WIFI"
    );
    if (!o) return null;
    o.probe = s === 0x40;
    if (o.probe) {
      o.name = "Probe device";
      o.auth = "";
      o.matches = B.identify(o);
    }
    for (i = 0; i < ies.length; i++) {
      d = ies[i][1];
      if (ies[i][0] !== 221) continue;
      if (
        d.length >= 8 &&
        hex(d.slice(0, 4)) === "fa0bbc0d" &&
        d[5] >> 4 === 15 &&
        d[6] === 25 &&
        d[7] > 0 &&
        d[7] <= 9 &&
        d.length >= 8 + 25 * d[7]
      ) {
        hit(o, "DRONE", "Remote ID", "ASTM F3411 Wi-Fi beacon", "HIGH");
        for (var j = 0; j < d[7]; j++) rid(d.slice(8 + 25 * j, 33 + 25 * j), o);
      }
      var st = txt(d);
      if (st.indexOf('"pwnd_tot"') >= 0 && st.indexOf('"name"') >= 0) {
        hit(
          o,
          "HACKER",
          "Pwnagotchi",
          "Beacon advertises pwnd_tot and name",
          "HIGH"
        );
      }
    }
    if (B.oui) B.oui(o);
    o.raw = x.payload;
    return o;
  };
  B.hexBytes = bytes;
})(BlindSpot);

/* OUI-SPY published vendor facts, pinned 296d4c34724fc8df986351036a89690414611589. */
var OUI_SPY = [
  ["18:7F:88", "RING", "RING", "MED", "BOTH"],
  ["24:2B:D6", "RING", "RING", "MED", "BOTH"],
  ["34:3E:A4", "RING", "RING", "MED", "BOTH"],
  ["54:E0:19", "RING", "RING", "MED", "BOTH"],
  ["5C:47:5E", "RING", "RING", "MED", "BOTH"],
  ["64:9A:63", "RING", "RING", "MED", "BOTH"],
  ["90:48:6C", "RING", "RING", "MED", "BOTH"],
  ["9C:76:13", "RING", "RING", "MED", "BOTH"],
  ["AC:9F:C3", "RING", "RING", "MED", "BOTH"],
  ["C4:DB:AD", "RING", "RING", "MED", "BOTH"],
  ["CC:3B:FB", "RING", "RING", "MED", "BOTH"],
  ["B4:1E:52", "FLOCK SAFETY", "FLOCK", "MED", "BOTH"],
  ["70:C9:4E", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["3C:91:80", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["D8:F3:BC", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["80:30:49", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["B8:35:32", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["14:5A:FC", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["74:4C:A1", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["08:3A:88", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["9C:2F:9D", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["C0:35:32", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["94:08:53", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["E4:AA:EA", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["F4:6A:DD", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["F8:A2:D6", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["24:B2:B9", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["00:F4:8D", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["D0:39:57", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["E8:D0:FC", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["E0:4F:43", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["B8:1E:A4", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["70:08:94", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["58:8E:81", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["EC:1B:BD", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["3C:71:BF", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["58:00:E3", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["90:35:EA", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["5C:93:A2", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["64:6E:69", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["48:27:EA", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["A4:CF:12", "FLOCK SAFETY", "FLOCK", "LOW", "WIFI"],
  ["0C:9A:E6", "DJI", "DRONE", "MED", "BOTH"],
  ["8C:58:23", "DJI", "DRONE", "MED", "BOTH"],
  ["04:A8:5A", "DJI", "DRONE", "MED", "BOTH"],
  ["58:B8:58", "DJI", "DRONE", "MED", "BOTH"],
  ["E4:7A:2C", "DJI", "DRONE", "MED", "BOTH"],
  ["60:60:1F", "DJI", "DRONE", "MED", "BOTH"],
  ["48:1C:B9", "DJI", "DRONE", "MED", "BOTH"],
  ["34:D2:62", "DJI", "DRONE", "MED", "BOTH"],
  ["00:12:1C", "PARROT", "DRONE", "MED", "BOTH"],
  ["00:26:7E", "PARROT", "DRONE", "MED", "BOTH"],
  ["90:03:B7", "PARROT", "DRONE", "MED", "BOTH"],
  ["90:3A:E6", "PARROT", "DRONE", "MED", "BOTH"],
  ["A0:14:3D", "PARROT", "DRONE", "MED", "BOTH"],
  ["38:1D:14", "SKYDIO", "DRONE", "MED", "BOTH"],
  ["7C:2A:9E", "META/RAYBAN SMARTGLASSES", "META", "MED", "BOTH"],
  ["CC:66:0A", "META/RAYBAN SMARTGLASSES", "META", "MED", "BOTH"],
  ["F4:03:43", "META/RAYBAN SMARTGLASSES", "META", "MED", "BOTH"],
  ["5C:E9:1E", "META/RAYBAN SMARTGLASSES", "META", "MED", "BOTH"],
  ["98:59:49", "META/RAYBAN SMARTGLASSES", "META", "MED", "BOTH"],
  ["00:25:DF", "AXON", "AXON", "MED", "BOTH"],
  ["D4:2D:C5", "I-PRO", "AXON", "MED", "BOTH"]
];
(function (B) {
  B.labels.COMOVING = "Seen along route";
  B.oui = function (o) {
    if (o.kind === "BLE" && o.addressType !== 0) return;
    if (parseInt(o.mac.slice(0, 2), 16) & 2) return;
    for (var i = 0; i < OUI_SPY.length; i++) {
      var r = OUI_SPY[i];
      if (r[0] !== o.mac.slice(0, 8) || (r[4] === "WIFI" && o.kind !== "WIFI"))
        continue;
      var duplicate = false;
      for (var j = 0; j < o.matches.length; j++)
        if (
          o.matches[j].type === r[2] &&
          o.matches[j].evidence.indexOf("OUI") >= 0
        )
          duplicate = true;
      if (!duplicate)
        o.matches.push({
          type: r[2],
          label: r[1] + " vendor hint",
          grade: r[3],
          certainty: "possible",
          evidence: "OUI-SPY " + r[0] + "; vendor association, not device proof"
        });
    }
  };
  function distance(a, b) {
    var rad = Math.PI / 180,
      dx = (a.lon - b.lon) * rad * Math.cos(((a.lat + b.lat) * rad) / 2),
      dy = (a.lat - b.lat) * rad;
    return 6371000 * Math.sqrt(dx * dx + dy * dy);
  }
  B.RouteWatch = function () {
    this.devices = [];
  };
  B.RouteWatch.prototype.observe = function (o, t, trusted) {
    if (
      o.kind !== "BLE" ||
      (trusted || []).indexOf(o.mac) >= 0 ||
      !B.validFix(o.gps)
    )
      return null;
    var a = this.devices,
      i,
      s = null;
    for (i = a.length - 1; i >= 0; i--) {
      if (t - a[i].last > 900000 || t < a[i].last) a.splice(i, 1);
      else if (a[i].mac === o.mac && a[i].addressType === o.addressType)
        s = a[i];
    }
    if (!s) {
      s = {
        mac: o.mac,
        addressType: o.addressType,
        first: t,
        last: t,
        points: [{ lat: o.gps.lat, lon: o.gps.lon }],
        hits: 0,
        lastPoint: t,
        alerted: -600000
      };
      a.push(s);
      if (a.length > 64) a.shift();
    }
    s.last = t;
    s.hits++;
    var distinct = true;
    for (i = 0; i < s.points.length; i++)
      if (distance(s.points[i], o.gps) < 100) distinct = false;
    if (distinct && t - s.lastPoint >= 30000 && s.points.length < 8) {
      s.points.push({ lat: o.gps.lat, lon: o.gps.lon });
      s.lastPoint = t;
    }
    var span = 0;
    for (i = 0; i < s.points.length; i++)
      span = Math.max(span, distance(s.points[0], s.points[i]));
    if (
      s.points.length >= 3 &&
      t - s.first >= 120000 &&
      s.hits >= 6 &&
      span >= 250
    ) {
      o.routeEvidence = {
        locations: s.points.length,
        seconds: Math.floor((t - s.first) / 1000),
        spanMeters: Math.round(span),
        observations: s.hits
      };
      o.matches.push({
        type: "COMOVING",
        label: "Seen along your route",
        grade: "MED",
        certainty: "possible",
        evidence:
          s.points.length +
          " locations / " +
          o.routeEvidence.seconds +
          "s / " +
          Math.round(span) +
          "m; same BLE address, not proof of following"
      });
      if (t - s.alerted >= 600000) {
        s.alerted = t;
        return o;
      }
    }
    return null;
  };
})(BlindSpot);
(function (B) {
  B.labels.DEAUTH = "Deauth / disassociation";
  B.labels.EVILTWIN = "Possible evil twins";
  B.labels.PORTAL = "Captive portals";
  B.labels.WATCH = "Custom watch filters";
  B.WifiWatch = function () {
    this.states = [];
    this.attacks = [];
  };
  B.WifiWatch.prototype.observe = function (o, t, peers, previous) {
    var type = "",
      evidence = "",
      title = "",
      grade = "MED",
      cooldown = 30000,
      i,
      p;
    if (o.kind === "EVENT" && o.attack) {
      var d = o.attack;
      if (d.windowMs > 3000 || !(d.count >= 12 || d.broadcast >= 6))
        return null;
      type = "DEAUTH";
      title = "DEAUTH / DISASSOC FLOOD";
      grade = "HIGH";
      cooldown = 15000;
      evidence =
        d.count +
        " frames / " +
        d.windowMs +
        "ms; " +
        d.broadcast +
        " broadcast; reason " +
        d.reason;
      this.attacks.push({ channel: o.channel, at: t, mac: o.mac });
      if (this.attacks.length > 32) this.attacks.shift();
    } else if (o.kind === "WIFI" && !o.probe && o.name) {
      if (o.portal && o.portal.status === "portal") {
        type = "PORTAL";
        title = "CAPTIVE PORTAL OBSERVED";
        evidence = o.portal.detail;
      }
      if (!type)
        for (i = 0; i < o.matches.length; i++)
          if (o.matches[i].label === "Pwnagotchi") return null;
      if (!type)
        for (i = 0; i < peers.length; i++) {
          p = peers[i];
          if (
            p.kind !== "WIFI" ||
            p.probe ||
            p.mac === o.mac ||
            p.name !== o.name ||
            t - p.elapsed > 120000
          )
            continue;
          if ((p.auth === "OPEN") !== (o.auth === "OPEN")) {
            type = "EVILTWIN";
            title = "POSSIBLE EVIL TWIN";
            evidence =
              "Same SSID: " + p.auth + " / " + o.auth + "; peer " + p.mac;
            break;
          }
        }
      if (
        !type &&
        previous &&
        previous.name === o.name &&
        (previous.auth === "OPEN") !== (o.auth === "OPEN")
      ) {
        type = "EVILTWIN";
        title = "AP SECURITY CHANGED";
        evidence =
          "Same BSSID changed " +
          previous.auth +
          " to " +
          o.auth +
          "; may be reconfiguration";
      }
      if (!type && !previous)
        for (i = this.attacks.length - 1; i >= 0; i--) {
          var ev = this.attacks[i];
          if (t - ev.at > 30000) continue;
          if (ev.channel === o.channel) {
            for (var j = 0; j < peers.length; j++) {
              p = peers[j];
              if (
                p.kind === "WIFI" &&
                !p.probe &&
                p.name === o.name &&
                p.mac !== o.mac &&
                t - p.elapsed < 120000
              ) {
                type = "EVILTWIN";
                title = "DEAUTH + DUPLICATE SSID";
                evidence =
                  "New BSSID after deauth on CH" +
                  o.channel +
                  "; possible attack chain";
                break;
              }
            }
          }
          if (type) break;
        }
      if (!type && /^evil[ _-]?portal(?:$|[ _-])/i.test(o.name)) {
        type = "PORTAL";
        title = "PORTAL TOOL NAME";
        grade = "LOW";
        evidence =
          "SSID advertises Evil Portal; name is spoofable. Use portal check.";
      }
    }
    if (!type) return null;
    var exists = false;
    for (i = 0; i < o.matches.length; i++)
      if (o.matches[i].type === type) exists = true;
    if (!exists)
      o.matches.push({
        type: type,
        label: title,
        grade: grade,
        certainty: type === "DEAUTH" ? "observed traffic" : "possible",
        evidence: evidence
      });
    o.warning = { type: type, evidence: evidence };
    var key = type + ":" + o.mac,
      st = null,
      a = this.states;
    for (i = a.length - 1; i >= 0; i--) {
      if (t - a[i].last > 600000 || t < a[i].last) a.splice(i, 1);
      else if (a[i].key === key) st = a[i];
    }
    if (!st) {
      st = { key: key, last: t, alerted: -600000 };
      a.push(st);
      if (a.length > 64) a.shift();
    }
    st.last = t;
    if (t - st.alerted < cooldown) return null;
    st.alerted = t;
    return {
      title: title,
      details: [
        o.name || o.mac,
        "CH" + o.channel + " " + o.mac,
        evidence,
        type === "DEAUTH"
          ? "Transmitter address may be spoofed."
          : "Review evidence; malicious intent unconfirmed.",
        "BACK dismisses; scanner stays active."
      ],
      routeAlert: true,
      device: o
    };
  };
  B.parseWatchFilter = function (text) {
    var at = String(text).indexOf(":"),
      type = String(text).slice(0, at).toUpperCase(),
      v = String(text)
        .slice(at + 1)
        .trim();
    if (
      at < 1 ||
      ["OUI", "MAC", "CID", "SVC", "NAME", "META", "PROBE", "BEACON"].indexOf(
        type
      ) < 0
    )
      throw Error("Use OUI, MAC, CID, SVC, NAME, META, PROBE or BEACON:value");
    if (type === "META") v = "META";
    else if (type === "CID" || type === "SVC") {
      v = v.replace(/^0x/i, "");
      if (!/^[0-9a-f]{1,4}$/i.test(v)) throw Error("Use a 16-bit hex ID");
      v = ("0000" + v).slice(-4).toUpperCase();
    } else if (type === "MAC") {
      if (!/^([0-9a-f]{2}:){5}[0-9a-f]{2}$/i.test(v))
        throw Error("Use six MAC bytes");
      v = v.toUpperCase();
    } else if (type !== "NAME") {
      if (!/^([0-9a-f]{2}:){2}[0-9a-f]{2}$/i.test(v))
        throw Error("Use three OUI bytes");
      v = v.toUpperCase();
    }
    if (!v || v.length > 40)
      throw Error("Filter value must be 1-40 characters");
    return { type: type, value: v };
  };
  B.applyWatchFilters = function (o, filters) {
    for (var i = 0; i < filters.length && i < 32; i++) {
      var f = filters[i],
        t = f.type,
        v = String(f.value || ""),
        match = false,
        publicMAC = !(parseInt(o.mac.slice(0, 2), 16) & 2);
      if (t === "MAC") match = o.mac === v;
      if (t === "NAME")
        match =
          o.kind === "BLE" &&
          o.name.toLowerCase().indexOf(v.toLowerCase()) >= 0;
      if (t === "OUI")
        match =
          o.kind === "BLE" &&
          o.addressType === 0 &&
          publicMAC &&
          o.mac.slice(0, 8) === v;
      if (t === "PROBE" || t === "BEACON")
        match =
          o.kind === "WIFI" &&
          publicMAC &&
          o.mac.slice(0, 8) === v &&
          (t === "PROBE" ? o.probe : !o.probe);
      if (t === "CID")
        match =
          o.kind === "BLE" &&
          (o.companyIDs || []).indexOf(parseInt(v, 16)) >= 0;
      if (t === "SVC")
        match =
          o.kind === "BLE" &&
          (o.serviceUUIDs || []).indexOf(parseInt(v, 16)) >= 0;
      if (t === "META")
        for (var j = 0; j < o.matches.length; j++)
          if (o.matches[j].type === "META" && o.matches[j].grade !== "LOW")
            match = true;
      if (match)
        for (var q = 0; q < o.matches.length; q++)
          if (
            o.matches[q].type === "WATCH" &&
            o.matches[q].label === t + " " + v
          )
            match = false;
      if (match)
        o.matches.push({
          type: "WATCH",
          label: t + " " + v,
          evidence:
            "User-configured filter match; not an identity or threat verdict",
          grade: "MED",
          certainty: "filter"
        });
    }
  };
})(BlindSpot);

BlindSpot.gpsState = function (s) {
  if (!s) return "WAIT GPS";
  if (s.fix && s.sentenceAge < 6000) return "GPS FIX";
  if (s.sentences > 0 && s.sentenceAge < 6000) return "NO FIX";
  if (s.bytes > 0 && s.byteAge < 6000) return "GPS DATA";
  return "NO DATA";
};

function runBlindSpot() {
  var B = BlindSpot,
    root = "/BlindSpot",
    cfgPath = root + "/config.json",
    cfg;
  function path(p) {
    return { fs: "sd", path: p };
  }
  function read(p) {
    return storage.read(path(p));
  }
  function write(p, s) {
    if (!storage.write(path(p), s, "write")) throw Error("SD write failed");
  }
  function atomic(p, s) {
    write(p + ".tmp", s);
    if (read(p + ".tmp") !== s) throw Error("SD verification failed");
    var had = exists(p);
    if (had) {
      if (exists(p + ".bak")) storage.remove(path(p + ".bak"));
      if (!storage.rename(path(p), p + ".bak")) throw Error("SD backup failed");
    }
    if (!storage.rename(path(p + ".tmp"), p)) {
      if (had) storage.rename(path(p + ".bak"), p);
      throw Error("SD commit failed");
    }
    if (had) storage.remove(path(p + ".bak"));
  }
  function list(p) {
    return storage.readdir(path(p));
  }
  function exists(p) {
    return storage.exists(path(p));
  }
  function mono() {
    return now();
  }
  var gold = 0xfd46,
    cream = 0xff39,
    dim = 0x8b65,
    bg = 0x0841,
    panel = 0x10c2,
    red = 0xf9e5;
  var screen = display.createSprite();
  function text(s, x, y, color, size) {
    screen.setTextSize(size || 1);
    screen.setTextColor(color || cream);
    screen.drawString(B.clean(s, 100), x, y);
  }
  function fill(x, y, w, h, c) {
    screen.drawFillRect(x, y, w, h, c);
  }
  function box(x, y, w, h) {
    fill(x, y, w, h, panel);
    screen.drawRect(x, y, w, h, gold);
  }
  function present() {
    screen.pushSprite();
  }
  function notice(s) {
    box(12, 52, 296, 70);
    text(s.slice(0, 45), 20, 64, gold);
    text(s.slice(45, 90), 20, 78);
    text("PRESS / BACK", 20, 103, dim);
    present();
    while (!keyboard.getSelPress() && !keyboard.getEscPress()) delay(30);
    delay(120);
    dirty = true;
  }
  var alertText = "",
    alertAt = -5000;
  var backAfter = 0;
  var dirty = true,
    stop = false,
    tab = 0,
    focus = false,
    idx = 0,
    group = null,
    modal = null,
    detailPage = 0;
  var cache = [],
    session = null,
    part = 0,
    pending = [],
    saved = 0,
    lastFlush = 0,
    error = "",
    running = false,
    channel = 1,
    phase = 0,
    nextScan = 0,
    drop = 0;
  var gpsInfo = "Bruce GPS settings",
    gpsNative = true,
    nativeGPS = null;
  var scanQueue = [],
    scanIndex = 0,
    scanKind = "",
    portalTarget = null,
    portalResume = false,
    holdChannel = 0,
    holdUntil = 0,
    holdStarted = 0;
  var wifiStats = {
    slices: 0,
    deauthFrames: 0,
    senders: 0,
    mode: "balanced",
    channel: 0
  };
  var bleStats = {
    records: 0,
    payloads: 0,
    manufacturers: 0,
    apple: 0,
    findMy: 0
  };
  var gps = new B.NMEA(),
    gpsBaud = 0,
    baudAt = 0,
    gpsOpen = false,
    latestGPS = null,
    scanCount = 0;
  var wifiWatch = new B.WifiWatch(),
    warnings = [];
  var routeWatch = new B.RouteWatch(),
    routePopup = null,
    popupUntil = 0;
  var tabs = ["DECK", "WI-FI", "BLE", "NOTABLE"],
    filterLow = false;
  var lastGPSPoll = -1000;
  function gpsPoll() {
    if (!gpsOpen) return;
    var t = mono();
    if (t - lastGPSPoll < 200) return;
    lastGPSPoll = t;
    if (gpsNative) {
      nativeGPS = JSON.parse(uart.gpsRead());
      latestGPS = nativeGPS.position || null;
      return;
    }
    gps.feed(uart.read(4096), t);
    latestGPS = gps.current(t);
    if (
      (gps.lastSentence < 0 || t - gps.lastSentence > 6000) &&
      t - baudAt > 5000
    ) {
      gpsBaud = (gpsBaud + 1) % cfg.gps.bauds.length;
      uart.begin(cfg.gps.bauds[gpsBaud], cfg.gps.rx, cfg.gps.tx);
      baudAt = t;
      gps.buffer = "";
    }
  }
  function position() {
    var f = gpsNative ? latestGPS : gps.current(mono());
    if (!B.validFix(f)) return null;
    return {
      lat: f.lat,
      lon: f.lon,
      ts: f.ts,
      alt: f.alt,
      accuracy: f.accuracy
    };
  }
  function gpsSnapshot() {
    if (gpsNative) return nativeGPS;
    return {
      rx: cfg.gps.rx,
      tx: cfg.gps.tx,
      baud: cfg.gps.bauds[gpsBaud],
      bytes: gps.byteCount,
      sentences: gps.good,
      badChecksum: gps.bad,
      byteAge: gps.lastByte < 0 ? 999999 : mono() - gps.lastByte,
      sentenceAge: gps.lastSentence < 0 ? 999999 : mono() - gps.lastSentence,
      fix: !!gps.current(mono()),
      lastLine: gps.lastLine
    };
  }
  function gpsLabel() {
    return cfg.gps.enabled ? B.gpsState(gpsSnapshot()) : "GPS OFF";
  }
  function memoryInfo() {
    return typeof device.getMemoryInfo === "function"
      ? JSON.parse(device.getMemoryInfo())
      : {};
  }
  function diagnostics() {
    var g = gpsSnapshot() || {},
      lines = [
        "GPS " + gpsLabel(),
        gpsInfo,
        "Bytes " +
          (g.bytes || 0) +
          " NMEA " +
          (g.sentences || 0) +
          " bad " +
          (g.badChecksum || 0),
        "Satellites " + (g.satellites || 0),
        "BLE records " + bleStats.records + " payloads " + bleStats.payloads,
        "Mfr data " + bleStats.manufacturers + " Apple " + bleStats.apple,
        "Find My frames " + bleStats.findMy,
        "Last GPS: " + (g.lastLine || "<none>").slice(0, 30),
        "BACK returns; scanner keeps running"
      ];
    var m = memoryInfo();
    lines.push(
      "WiFi " +
        wifiStats.mode +
        " CH" +
        wifiStats.channel +
        " slices " +
        wifiStats.slices
    );
    lines.push(
      "Deauth frames " +
        wifiStats.deauthFrames +
        " senders " +
        wifiStats.senders
    );
    lines.push(
      "Internal free " +
        (m.internalFree || 0) +
        " min " +
        (m.internalMinimum || 0)
    );
    lines.push("PSRAM free " + (m.psramFree || 0) + " reset " + m.resetReason);
    modal = { title: "R5 RADIO / GPS DIAGNOSTICS", details: lines };
    detailPage = 0;
    dirty = true;
  }
  function exportDiagnostics() {
    atomic(
      root + "/diagnostics.json",
      B.asciiJSON({
        version: "R5",
        memory: memoryInfo(),
        jsHeapBytes: typeof JS_HEAP_BYTES === "number" ? JS_HEAP_BYTES : null,
        gps: gpsSnapshot(),
        ble: bleStats,
        wifi: wifiStats,
        devices: cache
          .filter(function (o) {
            return o.kind === "BLE";
          })
          .slice(0, 64)
      })
    );
    notice("Saved /BlindSpot/diagnostics.json");
  }
  function notable(o) {
    for (var i = 0; i < o.matches.length; i++)
      if (filterLow || o.matches[i].grade !== "LOW") return true;
    return false;
  }
  function category(o, type) {
    for (var i = 0; i < o.matches.length; i++)
      if (
        o.matches[i].type === type &&
        (filterLow || o.matches[i].grade !== "LOW")
      )
        return true;
    return false;
  }
  function rows() {
    var a = [],
      i,
      k;
    if (tab === 3 && !group) {
      for (k in B.labels) {
        var n = 0;
        for (i = 0; i < cache.length; i++) if (category(cache[i], k)) n++;
        if (n) a.push({ category: k, name: B.labels[k], count: n });
      }
      return a;
    }
    for (i = 0; i < cache.length; i++)
      if (
        tab === 0
          ? notable(cache[i])
          : tab === 1
            ? cache[i].kind === "WIFI"
            : tab === 2
              ? cache[i].kind === "BLE"
              : category(cache[i], group)
      )
        a.push(cache[i]);
    return a;
  }
  function draw() {
    screen.fill(bg);
    screen.drawRect(0, 0, 320, 170, gold);
    text("BLIND SPOT", 7, 5, gold, 2);
    text(gpsLabel(), 213, 5, gold);
    text(String(device.getBatteryCharge()) + "%", 286, 5, cream);
    screen.drawLine(6, 25, 313, 25, gold);
    for (var i = 0; i < 4; i++) {
      var x = 6 + i * 78;
      if (i === tab) {
        fill(x, 30, 74, 15, gold);
        text(tabs[i], x + 4, 33, bg);
      } else text(tabs[i], x + 4, 33, dim);
    }
    var visible = tab === 0 ? 3 : 4;
    var a = rows(),
      start = Math.floor(idx / visible) * visible,
      left = tab === 0 ? 88 : 7,
      width = tab === 0 ? 222 : 302;
    if (tab === 0) {
      var wc = 0,
        bc = 0,
        nc = 0;
      for (i = 0; i < cache.length; i++) {
        if (cache[i].kind === "WIFI") wc++;
        else if (cache[i].kind === "BLE") bc++;
        if (notable(cache[i])) nc++;
      }
      box(6, 50, 76, 24);
      text("WI-FI", 11, 54, dim);
      text(wc, 48, 54, gold, 2);
      box(6, 77, 76, 24);
      text("BLE", 11, 81, dim);
      text(bc, 48, 81, gold, 2);
      box(6, 104, 76, 24);
      text("NOTABLE", 11, 108, dim);
      text(nc, 54, 118, gold);
      var bins = [],
        max = 1;
      for (i = 0; i < 14; i++) bins.push(0);
      for (i = 0; i < cache.length; i++)
        if (cache[i].kind === "WIFI" && !cache[i].probe) {
          bins[cache[i].channel - 1]++;
          max = Math.max(max, bins[cache[i].channel - 1]);
        }
      for (i = 0; i < 13; i++)
        fill(
          9 + i * 5,
          149 - Math.floor((17 * bins[i]) / max),
          3,
          Math.max(1, Math.floor((17 * bins[i]) / max)),
          gold
        );
      text("LATEST IDENTIFICATIONS", 92, 51, gold);
      screen.drawLine(89, 62, 308, 62, dim);
    }
    if (!a.length) {
      text("Listening...", left + 4, 66, gold);
      text("No matches yet", left + 4, 82, dim);
    }
    for (i = 0; i < visible && start + i < a.length; i++) {
      var r = a[start + i],
        y = (tab === 0 ? 67 : 51) + i * (tab === 0 ? 27 : 24),
        sel = focus && idx === start + i;
      if (sel) fill(left, y, width, 23, gold);
      var col = sel ? bg : cream;
      text(
        (r.category
          ? r.name
          : r.matches.length && tab !== 1 && tab !== 2
            ? r.matches[0].label
            : r.name || "<hidden>"
        ).slice(0, tab === 0 ? 32 : 45),
        left + 4,
        y + 2,
        col
      );
      text(
        r.category
          ? r.count + " observations"
          : r.kind +
              " " +
              r.rssi +
              "dBm " +
              (r.channel ? "CH" + r.channel + " " : "") +
              r.mac.slice(-8),
        left + 4,
        y + 13,
        sel ? bg : dim
      );
    }
    if (a.length > visible) {
      fill(313, 51, 3, 96, panel);
      fill(
        313,
        51 + Math.floor((96 * start) / a.length),
        3,
        Math.max(5, Math.floor((96 * visible) / a.length)),
        gold
      );
    }
    text(
      error
        ? error.slice(0, 50)
        : mono() - alertAt < 1500
          ? alertText.slice(0, 50)
          : running
            ? "REC " +
              saved +
              " saved | " +
              (phase ? "BLE" : "WI-FI") +
              " | drop " +
              drop
            : "PAUSED / SAVED",
      7,
      153,
      error ? red : dim
    );
    if (modal) drawModal();
    present();
    dirty = false;
  }
  function menu(title, items) {
    stopRadios();
    modal = { title: title, items: items, index: 0 };
    dirty = true;
  }
  function drawModal() {
    var m = modal;
    box(17, 39, 286, 111);
    text(m.title.slice(0, 43), 25, 46, gold);
    screen.drawLine(24, 59, 295, 59, dim);
    if (m.details) {
      var st = detailPage * 5;
      for (var j = 0; j < 5; j++)
        text((m.details[st + j] || "").slice(0, 43), 25, 64 + j * 13);
      text("TURN: PAGE  BACK: CLOSE", 25, 135, dim);
    } else {
      var start = Math.floor(m.index / 4) * 4;
      for (var i = 0; i < 4 && start + i < m.items.length; i++) {
        var y = 64 + i * 19;
        if (start + i === m.index) fill(24, y, 272, 18, gold);
        text(
          m.items[start + i].label.slice(0, 43),
          29,
          y + 4,
          start + i === m.index ? bg : cream
        );
      }
    }
  }
  function details(o) {
    var l = [
      o.name || "<hidden>",
      o.mac,
      o.kind + "  RSSI " + o.rssi + " dBm",
      o.kind === "WIFI"
        ? "Channel " + o.channel + "  " + o.auth
        : o.kind === "EVENT"
          ? "Channel " + o.channel
          : "Address type " + o.addressType,
      "Seen " + o.seen + " times",
      o.gps
        ? "Receiver " + o.gps.lat.toFixed(5) + "," + o.gps.lon.toFixed(5)
        : "Receiver GPS unavailable"
    ];
    for (var i = 0; i < o.matches.length; i++) {
      var h = o.matches[i];
      l.push(h.label + " [" + h.grade + "]");
      l.push(h.evidence);
      l.push(
        h.certainty === "signature"
          ? "Signature matched; not proof of intent"
          : "Possible match; not confirmed identity"
      );
    }
    if (o.remote) {
      var r = o.remote;
      l.push("REMOTE ID (broadcast claims)");
      if (r.serial) l.push("ID: " + r.serial);
      if (r.airframe !== undefined) l.push("Airframe type: " + r.airframe);
      if (r.lat !== undefined)
        l.push("Drone " + r.lat.toFixed(5) + "," + r.lon.toFixed(5));
      if (r.alt !== undefined) l.push("Drone altitude " + r.alt + " m");
      if (r.operatorLat !== undefined)
        l.push(
          "Operator " +
            r.operatorLat.toFixed(5) +
            "," +
            r.operatorLon.toFixed(5)
        );
    }
    l.push("PRESS: trust/untrust this address");
    if (o.findMy)
      l.push("Find My " + o.findMy.state + " / " + o.findMy.battery);
    if (o.kind === "BLE") {
      l.push("Payload " + (o.payloadBytes || 0) + " bytes");
      l.push("Mfr " + (o.manufacturerHex || "<none>").slice(0, 80));
    }
    if (o.beacon) {
      l.push("iBeacon " + o.beacon.uuid.slice(0, 16));
      l.push(o.beacon.uuid.slice(16));
      l.push("Major " + o.beacon.major + " Minor " + o.beacon.minor);
    }
    // Wrap evidence without hiding the rest of a long line.
    var lines = [];
    for (i = 0; i < l.length; i++) {
      var s = String(l[i]);
      do {
        lines.push(s.slice(0, 43));
        s = s.slice(43);
      } while (s.length);
    }
    modal = { title: "IDENTIFICATION DETAILS", details: lines, device: o };
    detailPage = 0;
    dirty = true;
  }
  function sessionNew() {
    scanQueue = [];
    scanIndex = 0;
    phase = 0;
    var names = list(root + "/sessions"),
      n = 1;
    while (names.indexOf("run-" + n) >= 0) n++;
    session = root + "/sessions/run-" + n;
    if (!storage.mkdir(path(session))) throw Error("Cannot create session");
    part = 0;
    pending = [];
    saved = 0;
    cache = [];
    routeWatch = new B.RouteWatch();
    wifiWatch = new B.WifiWatch();
    warnings = [];
    atomic(
      session + "/session.json",
      B.asciiJSON({ version: 1, name: "run-" + n, status: "recording" })
    );
    running = true;
    nextScan = mono();
    lastFlush = mono();
  }
  function flush() {
    if (!pending.length) return;
    var name = "part-" + ("000000" + part).slice(-6),
      json = B.asciiJSON({ version: 1, observations: pending }),
      csv = B.header;
    for (var i = 0; i < pending.length; i++)
      if (B.validFix(pending[i].gps) && !pending[i].probe)
        csv += B.csv(pending[i]);
    atomic(session + "/" + name + ".csv", csv);
    atomic(session + "/" + name + ".json", json);
    saved += pending.length;
    pending = [];
    part++;
    lastFlush = mono();
  }
  function finish() {
    running = false;
    stopRadios();
    drainScan(64);
    gpsPoll();
    flush();
    atomic(
      session + "/complete.json",
      B.asciiJSON({ parts: part, observations: saved })
    );
    dirty = true;
    uploadMenu(session);
  }
  function add(o) {
    if (!o) return;
    var t = mono(),
      at = -1,
      i,
      j,
      old,
      previous = null,
      selected = focus ? rows()[idx] : null;
    if (o.raw) o.raw = o.raw.slice(0, 128);
    B.applyWatchFilters(o, cfg.watchFilters || []);
    o.gps = position();
    o.elapsed = t;
    o.seen = 1;
    var routeAlert = routeWatch.observe(o, t, cfg.trusted);
    if (routeAlert && cfg.routeAlerts !== false) {
      routePopup = routeAlert;
    }
    for (i = 0; i < cache.length; i++)
      if (cache[i].mac === o.mac && cache[i].kind === o.kind) {
        at = i;
        break;
      }
    if (at >= 0) {
      old = cache[at];
      previous = old;
      o.seen = old.seen + 1;
      if (old.remote) {
        var rr = {};
        for (j in old.remote) rr[j] = old.remote[j];
        if (o.remote) for (j in o.remote) rr[j] = o.remote[j];
        o.remote = rr;
      }
      if (!o.matches.length && old.matches.length && !o.portal)
        o.matches = old.matches;
      cache.splice(at, 1);
    }

    var warning = wifiWatch.observe(o, t, cache, previous);
    if (
      warning &&
      cfg.smartWarnings !== false &&
      cfg.trusted.indexOf(o.mac) < 0
    ) {
      warnings.push(warning);
      if (warnings.length > 4) warnings.shift();
    }
    cache.unshift(o);
    if (cache.length > 256) cache.pop();
    if (selected) {
      var a = rows();
      for (i = 0; i < a.length; i++)
        if (selected.mac === a[i].mac && selected.kind === a[i].kind) {
          idx = i;
          break;
        }
    }
    // At most one log observation per device per 10 seconds; preserve newly discovered signatures.
    if (
      at < 0 ||
      !previous ||
      t - (o.lastLogged = previous.lastLogged || 0) > 10000 ||
      B.asciiJSON(o.matches) !== B.asciiJSON(previous.matches)
    ) {
      o.lastLogged = t;
      pending.push(o);
      if (pending.length >= 16) flush();
    }
    if (at < 0 && notable(o)) {
      alertText = "NEW: " + o.matches[0].label;
      alertAt = t;
    }
    dirty = true;
  }
  function drainScan(limit) {
    for (var n = 0; n < limit && scanIndex < scanQueue.length; n++) {
      var raw = scanQueue[scanIndex];
      scanQueue[scanIndex++] = null;
      var o = scanKind === "BLE" ? B.bleRaw(raw) : B.wifiRaw(raw);
      if (o && scanKind === "BLE") {
        bleStats.records++;
        if (o.payloadBytes) bleStats.payloads++;
        if (o.manufacturerHex) bleStats.manufacturers++;
        if (o.manufacturerHex.slice(0, 4) === "4c00") bleStats.apple++;
        if (o.findMy) bleStats.findMy++;
      }
      add(o);
      delay(1);
    }
    if (scanIndex >= scanQueue.length) {
      scanQueue = [];
      scanIndex = 0;
    }
  }
  function stopRadios() {
    wifi.surveyStop();
    ble.stopScan();
    wifi.portalCancel();
    portalTarget = null;
  }
  function scan() {
    gpsPoll();
    if (scanQueue.length) {
      drainScan(4);
      nextScan = mono() + 20;
      return;
    }
    var mode = cfg.scanMode || "balanced",
      locked = cfg.watchChannel || 0;
    if (mode === "ble") phase = 1;
    if (mode === "wifi" || locked) phase = 0;
    if (phase === 0) {
      var ch = locked || (mono() < holdUntil ? holdChannel : channel),
        r = wifi.survey(ch, locked ? 250 : 150, true);
      wifiStats.mode = mode;
      wifiStats.channel = ch;
      if (r === undefined) {
        nextScan = mono() + 20;
        return;
      }
      if (!r || !Array.isArray(r.frames))
        throw Error("Invalid Wi-Fi scan result");
      wifiStats.slices++;
      wifiStats.deauthFrames += r.deauth || 0;
      drop += r.dropped || 0;
      scanQueue = r.frames;
      scanKind = "WIFI";
      scanIndex = 0;
      var sec = r.security ? JSON.parse(r.security) : { senders: [] };
      wifiStats.senders = sec.senders.length;
      for (var i = 0; i < sec.senders.length; i++) {
        var d = sec.senders[i];
        if (d.count >= 12 || d.broadcast >= 6)
          add({
            kind: "EVENT",
            mac: d.mac,
            name: "Deauth / disassoc flood",
            rssi: d.rssi,
            channel: d.channel,
            probe: true,
            attack: d,
            matches: []
          });
      }
      if (
        r.deauth > 0 &&
        !locked &&
        (!holdStarted || mono() - holdStarted > 10000)
      ) {
        holdStarted = mono();
        holdChannel = ch;
        holdUntil = mono() + 3000;
      }
      if (!locked && mono() >= holdUntil) {
        channel++;
        if (channel > cfg.channels) {
          channel = 1;
          if (mode !== "wifi") phase = 1;
        }
      }
    } else {
      var a = ble.scan(1.0, true, true);
      if (a === undefined) {
        nextScan = mono() + 20;
        return;
      }
      if (!Array.isArray(a)) throw Error("Invalid BLE scan result");
      scanQueue = a;
      scanKind = "BLE";
      scanIndex = 0;
      phase = 0;
    }
    drainScan(4);
    gpsPoll();
    dirty = true;
    scanCount++;
    if (mono() - lastFlush > 4000) flush();
    nextScan = mono() + 20;
  }
  function beginPortal(o) {
    portalResume = running;
    stopRadios();
    drainScan(64);
    running = false;
    wifi.disconnect();
    wifi.portalStart(o.name, o.mac, o.channel);
    portalTarget = o;
    modal = {
      title: "CHECKING CAPTIVE PORTAL",
      details: [
        o.name,
        "Joining this selected open AP.",
        "Checking HTTP redirect / login response.",
        "No credentials submitted. BACK cancels."
      ],
      portalCheck: true
    };
    detailPage = 0;
    dirty = true;
  }
  function pollPortal() {
    if (!portalTarget) return;
    var raw = wifi.portalPoll();
    if (raw === undefined) return;
    var result = JSON.parse(raw),
      original = portalTarget,
      o = {};
    portalTarget = null;
    for (var k in original) o[k] = original[k];
    o.portal = result;
    o.matches = original.matches.filter(function (m) {
      return m.type !== "PORTAL";
    });
    add(o);
    warnings = warnings.filter(function (w) {
      return w.device !== o;
    });
    running = portalResume;
    phase = 0;
    nextScan = mono() + 100;
    modal = {
      title:
        result.status === "portal"
          ? "CAPTIVE PORTAL OBSERVED"
          : result.status === "no_portal_observed"
            ? "NO PORTAL OBSERVED"
            : "PORTAL CHECK INCONCLUSIVE",
      details: [
        o.name,
        result.detail || result.status,
        result.location || "",
        result.passwordForm
          ? "Page contains a password form."
          : "This does not establish network safety.",
        "BACK returns to scanning."
      ],
      routeAlert: true,
      device: o
    };
    popupUntil = mono() + 15000;
    detailPage = 0;
    dirty = true;
  }
  function deviceActions(o) {
    var items = [
      {
        label:
          (cfg.trusted.indexOf(o.mac) >= 0 ? "Untrust " : "Trust ") + o.mac,
        fn: function () {
          var at = cfg.trusted.indexOf(o.mac);
          if (at >= 0) cfg.trusted.splice(at, 1);
          else {
            if (cfg.trusted.length >= 64) cfg.trusted.shift();
            cfg.trusted.push(o.mac);
          }
          saveConfig();
          modal = null;
        }
      }
    ];
    if (o.kind === "WIFI" && !o.probe) {
      items.push({
        label: "Monitor CH" + o.channel + " (Wi-Fi guard)",
        fn: function () {
          stopRadios();
          cfg.watchChannel = o.channel;
          cfg.scanMode = "wifi";
          phase = 0;
          saveConfig();
          modal = null;
          nextScan = mono();
        }
      });
      if (o.auth === "OPEN")
        items.push({
          label: "Check captive portal (connects)",
          fn: function () {
            beginPortal(o);
          }
        });
    }
    items.push({
      label: "Cancel",
      fn: function () {
        modal = null;
      }
    });
    menu("DEVICE ACTIONS", items);
  }
  function scanSettings() {
    menu("RADIO / WATCH FILTERS", [
      {
        label: "Mode: " + cfg.scanMode,
        fn: function () {
          stopRadios();
          cfg.scanMode =
            cfg.scanMode === "balanced"
              ? "wifi"
              : cfg.scanMode === "wifi"
                ? "ble"
                : "balanced";
          cfg.watchChannel = 0;
          phase = 0;
          saveConfig();
          scanSettings();
        }
      },
      {
        label: "Channel: " + (cfg.watchChannel || "HOP"),
        fn: function () {
          var v = keyboard.keyboard(
            String(cfg.watchChannel || 0),
            2,
            "0 = hop, or channel 1-13"
          );
          if (v !== null && v !== undefined && /^(0|[1-9]|1[0-3])$/.test(v)) {
            stopRadios();
            cfg.watchChannel = Number(v);
            if (cfg.watchChannel) {
              cfg.scanMode = "wifi";
              if (cfg.watchChannel > cfg.channels) cfg.channels = 13;
            }
            phase = 0;
            saveConfig();
          }
          scanSettings();
        }
      },
      {
        label: "Add watch filter (" + cfg.watchFilters.length + "/32)",
        fn: function () {
          var v = keyboard.keyboard("", 64, "TYPE:value, e.g. NAME:Ray-Ban");
          if (v) {
            try {
              if (cfg.watchFilters.length >= 32) throw Error("32 filter limit");
              cfg.watchFilters.push(B.parseWatchFilter(v));
              saveConfig();
            } catch (e) {
              notice(String(e.message || e));
            }
          }
          scanSettings();
        }
      },
      {
        label: "List / remove watch filters",
        fn: function () {
          var items = [];
          for (var i = 0; i < cfg.watchFilters.length; i++)
            (function (n) {
              items.push({
                label:
                  cfg.watchFilters[n].type + ":" + cfg.watchFilters[n].value,
                fn: function () {
                  cfg.watchFilters.splice(n, 1);
                  saveConfig();
                  scanSettings();
                }
              });
            })(i);
          items.push({ label: "Back", fn: scanSettings });
          menu("SELECT TO REMOVE FILTER", items);
        }
      },
      { label: "Back", fn: settings }
    ]);
  }
  function uploadMenu(dir) {
    menu("SESSION SAVED", [
      {
        label: "Upload to WiGLE",
        fn: function () {
          upload(dir, "wigle");
        }
      },
      {
        label: "Upload to WDGWars",
        fn: function () {
          upload(dir, "wdgwars");
        }
      },
      {
        label: "Upload to Wardrift",
        fn: function () {
          upload(dir, "wardrift");
        }
      },
      {
        label: "Keep offline / Back",
        fn: function () {
          modal = null;
        }
      },
      { label: "API key settings", fn: settings },
      {
        label: "Upload all three",
        fn: function () {
          uploadAll(dir);
        }
      }
    ]);
  }
  function upload(dir, service, bulk) {
    var outcome = { service: service, accepted: 0, skipped: 0, message: "" };
    function status(msg) {
      outcome.message = String(msg);
      if (!bulk) notice(msg);
    }
    var ca = read(root + "/roots.pem");
    if (ca.indexOf("BEGIN CERTIFICATE") < 0) throw Error("Install roots.pem");
    var key = cfg[service],
      token = service === "wigle" ? key.encoded : key.key;
    if (!token) {
      status("Set " + service + " key in /BlindSpot/config.json");
      return outcome;
    }
    if (/[\r\n]/.test(token)) {
      status("Invalid API key characters");
      return outcome;
    }
    var parts = list(dir)
        .filter(function (x) {
          return /^part-\d{6}\.json$/.test(x);
        })
        .sort(),
      eligible = false;
    for (var pi = 0; pi < parts.length && !eligible; pi++) {
      var obs = JSON.parse(read(dir + "/" + parts[pi])).observations;
      for (var oi = 0; oi < obs.length; oi++)
        if (B.validFix(obs[oi].gps) && !obs[oi].probe) {
          eligible = true;
          break;
        }
    }
    if (!eligible) {
      status("No GPS-qualified rows. Session is saved locally.");
      return outcome;
    }
    stopRadios();
    running = false;
    modal = {
      title: "CONNECTING FOR UPLOAD",
      details: [
        "Stopping Wi-Fi capture and Bluetooth.",
        "Trying saved Bruce Wi-Fi networks.",
        "Choose your Internet Wi-Fi if prompted.",
        "Session files remain on the SD card."
      ]
    };
    detailPage = 0;
    draw();
    if (!wifi.connected()) {
      var joined = wifi.connectSaved();
      if (!joined && !wifi.connected()) joined = wifi.connectDialog();
      if (!wifi.connected()) {
        outcome.connectionFailed = true;
        status("Wi-Fi connection cancelled or failed; files kept");
        return outcome;
      }
    }
    var utc;
    try {
      utc = wifi.utcNow();
    } catch (clockError) {
      status("Connected to Wi-Fi, but time sync failed. Check Internet / NTP.");
      return outcome;
    }
    var accepted = 0,
      skipped = 0;
    for (var i = 0; i < parts.length; i++) {
      if (keyboard.getEscPress()) {
        outcome.cancelled = true;
        outcome.message = "Cancelled";
        break;
      }
      var base = parts[i].slice(0, -5),
        receipt = dir + "/" + base + "." + service + ".receipt",
        unknown = dir + "/" + base + "." + service + ".pending";
      if (exists(receipt)) {
        skipped++;
        continue;
      }
      if (exists(unknown) && service !== "wardrift") {
        status("Uncertain earlier upload. Check account; see README");
        break;
      }
      var batch = JSON.parse(read(dir + "/" + parts[i])).observations,
        qualified = batch.filter(function (o) {
          return B.validFix(o.gps) && !o.probe;
        }),
        csv = B.header;
      if (!qualified.length) continue;
      for (var j = 0; j < qualified.length; j++) csv += B.csv(qualified[j]);
      if (read(dir + "/" + base + ".csv") !== csv)
        throw Error("CSV integrity mismatch; upload stopped");
      var id = dir.slice(dir.lastIndexOf("/") + 1) + "-" + base,
        body,
        headers = {},
        url;
      if (service === "wardrift") {
        url = "https://wardrift.net/v1/ingest/signals";
        headers = { "X-API-Key": token, "Content-Type": "application/json" };
        // Persist a unique envelope before first attempt, reuse unchanged if recovered.
        var envelopePath = dir + "/" + base + ".wardrift.request",
          envelope;
        if (exists(envelopePath)) envelope = JSON.parse(read(envelopePath));
        else {
          var seq = Math.max(
            Number(cfg.wardrift.sequence || 0) + 1,
            Math.floor(Date.now() / 1000)
          );
          cfg.wardrift.sequence = seq;
          saveConfig();
          envelope = {
            idempotency_key: uuid(),
            sequence_no: seq,
            device_ts: utc,
            signature: "",
            items: B.wardriftItems(qualified)
          };
          atomic(envelopePath, B.asciiJSON(envelope));
        }
        body = B.asciiJSON(envelope);
      } else {
        var boundary = "BlindSpot-" + id;
        url =
          service === "wigle"
            ? "https://api.wigle.net/api/v2/file/upload"
            : "https://wdgwars.pl/api/upload-csv";
        headers = {
          "Content-Type": "multipart/form-data; boundary=" + boundary
        };
        if (service === "wigle") headers.Authorization = "Basic " + token;
        else headers["X-API-Key"] = token;
        body = B.multipart(
          csv,
          id + ".csv",
          boundary,
          service === "wigle" ? !!key.donate : null
        );
      }
      modal = {
        title: "UPLOADING " + service.toUpperCase(),
        details: [
          "Part " + (i + 1) + " / " + parts.length,
          "Local files will be retained.",
          "BACK cancels between parts."
        ]
      };
      detailPage = 0;
      draw();
      atomic(unknown, B.asciiJSON({ state: "in-flight", service: service }));
      var result;
      try {
        result = B.response(
          service,
          wifi.httpFetch(url, {
            method: "POST",
            headers: headers,
            body: body,
            ca: ca
          })
        );
      } catch (e) {
        status("Upload outcome unknown. Files kept; see README");
        break;
      }
      if (!result.ok) {
        if (result.unknown === false) storage.remove(path(unknown));
        status(result.status);
        break;
      }
      atomic(receipt, B.asciiJSON(result));
      storage.remove(path(unknown));
      accepted++;
      delay(1200);
    }
    outcome.accepted = accepted;
    outcome.skipped = skipped;
    if (!outcome.message)
      outcome.message =
        "Accepted " + accepted + " parts; already sent " + skipped;
    if (!bulk) {
      notice(outcome.message);
      uploadMenu(dir);
    }
    return outcome;
  }
  function uploadAll(dir) {
    var services = ["wigle", "wdgwars", "wardrift"],
      lines = [];
    for (var i = 0; i < services.length; i++) {
      var result;
      try {
        result = upload(dir, services[i], true);
      } catch (e) {
        result = {
          service: services[i],
          message: B.clean(e.message || e, 120),
          accepted: 0,
          skipped: 0
        };
      }
      lines.push(
        services[i].toUpperCase() +
          ": " +
          result.accepted +
          " accepted, " +
          result.skipped +
          " already sent"
      );
      lines.push(result.message);
      if (result.connectionFailed) {
        lines.push("Remaining destinations not attempted: no Wi-Fi.");
        break;
      }
      if (result.cancelled) {
        lines.push("Remaining destinations cancelled.");
        break;
      }
    }
    lines.push("Local files retained. BACK closes.");
    modal = { title: "UPLOAD ALL RESULTS", details: lines, routeAlert: true };
    detailPage = 0;
    popupUntil = mono() + 60000;
    dirty = true;
  }
  function uuid() {
    var s = "";
    for (var i = 0; i < 32; i++)
      s += Math.floor(Math.random() * 16).toString(16);
    return (
      s.slice(0, 8) +
      "-" +
      s.slice(8, 12) +
      "-4" +
      s.slice(13, 16) +
      "-a" +
      s.slice(17, 20) +
      "-" +
      s.slice(20)
    );
  }
  function saveConfig() {
    atomic(cfgPath, B.asciiJSON(cfg));
  }
  function openGPS() {
    gpsNative = cfg.gps.source !== "custom";
    if (gpsNative) {
      if (typeof uart.gpsRead !== "function")
        throw Error("R5 firmware required for native GPS");
      gpsInfo = String(uart.begin());
      nativeGPS = null;
    } else {
      uart.begin(cfg.gps.bauds[0], cfg.gps.rx, cfg.gps.tx);
      gpsInfo = "Custom RX " + cfg.gps.rx + " TX " + cfg.gps.tx;
    }
    gpsOpen = true;
    baudAt = mono();
  }
  function settings() {
    menu("API KEYS (stored on SD)", [
      {
        label: "WiGLE encoded API token",
        fn: function () {
          var k = keyboard.keyboard("", 256, "WiGLE encoded API token", true);
          if (k) {
            cfg.wigle.encoded = k;
            saveConfig();
          }
          settings();
        }
      },
      {
        label: "WDGWars API key",
        fn: function () {
          var k = keyboard.keyboard("", 256, "WDGWars key", true);
          if (k) {
            cfg.wdgwars.key = k;
            saveConfig();
          }
          settings();
        }
      },
      {
        label: "Wardrift API key",
        fn: function () {
          var k = keyboard.keyboard("", 256, "Wardrift key", true);
          if (k) {
            cfg.wardrift.key = k;
            saveConfig();
          }
          settings();
        }
      },
      {
        label: "GPS: " + (cfg.gps.enabled ? "ON" : "OFF"),
        fn: function () {
          cfg.gps.enabled = !cfg.gps.enabled;
          if (gpsOpen) uart.end();
          gpsOpen = false;
          gps = new B.NMEA();
          if (cfg.gps.enabled) openGPS();
          saveConfig();
          settings();
        }
      },
      {
        label:
          "Wi-Fi warnings: " + (cfg.smartWarnings !== false ? "ON" : "OFF"),
        fn: function () {
          cfg.smartWarnings = cfg.smartWarnings === false;
          saveConfig();
          settings();
        }
      },
      {
        label: "Route alerts: " + (cfg.routeAlerts !== false ? "ON" : "OFF"),
        fn: function () {
          cfg.routeAlerts = cfg.routeAlerts === false;
          saveConfig();
          settings();
        }
      },
      {
        label: "Clear trusted addresses",
        fn: function () {
          cfg.trusted = [];
          saveConfig();
          settings();
        }
      },
      { label: "Radio modes / watch filters", fn: scanSettings },
      { label: "GPS / BLE diagnostics", fn: diagnostics },
      { label: "Export diagnostics to SD", fn: exportDiagnostics },
      {
        label: "Show weak matches: " + (filterLow ? "ON" : "OFF"),
        fn: function () {
          filterLow = !filterLow;
          settings();
        }
      },
      {
        label: "Back",
        fn: function () {
          modal = null;
        }
      }
    ]);
  }
  function history() {
    var dirs = list(root + "/sessions")
        .filter(function (n) {
          return /^run-\d+$/.test(n);
        })
        .sort(function (a, b) {
          return Number(b.slice(4)) - Number(a.slice(4));
        }),
      items = [];
    for (var i = 0; i < dirs.length; i++)
      (function (name) {
        items.push({
          label: name,
          fn: function () {
            uploadMenu(root + "/sessions/" + name);
          }
        });
      })(dirs[i]);
    items.push({
      label: "Back",
      fn: function () {
        modal = null;
      }
    });
    menu("SAVED SESSIONS", items);
  }
  function actions() {
    var items = [
      {
        label: running ? "Stop, save & upload" : "Start new session",
        fn: function () {
          if (running) finish();
          else {
            if (pending.length) flush();
            wifi.disconnect();
            sessionNew();
            error = "";
            modal = null;
          }
        }
      },
      {
        label: "Saved sessions",
        fn: function () {
          if (running) {
            notice("Stop current session first");
            return;
          }
          history();
        }
      },
      { label: "API keys / settings", fn: settings },
      {
        label: "Resume viewing",
        fn: function () {
          modal = null;
        }
      },
      {
        label: "Save & exit",
        fn: function () {
          if (session) {
            running = false;
            stopRadios();
            drainScan(64);
            flush();
            atomic(
              session + "/complete.json",
              B.asciiJSON({ parts: part, observations: saved })
            );
          }
          stop = true;
        }
      }
    ];
    if (error)
      items.push({
        label: "Exit without saving",
        fn: function () {
          menu("DISCARD UNSAVED BUFFER?", [
            {
              label: "Keep app open",
              fn: function () {
                modal = null;
              }
            },
            {
              label: "Discard & exit",
              fn: function () {
                stop = true;
              }
            }
          ]);
        }
      });
    menu("BLIND SPOT", items);
  }
  function turn(d) {
    nextScan = mono() + 200;
    if (modal) {
      if (modal.details)
        detailPage = Math.max(
          0,
          Math.min(Math.ceil(modal.details.length / 5) - 1, detailPage + d)
        );
      else
        modal.index =
          (modal.index + d + modal.items.length) % modal.items.length;
    } else if (focus) {
      var a = rows();
      idx = Math.max(0, Math.min(a.length - 1, idx + d));
    } else {
      tab = (tab + d + 4) % 4;
      idx = 0;
      group = null;
    }
    dirty = true;
  }
  function select() {
    if (modal) {
      if (modal.device) {
        deviceActions(modal.device);
      } else if (!modal.details) modal.items[modal.index].fn();
    } else if (!focus) {
      focus = true;
      idx = 0;
    } else {
      var a = rows(),
        o = a[idx];
      if (o) {
        if (o.category) {
          group = o.category;
          idx = 0;
        } else details(o);
      }
    }
    dirty = true;
  }
  function back() {
    if (portalTarget) {
      wifi.portalCancel();
      portalTarget = null;
      running = portalResume;
      phase = 0;
      modal = null;
      backAfter = mono() + 400;
      dirty = true;
      return;
    }
    if (mono() < backAfter) return;
    if (modal) {
      if (modal.routeAlert || modal.failure) backAfter = mono() + 400;
      modal = null;
    } else if (group) {
      group = null;
      idx = 0;
    } else if (focus) {
      focus = false;
    } else actions();
    dirty = true;
  }
  try {
    if (display.width() !== 320 || display.height() !== 170)
      throw Error("Requires landscape 320x170");
    if (!storage.mkdir(path(root)) || !storage.mkdir(path(root + "/sessions")))
      throw Error("Insert writable SD card");
    cfg = JSON.parse(read(exists(cfgPath) ? cfgPath : cfgPath + ".bak"));
    if (!cfg.gps || !cfg.wigle || !cfg.wdgwars || !cfg.wardrift)
      throw Error("Install supplied config.json");
    if (
      typeof wifi.survey !== "function" ||
      typeof wifi.portalStart !== "function" ||
      typeof wifi.connectSaved !== "function" ||
      typeof ble.stopScan !== "function"
    )
      throw Error("Install Blind Spot firmware first");
    if (!Array.isArray(cfg.trusted)) cfg.trusted = [];
    if (!Array.isArray(cfg.watchFilters)) cfg.watchFilters = [];
    cfg.watchFilters = cfg.watchFilters.slice(0, 32);
    if (["balanced", "wifi", "ble"].indexOf(cfg.scanMode) < 0)
      cfg.scanMode = "balanced";
    cfg.watchChannel = Number(cfg.watchChannel) || 0;
    if (cfg.watchChannel < 0 || cfg.watchChannel > 13) cfg.watchChannel = 0;
    if (cfg.channels !== 11 && cfg.channels !== 13) cfg.channels = 11;
    if (cfg.gps.enabled) openGPS();
    wifi.disconnect();
    sessionNew();
    while (!stop) {
      try {
        gpsPoll();
        pollPortal();
        if (keyboard.getPrevPress()) turn(-1);
        if (keyboard.getNextPress()) turn(1);
        if (keyboard.getSelPress()) select();
        if (keyboard.getEscPress()) back();
        // Paint encoder input before any Wi-Fi slice or SD work. BLE polling never waits.
        if (dirty) draw();
        if (running && (!modal || modal.details) && mono() >= nextScan) scan();
        if (modal && modal.routeAlert && mono() > popupUntil) {
          modal = null;
          dirty = true;
        }
        if (routePopup && !modal) {
          var ev = routePopup.routeEvidence;
          modal = {
            title: "REPEATED DEVICE ON ROUTE",
            details: [
              routePopup.mac,
              ev.locations +
                " places, " +
                ev.seconds +
                "s, " +
                ev.spanMeters +
                "m",
              "Same BLE address seen as you moved.",
              "May be your device or a fellow traveller.",
              "Not proof of following. BACK to dismiss."
            ],
            routeAlert: true,
            device: routePopup
          };
          detailPage = 0;
          popupUntil = mono() + 10000;
          routePopup = null;
          dirty = true;
        }
        if (warnings.length && !modal) {
          modal = warnings.shift();
          detailPage = 0;
          popupUntil = mono() + 10000;
          dirty = true;
        }
        if (dirty) draw();
        delay(20);
      } catch (scanError) {
        running = false;
        stopRadios();
        error = B.clean(scanError.message || scanError, 80);
        modal = {
          title: "SCANNER PAUSED",
          details: [
            error.slice(0, 43),
            error.slice(43),
            "BACK closes this message; app stays open.",
            "Use menu to save or restart the session."
          ],
          failure: true
        };
        detailPage = 0;
        dirty = true;
        draw();
        delay(20);
      }
    }
  } catch (e) {
    running = false;
    error = B.clean(e.message || e, 90);
    notice(error);
  }
  stopRadios();
  if (gpsOpen) uart.end();
  screen.deleteSprite();
}
if (typeof module !== "undefined" && module.exports) module.exports = BlindSpot;
else runBlindSpot();

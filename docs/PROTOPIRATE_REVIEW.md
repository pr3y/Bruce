# RF, EMV and ProtoPirate RX1: maintainer review guide

This consolidates #2961 and its #2962 main backport, then adds the ProtoPirate
4.0 interpreter app and RX1 corrections. The starting commit is current upstream
`dev`, `dc089b920aab214c32fba89ad863a45c821a1627`, not either older PR head.
The latest dev board, platform, pins and shared-bus implementation are retained.

## Change map

| Area / files | Problem and resulting behavior |
| --- | --- |
| `src/modules/rf/rf_utils.*` | Include the antenna-switch block for `T_EMBED_1101` as well as legacy `T_EMBED`; retain the existing CS guard and 315/433/868/915 MHz routing. Preserve an already initialized radio when setting up RX; return allocation failure instead of aborting. |
| `src/modules/rf/protocols/rf_decoder.*` | RX sessions accept a bounded capture size. JS RAW requests 8192 RMT symbols (up to 16384 durations); normal RX remains 256 symbols. Preserve full-buffer reporting, cleanup and the existing M5 GPIO receiver behavior. |
| `src/modules/rf/rf_scan.*`, `rf_raw_capture.h` | Apply and verify named CC1101 receive presets after initialization. Preserve signed durations without repeat normalization, split RAW lines at 512 values, report preset/register/full-buffer metadata, poll promptly, use wrap-safe timeouts and preserve Back for JS. |
| `src/modules/rf/protocols/rf_encoder.cpp` | Derive the RMT wait from the complete waveform duration plus margin instead of cutting long transmissions off at two seconds. Bound the wait to 60 seconds. |
| `src/modules/rfid/emv_protocol.*` | Portable bounded BER-TLV/APDU parser; discover variable-length AIDs and priorities, build dynamic PDOL, handle GPO 80/77, traverse AFL ranges, decode PAN/track-2 with padding, and handle SW61/SW6C with bounded retries. Optional cardholder/date fields do not prevent a successful read. |
| `src/modules/rfid/emv_reader.*` | Integrate the parser with PN532 I2C/SPI. Validate normal/extended frames, lengths and checksums; use the actual ISO14443-4 target; avoid response-length truncation and fragmented I2C status handling. Add bounded cancellation, ACK abort, field shutdown, adapter cleanup, scrollable results and explicit saves. Do not automatically print card details to Serial. |
| `lib/ProtoPirate/` | Pinned original protocol sources, compatibility layer, registry, scoped memory cleanup and JSON API. 26 decoder modules and 23 encoders; original licenses, file hashes and a reproducible local patch are included. |
| `src/modules/bjs_interpreter/protopirate_js.cpp`, `subghz_js.*` | Add the protocol API and complete-stream RMT transmit bridge. Validate frequency/preset, duration counts, ranges and duplicate fields; return actual TX status and release the radio. Existing raw-pulse TX APIs remain available. |
| `mqjs_stdlib.c`, `mqjs_stdlib.h`, `lib/mquickjs_headers/mqjs_stdlib.build.sha256` | Register new JS functions and regenerate the matching interpreter table/hash. Generated-header index changes account for much of that diff. |
| `sd_files/ProtoPirate.js` | ES5 frontend for live receive, RAW/key-file decoding, history, SD browsing/save, timing analysis, original RAW replay and emulation with button/counter editing. Emulation is available by default; transmitting is explicit and confirmation defaults to Cancel. |
| `storage_js.cpp` | Validate arguments, accept string/typed-array data, retain bytes across path-conversion allocations/GC, and return write/flush failure rather than unconditional success. |
| `interpreter.cpp` | Restore navigation footer labels after clearing a touch display for the script. |
| `patch.py` | Resolve installed unified or legacy ESP32 objcopy tools and verify patch success before moving the original archive. Bound external minifier requests and fall back to original functional assets; `BRUCE_OFFLINE_WEB_ASSETS=1` allows local gzip without that service. |
| `tests/`, `tools/import_protopirate.py`, attribution/docs | Reproducible host coverage, pinned vendor imports, dependency/build provenance and explicit feature/validation limits. |

## Protocol and RX1 corrections

Kia V1/V2 now emit their leading Manchester bit and count the seeded decoder bit
correctly. Kia V1 retains the full 57-bit key. Direct field handling supports the
full serial range, validates button/counter bounds and updates saved Key/CRC/
RawCnt fields consistently. Ford V0 starts Manchester encoding in the proper
state, reserves the needed upload capacity, validates its header/button/counter
fields and saves the rebuilt key. Subaru's signed-shift undefined behavior is
removed. These changes are isolated in `lib/ProtoPirate/bruce-protocol-fixes.patch`.

RX1 fixes the one-second capture timeout and the blocking result menu opened by
the first unmatched/noise capture. A synthetic three-repeat Chrysler waveform
lasts 2686.8 ms, before RMT's 30 ms idle completion. Auto profiles now dwell for
12 seconds by elapsed time; locked profiles use 30-second reads. Noise does not
advance the profile. Back stops receiving and keeps the last unmatched RAW in
History. **Receive matching signal** loads the file's frequency/preset, disables
hopping and begins receiving. Auto scanning is sequential and can still miss a
signal on another profile or across a timeout boundary.

## JavaScript contract and limits

All protocol functions are on `require('subghz')`. `protoInfo`, `protoDecode` and
`protoEncode` return JSON strings; the frontend parses and checks `ok`. Errors
have `ok:false` and `error`. `protoTransmit` returns a boolean.

| Function | Contract |
| --- | --- |
| `readRaw(timeoutSeconds=10, rxPreset='')` | Existing one-argument callers retain their default profile. Timeout is 1-60 seconds. Named presets require CC1101. Returns a RAW `.sub` string or an empty string. |
| `protoInfo()` | API version 1, pinned upstream revision, development status, emulation enabled, keystore availability and per-protocol encoder availability. |
| `protoDecode(subText)` | RAW pulse decoding or deserialization of an existing decoded key file. Returns frames with protocol, text, data, encoder and validation, plus result-limit/serialization status. Loading a key file does not verify an over-air waveform. |
| `protoEncode(decodedSubText)` | Generates bounded RAW and updated decoded fields without touching the radio. The frontend advances the counter for an explicit send; preview/cancel do not commit it. |
| `protoTransmit(rawSubText)` | One complete RMT transaction across all RAW lines. Rejects full-buffer captures, unsupported presets, invalid frequencies and malformed/oversized timings. |

Decode input is limited to 256 KiB, encode input to 16 KiB, durations to 16384,
each duration to 1,000,000 us and encoded/transmitted total time to 10 seconds.
At most 16 unique decoded records are returned. Native protocol allocations are
bounded to 512 active allocations / 1 MiB with C-only error unwinding and cleanup.
The scoped allocator and shared upload buffer are **not reentrant**: calls belong
to the existing single JS interpreter thread, not concurrent tasks. Reported
validation can be `not-verified`; an encoder module is not proof of integrity or
acceptance by a real receiver.

The app offers AM650/AM270 and FM476/FM238 live profiles. Older Bruce firmware
without these native functions must be upgraded with the app; the app displays
a compatibility error. RX1 itself only replaces the JS file when that matching
firmware is already installed. Native JS bindings retain the existing
`LITE_VERSION` / `DISABLE_INTERPRETER` build guards.

## Evidence and outstanding work

- Full T-Embed CC1101 non-LITE build succeeded. Application: 4,431,040 bytes in a
  4,653,056-byte partition; merged image: 4,496,576 bytes; static RAM: 127,168 bytes.
  Image hash, offsets and required ProtoPirate/NRF/EMV symbols were checked.
- Six synthetic protocol loopbacks pass. Kia V1/V2 and Ford V0 pass 600
  boundary/random/edit/save/reload cycles (1806 API calls in one process).
- Chrysler passes 38 native decode calls including paired frames, multiline RAW,
  leading noise, 32 jitter cases and rejection of truncated/invalid frames.
- RF tests cover five board/module configurations, RAW preset/timing preservation,
  buffer metadata, timeout reproduction and prompt Back. EMV tests include 5000
  malformed inputs and synthetic APDU/PN532 exchanges under ASan/UBSan.
- Frontend tests cover four receive flows, save/reload metadata, counter editing,
  default emulation, cancel/explicit TX and firmware compatibility. The complete
  noise-to-Chrysler UI flow runs in actual MQuickJS 0.0.6 with a 512000-byte limit.
- On 2026-10-15 the reporter confirmed Bruce transmission was received on their
  Flipper running original ProtoPirate. RF antenna improvement was also previously
  reporter-confirmed. This is evidence for the tested signal, not all protocols.

NRF24 implementation and board/shared-SPI defaults are inherited from this dev
baseline. NRF24 physical switching/communication, final EMV card/bus checks and
the full board matrix have not been independently verified here. Host tests do
not replace physical RF or card tests. Upstream build warnings remain, including
audio decoder bounds warnings; this PR does not claim they were fixed.

Full Flipper parity is not claimed: encrypted device keystores, custom F4/Honda
profiles, brute-force recovery, model database and remote analyzer are not ported.
Fiat V2, Ford V3 and Scher-Khan have no encoder in the pinned upstream source.
See `PROTOPIRATE_PORT.md` for commands and `PROTOPIRATE_BUILD_DEPENDENCIES.json`
for the exact compiled dependencies. No firmware binaries or machine-specific
PlatformIO paths are committed.

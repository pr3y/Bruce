# Attribution

Blind Spot is GPL-3.0-or-later; see LICENSE.

Detection tables and name/payload/Remote ID rule adaptations derive from
[SquachWatch-CYD](https://github.com/skizzophrenic/SquachWatch-CYD),
commit 6997feeba55f2c54ccc391afd5aa6e261cd55d28, GPL-3.0.
Used src/signatures.cpp, src/remote_id.cpp and docs/DETECTIONS.md. No UI assets,
mascot, theme or layout were copied. The pinned upstream files retain the historical caveats behind the rules.

The Apple advertisement heuristic is credited upstream to nyanBOX's
jbohack/airtag_detector.cpp (MIT); Flock signature research is credited upstream
to colonelpanichacks/flock-you (MIT), NitekryDPaul and DeFlockJoplin. The bounded
AD-structure matching in Blind Spot is a rewrite with more conservative labels.

Bruce firmware retains its existing copyright/license and THIRD_PARTY.md.
The app's original gold UI, NMEA validation, journaling and upload adapters are
provided as source under GPL-3.0-or-later with this package.

Upload contract references:
- WiGLE official Android ObservationUploader.java (WiGLE CSV 1.6 columns).
- https://www.wdgwars.pl/press (documented multipart API).
- https://github.com/TaoTheReaper/wardrift-sync/blob/main/src/index.js (observed Wardrift
  envelope contract; protocol reference only, implementation not copied).
Wardrift's integration is not validated against a live authenticated account.

OUI-SPY reference: https://github.com/colonelpanichacks/ouispy-detector/tree/296d4c34724fc8df986351036a89690414611589 . Published BLEOUIs.md / WiFiOUIs.md vendor facts and documented composite criteria informed original Blind Spot matching code. No OUI-SPY firmware implementation copied. Vendor hints are not product or intent identification.

R3 Find My field structure and family/status interpretation reference: https://github.com/malmeloo/FindMy.py/blob/main/findmy/scanner/scanner.py (blob 3b97372404e860c43e777f0b21fdcabdf0fe38ff). Matching is an original bounded JavaScript implementation; no key correlation or owner lookup implemented.

# Blind Spot

Counter-surveillance and GPS wardriving for the T-Embed CC1101 / Plus (320×170 display, PSRAM and SD). Runs through Bruce's JavaScript interpreter with the native APIs in this PR.

## Install

1. Install matching firmware built from this branch.
2. Copy `BlindSpot.js` to `/BruceJS/BlindSpot.js` on the SD card.
3. Create `/BlindSpot`, copy `roots.pem` there, and copy `config.example.json` as `config.json`. Preserve existing configuration and sessions when updating.
4. Open the script from Bruce. Local detection does not require GPS or API keys. For wardriving, connect a GPS receiver and set its pins/baud in Bruce's GPS settings.

## Counter-surveillance

Recognizes supported Find My/AirTag, Tile, SmartTag and Find Hub advertisements, camera/body-camera and surveillance-vendor hints, smart-glasses signatures, and drone Remote ID broadcasts. Shows the matching evidence and confidence; an OUI is a vendor association, not proof of a specific product.

With GPS, route-repeat warnings require the same BLE address/type at multiple locations over time; trusted devices are excluded. Rotating addresses are not linked into a claimed identity. Wi-Fi monitoring warns about per-transmitter deauth/disassociation bursts and same-name networks with conflicting security, and correlates nearby deauth activity with new duplicate SSIDs. These are suspicious patterns, not proof of following or malicious intent.

For an open network, select its details → Device actions → Check captive portal. This explicitly connects to the selected BSSID and probes for a redirect/HTML response; it never submits passwords. A legitimate captive portal can trigger it. Passive scanning cannot reliably identify a standalone evil portal.

Settings offer balanced Wi-Fi/BLE, Wi-Fi guard, BLE-only, channel monitoring, trusted devices and OUI/MAC/CID/SVC/NAME/META/PROBE/BEACON filters. Back dismisses alerts; Save & exit ends the app.

## Wardriving and uploads

GPS-qualified observations are saved as local CSV/JSON sessions. In Back → API keys / settings, enter WiGLE's encoded API credential, a WDG Wars API key and a Wardrift API key. The same values are stored in `/BlindSpot/config.json` (`wigle.encoded`, `wdgwars.key`, `wardrift.key`); defaults are empty. Credentials are stored on the SD card in plaintext, so do not share that file.

Stop, save & upload offers individual destinations and Upload all three. Scanning stops, saved Bruce Wi-Fi is tried first, then the network selector if needed. Use a 2.4 GHz Internet connection. TLS uses the supplied root certificates; each service keeps separate acknowledgement/retry records. Unknown POST outcomes are not blindly resent. GPS reception and authenticated uploads require on-device validation.

Detection data and adaptations are credited in THIRD_PARTY.md. No 5 GHz or Bluetooth Classic support is claimed for this ESP32-S3 implementation.

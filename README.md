![Bruce Main Menu](./media/pictures/bruce_banner.jpg)

# :shark: Bruce

Bruce is meant to be a versatile ESP32 firmware that supports a ton of offensive features focusing on facilitating Red Team operations.
It also supports m5stack products and works great with Cardputer, Sticks and M5Cores.

## :building_construction: How to install

### The easiest way to install Bruce is using our official Web Flasher!
### Check out: https://bruce.computer/flasher

Alternatively, you can download the latest binary from releases or actions and flash locally using esptool.py
```sh
esptool.py --port /dev/ttyACM0 write_flash 0x00000 Bruce-<device>.bin
```

**For m5stack devices**

If you already use M5Launcher to manage your m5stack device, you can install it with OTA

Or you can burn it directly from the [m5burner tool](https://docs.m5stack.com/en/download), just search for 'Bruce' (My official builds will be uploaded by "owner" and have photos.) on the device category you want to and click on burn


## :keyboard: Discord Server

Contact us in our [Discord Server](https://discord.gg/WJ9XF9czVT)!

## :bookmark_tabs: Wiki

For more information on each function supported by Bruce, [read our wiki here](https://github.com/pr3y/Bruce/wiki).
Also, [read our FAQ](https://github.com/pr3y/Bruce/wiki/FAQ)

## :computer: List of Features

### WiFi
- [x] Connect to WiFi
- [x] WiFi AP
- [x] Disconnect WiFi
- [X] [WiFi Atks](https://github.com/pr3y/Bruce/wiki/WiFi#wifi-atks)
    - [x] [Beacon Spam](https://github.com/pr3y/Bruce/wiki/WiFi#beacon-spam)
    - [x] [Target Atk](https://github.com/pr3y/Bruce/wiki/WiFi#target-atk)
        - [x] Information
        - [X] Target Deauth
        - [X] EvilPortal + Deauth
    - [X] Deauth Flood (More than one target)
- [X] [Wardriving](https://github.com/pr3y/Bruce/wiki/Wardriving)
- [X] [TelNet](https://github.com/pr3y/Bruce/wiki/WiFi#telnet)
- [X] [SSH](https://github.com/pr3y/Bruce/wiki/WiFi#ssh)
- [x] [RAW Sniffer](https://github.com/pr3y/Bruce/wiki/WiFi#raw-sniffer)
- [x] [DPWO-ESP32](https://github.com/pr3y/Bruce/wiki/WiFi#dpwo-esp32)
- [x] [Evil Portal](https://github.com/pr3y/Bruce/wiki/WiFi#evil-portal)
- [X] [Scan Hosts](https://github.com/pr3y/Bruce/wiki/WiFi#evil-portal)
- [x] [Wireguard Tunneling](https://github.com/pr3y/Bruce/wiki/WiFi#wireguard-tunneling)
- [x] Brucegotchi
    - [x] Pwnagotchi friend
    - [x] Pwngrid spam faces & names
        - [x] [Optional] DoScreen a very long name and face
        - [x] [Optional] Flood uniq peer identifiers

### BLE
- [X] [BLE Scan](https://github.com/pr3y/Bruce/wiki/BLE#ble-scan)
- [X] Bad BLE - Run Ducky scripts, similar to [BadUsb](https://github.com/pr3y/Bruce/wiki/Others#badusb)
- [X] BLE Keyboard - Cardputer Only (for now)
- [X] iOS Spam
- [X] Windows Spam
- [X] Samsung Spam
- [X] Android Spam
- [X] Spam All

### RF
- [x] Scan/Copy
- [x] [Custom SubGhz](https://github.com/pr3y/Bruce/wiki/RF#replay-payloads-like-flipper)
- [x] Spectrum
- [x] Jammer Full (sends a full squared wave into output)
- [x] Jammer Intermittent (sends PWM signal into output)
- [x] Config
    - [X] RF TX Pin
    - [X] RF RX Pin
    - [X] RF Module
        - [x] RF433 T/R M5Stack
        - [x] [CC1101 (Sub-Ghz)](https://github.com/pr3y/Bruce/wiki/CC1101)
    - [X] RF Frequency
- [x] Replay

### RFID
- [x] Read tag
- [x] Read 125kHz
- [x] Clone tag
- [x] Write NDEF records
- [x] Amiibolink
- [x] Chameleon
- [x] Write data
- [x] Erase data
- [x] Save file
- [x] Load file
- [x] Config
    - [X] [RFID Module](https://github.com/pr3y/Bruce/wiki/RFID#supported-modules)
        - [x] PN532
- [ ] Emulate tag

### IR
- [x] TV-B-Gone
- [x] IR Receiver
- [x] [Custom IR (NEC, NEC42, NECExt, SIRC, SIRC15, SIRC20, Samsung32, RC5, RC5X, RC6)](https://github.com/pr3y/Bruce/wiki/IR#replay-payloads-like-flipper)
- [x] Config
    - [X] Ir TX Pin
    - [X] Ir RX Pin

### FM
- [x] [Broadcast standard](https://github.com/pr3y/Bruce/wiki/FM#play_or_pause_button-broadcast-standard)
- [x] [Broadcast reserved](https://github.com/pr3y/Bruce/wiki/FM#no_entry_sign-broadcast-rerserved)
- [x] [Broadcast stop](https://github.com/pr3y/Bruce/wiki/FM#stop_button-broadcast-stop)
- [ ] [FM Spectrum](https://github.com/pr3y/Bruce/wiki/FM#ocean-fm-spectrum)
- [ ] [Hijack Traffic Announcements](https://github.com/pr3y/Bruce/wiki/FM#car-hijack-ta)
- [ ] [Config](https://github.com/pr3y/Bruce/wiki/FM#bookmark_tabs-config)

### NRF24
- [X] [NRF24 Jammer](https://github.com/pr3y/Bruce/wiki/BLE#nrf24-jammer)
- [X] 2.4G Spectrum
- [ ] Mousejack

### Scripts
- [X] [JavaScript Interpreter](https://github.com/pr3y/Bruce/wiki/Interpreter) [Credits to justinknight93](https://github.com/justinknight93/Doolittle)

### Others
- [X] Mic Spectrum
- [X] QRCodes
    - [x] Custom
    - [x] PIX (Brazil bank transfer system)
- [x] [SD Card Mngr](https://github.com/pr3y/Bruce/wiki/Others#sd-card-mngr)
    - [x] View image (jpg)
    - [x] File Info
    - [x] [Wigle Upload](https://github.com/pr3y/Bruce/wiki/Wardriving#how-to-upload)
    - [x] Play Audio
    - [x] View File
- [x] [LittleFS Mngr](https://github.com/pr3y/Bruce/wiki/Others#littlefs-mngr)
- [x] [WebUI](https://github.com/pr3y/Bruce/wiki/Others#webui)
    - [x] Server Structure
    - [x] Html
    - [x] SDCard Mngr
    - [x] Spiffs Mngr
- [x] Megalodon
- [x] [BADUsb (New features, LittleFS and SDCard)](https://github.com/pr3y/Bruce/wiki/Others#badusb)
- [x] USB Keyboard
- [x] [Openhaystack](https://github.com/pr3y/Bruce/wiki/Others#openhaystack)
- [x] [LED Control](https://github.com/pr3y/Bruce/wiki/Others#led-control)

### Clock
- [X] RTC Support
- [X] NTP time adjust
- [X] Manual adjust

### Connect (ESPNOW)
- [X] Send File
- [X] Receive File

### Config
- [x] Brightness
- [x] Dim Time
- [x] Orientation
- [X] UI Color
- [x] Boot Sound on/off
- [x] Clock
- [x] Sleep
- [x] Restart

## Specific functions per Device, the ones not mentioned here are available to all.
| Device     | CC1101   | NRF24     | Interpreter   | FMRadio   | PN532    | Mic_SPM1423   | BadUSB    | RGB Led | Speaker | LITE_MODE |
| ---        | ---      |  ---      | ---           | ---       | ---      | ---            | ---       | ---     | ---     | ---   |
| Cardputer  | :ok:     | :ok:      | :ok:          | :ok:      | :ok:     | :ok:           | :ok:      | :ok:    | NS4168  | :x: |
| StickCPlus2 | :ok:    | :ok:      | :ok:          | :ok:      | :ok:     | :ok:           | :ok:[here](https://github.com/pr3y/Bruce/wiki/Others#badusb)       | :x:     | Tone    | :x: |
| StickCPlus 1.1 | :ok: | :ok:      | :x:           | :ok:      | :ok:     | :ok:           | :ok:[here](https://github.com/pr3y/Bruce/wiki/Others#badusb)       | :x:     | Tone    | :ok:  |
| Core       | :x:      | :x:       | :x:           | :x:       | :x:      | :ok:           | :x:       | :x:     | Tone    | :x: |
| Core2      | :x:      | :x:       | :x:           | :x:       | :x:      | :ok:           | :x:       | :x:     | :x:     | :x: |
| CoreSe/SE  | :x:      | :x:       | :ok:          | :x:       | :x:      | :x:            | :ok:      | :x:     | :x:     | :x: |
| CYD-2432S028  | :ok:      | :ok:       | :ok:          | :x:       | :ok:      | :x:            | :ok:      | :x:     | :x:     | :x:* |


*LITE_MODE*: TelNet, SSH, DPWO, WireGuard, ScanHosts, RawSniffer, Brucegotchi, BLEBacon, BLEScan, Interpreter and OpenHaystack are NOT available for M5Launcher Compatibility
* CYD has a LITE_MODE version for Launcher Compatibility

## :sparkles: Why and how does it look?

Bruce stems from a keen observation within the community focused on devices like Flipper Zero. While these devices offered a glimpse into the world of offensive security, there was a palpable sense that something more could be achieved without being that overpriced, particularly with the robust and modular hardware ecosystem provided by m5stack products.

![Bruce Main Menu](./media/pictures/pic1.png)
![Bruce on M5Core](./media/pictures/core.png)
![Bruce on Stick](./media/pictures/stick.png)
![Bruce on CYD](./media/pictures/cyd.png)

Other media can be [found here](./media/).

## :clap: Acknowledgements

+ [@bmorcelli](https://github.com/bmorcelli) for new core and a bunch of new features, also porting to many devices!
+ [@IncursioHack](https://github.com/IncursioHack) for adding RF and RFID modules features.
+ [@Luidiblu](https://github.com/Luidiblu) for logo and UI design assistance.
+ [@eadmaster](https://github.com/eadmaster) for adding a lot of features.
+ [@rennancockles](https://github.com/rennancockles) for a lot of RFID code, refactoring and others features.
+ Everyone who contributed in some way to the project, thanks :heart:

## :construction: Disclaimer

Bruce is a tool for cyber offensive and red team operations, distributed under the terms of the Affero General Public License (AGPL). It is intended for legal and authorized security testing purposes only. Use of this software for any malicious or unauthorized activities is strictly prohibited. By downloading, installing, or using Bruce, you agree to comply with all applicable laws and regulations. This software is provided free of charge, and we do not accept payments for copies or modifications. The developers of Bruce assume no liability for any misuse of the software. Use at your own risk.


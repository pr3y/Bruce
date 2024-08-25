![Bruce Main Menu](./media/pictures/bruce_banner.jpg)

# :shark: Bruce

Bruce is meant to be a versatile ESP32 firmware that supports a ton of offensive features focusing on facilitating Red Team operations.
It also supports m5stack products and works great with Cardputer, Sticks and M5Cores.

## :building_construction: How to install

**For m5stack devices**

The easiest way to install Bruce is if you already use M5Launcher to manage your m5stack device, you can install it with OTA

Or you can burn it directly from the [m5burner tool](https://docs.m5stack.com/en/download), just search for 'Bruce' (My official builds will be uploaded by "owner" and have photos.) on the device category you want to and click on burn

Alternatively, you can also download the latest binary from releases and flash locally using esptool.py
```sh
esptool.py --port /dev/ttyACM0 write_flash 0x00000 Bruce.bin
```
or use a web flasher like https://web.esphome.io/

## :keyboard: Discord Server

Contact us in our [Discord Server](https://discord.gg/WJ9XF9czVT).

## :bookmark_tabs: Wiki

For more information on each function supported by Bruce, [read our wiki here](https://github.com/pr3y/Bruce/wiki).
Also, [read our FAQ](https://github.com/pr3y/Bruce/wiki/FAQ)

## :computer: List of Features

### WiFi
- [x] Connect to WiFi
- [x] WiFi AP
- [x] Disconnect WiFi
- [X] WiFi Atks
    - [x] Beacon Spam
    - [x] Target Atk
        - [x] Information
        - [X] Target Deauth
        - [X] EvilPortal + Deauth
    - [X] Deauth Flood (More than one target)
- [X] TelNet
- [X] SSH
- [x] RAW Sniffer
- [x] DPWO-ESP32
- [x] Evil Portal
- [X] Scan Hosts
- [x] Wireguard Tun

### BLE
- [X] BLE Beacon
- [X] BLE Scan
- [X] NRF24 Jammer
- [X] AppleJuice
- [X] SwiftPair
- [X] Android Spam
- [X] Samsung
- [X] SourApple
- [X] BT Maelstrom

### RF
- [x] Scan/Copy
- [x] Custom SubGhz
- [x] Spectrum
- [x] Jammer Full (sends a full squared wave into output)
- [x] Jammer Intermittent (sends PWM signal into output)
- [x] Config
    - [X] RF TX Pin
    - [X] RF RX Pin
    - [X] RF Module
    - [X] RF Frequency
- [x] Replay

### RFID
- [x] Read tag
- [x] Read 125kHz
- [x] Clone tag
- [x] Write NDEF records
- [x] Write data
- [x] Erase data
- [x] Save file
- [x] Load file
- [ ] Emulate tag

### IR
- [x] TV-B-Gone
- [x] IR Receiver
- [x] Custom IR (NEC, NEC42, NECExt, SIRC, SIRC15, SIRC20, Samsung32, RC5, RC5X, RC6)
- [x] Config
    - [X] Ir TX Pin
    - [X] Ir RX Pin

### Others
- [X] Mic Spectrum
- [X] QRCodes
- [x] SD Card Mngr
- [x] SPIFFS Mngr
- [x] WebUI
    - [x] Server Structure
    - [x] Html
    - [x] SDCard Mngr
    - [x] Spiffs Mngr
- [x] Megalodon
- [x] BADUsb (New features, SPIFFS and SDCard)
- [X] Openhaystack
- [X] JavaScript Interpreter (Credits to justinknight93)[https://github.com/justinknight93/Doolittle]

### Clock
- [X] Clock

### Config
- [x] Brightness
- [x] Dim Time
- [x] Orientation
- [X] UI Color
- [x] Clock
- [x] Sleep
- [x] Restart

## Specific functions per Device, the ones not mentioned here are available to all.
| Device     | CC1101   | NRF24     | Interpreter   | FMRadio   | Mic_SPM1423   | BadUSB    | RGB Led | Speaker | LITE_MODE |
| ---        | ---      |  ---      | ---           | ---       | ---           | ---       | ---     | ---     | ---   |
| Cardputer  | :ok:     | :ok:      | :ok:          | :x:       | :ok:          | :ok:      | :ok:    | NS4168  | :x: |
| StickCPlus2 | :ok:    | :ok:      | :ok:          | :ok:      | :ok:          | :x:       | :x:     | Tone    | :x: |
| StickCPlus 1.1 | :ok: | :ok:      | :ok:          | :x:       | :ok:          | :x:       | :x:     | Tone    | :ok:  |
| Core       | :x:      | :x:       | :x:           | :x:       | :ok:          | :x:       | :x:     | Tone    | :x: |
| Core2      | :x:      | :x:       | :x:           | :x:       | :ok:          | :x:       | :x:     | :x:     | :x: |
| CoreSe/SE  | :x:      | :x:       | :ok:          | :x:       | :x:           | :ok:      | :x:     | :x:     | :x: |

*LITE_MODE*: TelNet, SSH, DPWO, WireGuard, BLEBacon, BLEScan, FMRadio and OpenHaystack are NOT available for M5Launcher Compatibility

## :sparkles: Why and how does it look?

Bruce stems from a keen observation within the community focused on devices like Flipper Zero. While these devices offered a glimpse into the world of offensive security, there was a palpable sense that something more could be achieved without being that overpriced, particularly with the robust and modular hardware ecosystem provided by m5stack products.

![Bruce Main Menu](./media/pictures/pic1.png)
![Bruce on M5Core](./media/pictures/core.png)
![Bruce on Stick](./media/pictures/stick.png)

Other media can be [found here](./media/).

## :clap: Acknowledgements

+ [@bmorcelli](https://github.com/bmorcelli) for new core and a bunch of new features.
+ [@IncursioHack](https://github.com/IncursioHack) for adding RF and RFID modules features.
+ [@Luidiblu](https://github.com/Luidiblu) for logo and UI design assistance.
+ [@eadmaster](https://github.com/eadmaster) for adding a lot of features.
+ Everyone who contributed in some way to the project, thanks :heart:

## :construction: Disclaimer

Bruce is a tool for cyber offensive and red team operations, distributed under the terms of the Affero General Public License (AGPL). It is intended for legal and authorized security testing purposes only. Use of this software for any malicious or unauthorized activities is strictly prohibited. By downloading, installing, or using Bruce, you agree to comply with all applicable laws and regulations. This software is provided free of charge, and we do not accept payments for copies or modifications. The developers of Bruce assume no liability for any misuse of the software. Use at your own risk.


# Bruce

Bruce is meant to be a versatile ESP32 firmware that supports a ton of offensive features focusing to facilitate on Red Team operations.
It also supports m5stack products and works great with Cardputer and sticks.

# How to install
## For m5stack devices
The easiest way to install Bruce is if you already use M5Launcher to manage your m5stack device, you can install it with OTA

Or you can burn it directly from the [m5burner tool](https://docs.m5stack.com/en/download), just search for 'Bruce' on the device category you want to and click on burn

Alternatively you can also download the binary from releases and flash locally using esptool.py
```sh
esptool.py --port /dev/ttyACM0 write_flash 0x00000 Bruce.bin
```
or use a web flasher like https://web.esphome.io/

# Wiki
For more information on each function supported by Bruce, [read our wiki here](https://example.com).

# List of Features

## WiFi
- [x] Connect to WiFi (New)
- [x] WiFi AP (New)
- [x] Disconnect WiFi (New)
- [X] WiFi Atks
    - [x] Beacon Spam
    - [x] Target Atk
        - [x] Information
        - [X] Target Deauth
        - [X] EvilPortal + Deauth
    - [ ] Deauth Flood (More than one target)
- [X] TelNet
- [X] SSH
- [x] RAW Sniffer
- [x] DPWO-ESP32
- [x] Evil Portal (New features, SPIFFS and SDCard)
- [X] Scan Hosts
- [x] Wireguard Tun

## BLE
- [X] AppleJuice
- [X] SwiftPair
- [X] Android Spam (New?)
- [X] Samsung (New)
- [X] SourApple
- [X] BT Maelstrom

## RF
- [ ] Scan/Copy (New)
- [ ] Replay
- [x] Spectrum (New)

## RFID
- [x] Read and Write
- [ ] Replay ?

## Others
- [x] TV-B-Gone
- [x] SD Card Mngr (New)
- [x] SPIFFS Mngr (New)
- [x] WebUI (New)
    - [x] Server Structure
    - [x] Html
    - [x] SDCard Mngr
    - [x] Spiffs Mngr
- [x] Megalodon (New)
- [x] Custom IR (New, SPIFFS and SDCard)
- [x] BADUsb (New features, SPIFFS and SDCard)
- [X] Openhaystack

## Settings
- [x] Brightness
- [x] Orientation
- [x] Clock (New)
- [x] Restart

     


# 🦈 Bruce Firmware (Gabriwar Fork)

💡 _All credit goes to [pr3y/Bruce](https://github.com/pr3y/Bruce)._  

---

## 🚀 Flashing Instructions  
### 💻 Option 1: Using M5Stick-Launcher  
1. 📥 Go to the [Releases](https://github.com/pr3y/Bruce/releases) section and download the `.bin` file.  
2. ⚡ Use the [M5Stick-Launcher](https://github.com/bmorcelli/M5Stick-Launcher) to flash and launch the firmware.  

### 🧰 Option 2: Using `esptool.py`  
1. 🛠️ Install `esptool` using:  
```bash
pip install esptool
```  
2. 🔥 Flash the firmware using:  
```bash
esptool.py --port /dev/ttyACM0 write_flash 0x00000 Bruce-<device>.bin
```  
💡 *Replace `<device>` with the appropriate device name.*  

---

## 🌟 Features  

### 🛡️ Attacks  
- 💣 **Deauth + Password Verify Attack**  

### ⚙️ Quality of Life (QoL) Improvements  
- 📡 **Enhanced Wi-Fi Scan**: Displays RSSI and security type for each network.

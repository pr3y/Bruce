@echo off
echo ======================================
echo Select your device to compile:
echo ======================================
echo 1. m5stack-cardputer (ESP32-S3)
echo 2. m5stack-cplus2 (ESP32)
echo 3. m5stack-cplus1_1 (ESP32)
echo 4. m5stack-core2 (ESP32)
echo 5. m5stack-core (ESP32)
echo 6. m5stack-core4mb (ESP32)
echo 7. lilygo-t-embed-cc1101 (ESP32-S3)
echo ======================================
set /p choice=Enter the number corresponding to your device: 

if "%choice%"=="1" (
    esptool --chip esp32s3 merge_bin --output Bruce3_Cardputer.bin 0x0 .pio\build\m5stack-cardputer\bootloader.bin 0x8000 .pio\build\m5stack-cardputer\partitions.bin 0x10000 .pio\build\m5stack-cardputer\firmware.bin
) else if "%choice%"=="2" (
    esptool --chip esp32 merge_bin --output Bruce3_cplus2.bin 0x1000 .pio\build\m5stack-cplus2\bootloader.bin 0x8000 .pio\build\m5stack-cplus2\partitions.bin 0x10000 .pio\build\m5stack-cplus2\firmware.bin
) else if "%choice%"=="3" (
    esptool --chip esp32 merge_bin --output Bruce3_cplus1_1.bin 0x1000 .pio\build\m5stack-cplus1_1\bootloader.bin 0x8000 .pio\build\m5stack-cplus1_1\partitions.bin 0x10000 .pio\build\m5stack-cplus1_1\firmware.bin
) else if "%choice%"=="4" (
    esptool --chip esp32 merge_bin --output Bruce3_core2.bin 0x1000 .pio/build/m5stack-core2/bootloader.bin 0x8000 .pio/build/m5stack-core2/partitions.bin 0x10000 .pio/build/m5stack-core2/firmware.bin
) else if "%choice%"=="5" (
    esptool --chip esp32 merge_bin --output Bruce3_core.bin 0x1000 .pio/build/m5stack-core/bootloader.bin 0x8000 .pio/build/m5stack-core/partitions.bin 0x10000 .pio/build/m5stack-core/firmware.bin
) else if "%choice%"=="6" (
    esptool --chip esp32 merge_bin --output Bruce3_core4mb.bin 0x1000 .pio/build/m5stack-core4mb/bootloader.bin 0x8000 .pio/build/m5stack-core4mb/partitions.bin 0x10000 .pio/build/m5stack-core4mb/firmware.bin
) else if "%choice%"=="7" (
    esptool --chip esp32s3 merge_bin --output Bruce3_T-Embed_CC1101.bin 0x0 .pio\build\lilygo-t-embed-cc1101\bootloader.bin 0x8000 .pio\build\lilygo-t-embed-cc1101\partitions.bin 0x10000 .pio\build\lilygo-t-embed-cc1101\firmware.bin
) else (
    echo Invalid choice. Please run the script again and choose a valid option.
    exit /b
)

echo ======================================
echo Build complete for selected device.
echo ======================================
pause

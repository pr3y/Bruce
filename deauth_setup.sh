#!/bin/bash

# Define paths with proper home expansion

pio pkg install -p espressif32 -t toolchain-xtensa32

esp32_file="$HOME/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32/lib/libnet80211.a"
esp32s3_file="$HOME/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libnet80211.a"

esp32_file_temp="$HOME/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32/lib/libnet80211_temp.a"
esp32s3_file_temp="$HOME/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libnet80211_temp.a"

# Definir caminhos para as ferramentas objcopy
toolchain_esp32="$HOME/.platformio/packages/toolchain-xtensa-esp32/xtensa-esp32-elf/bin/objcopy"
toolchain_esp32s3="$HOME/.platformio/packages/toolchain-xtensa-esp32s3/xtensa-esp32s3-elf/bin/objcopy"

# Verificar se os arquivos existem antes de executar os comandos
if [[ -f "$esp32_file" && -f "$esp32s3_file" ]]; then
    # Execute objcopy commands for ESP32
    $toolchain_esp32 --weaken-symbol=ieee80211_raw_frame_sanity_check "$esp32_file" "$esp32_file_temp"
    
    # Rename the original file to .old
    mv "$esp32_file" "${esp32_file}.old"

    # Rename the _temp to original
    mv "$esp32_file_temp" "$esp32_file"

    # Execute objcopy commands for ESP32-S3
    $toolchain_esp32s3 --weaken-symbol=ieee80211_raw_frame_sanity_check "$esp32s3_file" "$esp32s3_file_temp"

    # Rename the original file to .old
    mv "$esp32s3_file" "${esp32s3_file}.old"

    # Rename the _temp to original
    mv "$esp32s3_file_temp" "$esp32s3_file"

    echo "Done."
else
    echo "One or more specified files were not found."
    exit 1
fi

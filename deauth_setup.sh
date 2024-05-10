#!/bin/bash

rem Define file paths
esp32_file="~/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32/lib/libnet80211.a"
esp32s3_file="~/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libnet80211.a"

esp32_file2="~/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32/lib/libnet80211_temp.a"
esp32s3_file2="~/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libnet80211_temp.a"

# Now execute objcopy commands (only if backups were created)
~/.platformio/packages/toolchain-xtensa-esp32/xtensa-esp32-elf/bin/objcopy --weaken-symbol=ieee80211_raw_frame_sanity_check $esp32_file $esp32_file2

# Rename the original file to .old
rn $esp32_file "libnet80211.a.old"

# Rename the _temp to original
rn $esp32_file2 "libnet80211.a"

# Now execute objcopy commands (only if backups were created)
~/.platformio/packages/toolchain-xtensa-esp32s3/xtensa-esp32s3-elf/bin/objcopy --weaken-symbol=ieee80211_raw_frame_sanity_check $esp32s3_file $esp32s3_file2

# Rename the original file to .old
rn $esp32s3_file "libnet80211.a.old"

# Rename the _temp to original
rn $esp32s3_file2 "libnet80211.a"

echo "Done."

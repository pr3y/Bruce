@echo off
setlocal enabledelayedexpansion

rem Define file paths
set "esp32_file=%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\lib\libnet80211.a"
set "esp32s3_file=%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32s3\lib\libnet80211.a"

set "esp32_file2=%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\lib\libnet80211_temp.a"
set "esp32s3_file2=%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32s3\lib\libnet80211_temp.a"

rem Now execute objcopy commands (only if backups were created)
"%userprofile%"\.platformio\packages\toolchain-xtensa-esp32\xtensa-esp32-elf\bin\objcopy --weaken-symbol=ieee80211_raw_frame_sanity_check "%esp32_file%" "%esp32_file2%"

rem Rename the original file to .old
ren "%esp32_file%" "libnet80211.a.old"

rem Rename the _temp to original
ren "%esp32_file2%" "libnet80211.a"

rem Now execute objcopy commands (only if backups were created)
"%userprofile%"\.platformio\packages\toolchain-xtensa-esp32s3\xtensa-esp32s3-elf\bin\objcopy --weaken-symbol=ieee80211_raw_frame_sanity_check "%esp32s3_file%" "%esp32s3_file2%"

rem Rename the original file to .old
ren "%esp32s3_file%" "libnet80211.a.old"

rem Rename the _temp to original
ren "%esp32s3_file2%" "libnet80211.a"


PAUSE


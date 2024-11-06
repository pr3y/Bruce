esptool --chip esp32s3 merge_bin --output Bruce3_Cardputer.bin 0x0 .pio\build\m5stack-cardputer\bootloader.bin 0x8000 .pio\build\m5stack-cardputer\partitions.bin 0x10000 .pio\build\m5stack-cardputer\firmware.bin

esptool --chip esp32 merge_bin --output Bruce3_cplus2.bin 0x1000 .pio\build\m5stack-cplus2\bootloader.bin 0x8000 .pio\build\m5stack-cplus2\partitions.bin 0x10000 .pio\build\m5stack-cplus2\firmware.bin

esptool --chip esp32 merge_bin --output Bruce3_cplus1_1.bin 0x1000 .pio\build\m5stack-cplus1_1\bootloader.bin 0x8000 .pio\build\m5stack-cplus1_1\partitions.bin 0x10000 .pio\build\m5stack-cplus1_1\firmware.bin

esptool --chip esp32 merge_bin --output Bruce3_core2.bin 0x1000 .pio/build/m5stack-core2/bootloader.bin 0x8000 .pio/build/m5stack-core2/partitions.bin 0x10000 .pio/build/m5stack-core2/firmware.bin

esptool --chip esp32 merge_bin --output Bruce3_core.bin 0x1000 .pio/build/m5stack-core/bootloader.bin 0x8000 .pio/build/m5stack-core/partitions.bin 0x10000 .pio/build/m5stack-core/firmware.bin

esptool --chip esp32 merge_bin --output Bruce3_core4mb.bin 0x1000 .pio/build/m5stack-core4mb/bootloader.bin 0x8000 .pio/build/m5stack-core4mb/partitions.bin 0x10000 .pio/build/m5stack-core4mb/firmware.bin

esptool --chip esp32s3 merge_bin --output Bruce3_T-Embed_CC1101.bin 0x0 .pio\build\lilygo-t-embed-cc1101\bootloader.bin 0x8000 .pio\build\lilygo-t-embed-cc1101\partitions.bin 0x10000 .pio\build\lilygo-t-embed-cc1101\firmware.bin
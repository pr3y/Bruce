#!/bin/bash
ESPTOOL_PATH="$HOME/.local/bin"
ESPTOOL=$([[ -f $ESPTOOL_PATH/esptool.py ]] && echo "$ESPTOOL_PATH/esptool.py" || echo "$ESPTOOL_PATH/esptool")

$ESPTOOL --chip esp32s3 merge_bin --output Bruce3_Cardputer.bin 0x0 .pio/build/m5stack-cardputer/bootloader.bin 0x8000 .pio/build/m5stack-cardputer/partitions.bin 0x10000 .pio/build/m5stack-cardputer/firmware.bin
$ESPTOOL --chip esp32 merge_bin --output Bruce3_cplus2.bin 0x1000 .pio/build/m5stack-cplus2/bootloader.bin 0x8000 .pio/build/m5stack-cplus2/partitions.bin 0x10000 .pio/build/m5stack-cplus2/firmware.bin
$ESPTOOL --chip esp32 merge_bin --output Bruce3_cplus1_1.bin 0x1000 .pio/build/m5stack-cplus1_1/bootloader.bin 0x8000 .pio/build/m5stack-cplus1_1/partitions.bin 0x10000 .pio/build/m5stack-cplus1_1/firmware.bin
$ESPTOOL --chip esp32 merge_bin --output Bruce3_core2.bin 0x1000 .pio/build/m5stack-core2/bootloader.bin 0x8000 .pio/build/m5stack-core2/partitions.bin 0x10000 .pio/build/m5stack-core2/firmware.bin
$ESPTOOL --chip esp32 merge_bin --output Bruce3_core16mb.bin 0x1000 .pio/build/m5stack-core16mb/bootloader.bin 0x8000 .pio/build/m5stack-core16mb/partitions.bin 0x10000 .pio/build/m5stack-core16mb/firmware.bin
$ESPTOOL --chip esp32 merge_bin --output Bruce3_core4mb.bin 0x1000 .pio/build/m5stack-core4mb/bootloader.bin 0x8000 .pio/build/m5stack-core4mb/partitions.bin 0x10000 .pio/build/m5stack-core4mb/firmware.bin

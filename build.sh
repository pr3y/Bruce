#!/bin/bash
~/.local/bin/esptool.py --chip esp32s3 merge_bin --output Bruce3_Cardputer.bin 0x0 .pio/build/m5stack-cardputer/bootloader.bin 0x8000 .pio/build/m5stack-cardputer/partitions.bin 0x10000 .pio/build/m5stack-cardputer/firmware.bin
~/.local/bin/esptool.py --chip esp32 merge_bin --output Bruce3_cplus2.bin 0x1000 .pio/build/m5stack-cplus2/bootloader.bin 0x8000 .pio/build/m5stack-cplus2/partitions.bin 0x10000 .pio/build/m5stack-cplus2/firmware.bin
~/.local/bin/esptool.py --chip esp32 merge_bin --output Bruce3_cplus1_1.bin 0x1000 .pio/build/m5stack-cplus1_1/bootloader.bin 0x8000 .pio/build/m5stack-cplus1_1/partitions.bin 0x10000 .pio/build/m5stack-cplus1_1/firmware.bin

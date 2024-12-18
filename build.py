Import("env")

env.AddCustomTarget(
    "build-firmware",
    ["$BUILD_DIR/bootloader.bin","$BUILD_DIR/partitions.bin","$BUILD_DIR/firmware.bin"],
    'pio pkg exec -p "tool-esptoolpy" -- esptool.py --chip esp32s3 merge_bin --output Bruce3_T-Embed_CC1101.bin 0x0 ${BUILD_DIR}\\bootloader.bin 0x8000 ${BUILD_DIR}\partitions.bin 0x10000 ${BUILD_DIR}\\firmware.bin'   
)
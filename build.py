from typing import TYPE_CHECKING, Any

if TYPE_CHECKING:
    Import: Any = None
    env: Any = {}

Import("env")

env.Append(CXXFLAGS=["-Wno-conversion-null"])
env.AddCustomTarget(
    "build-firmware",
    [
        "$BUILD_DIR/bootloader.bin",
        "$BUILD_DIR/partitions.bin",
        "$BUILD_DIR/firmware.bin",
    ],
    'pio pkg exec -p "tool-esptoolpy" -- esptool.py --chip ${BOARD_MCU} merge_bin --output Bruce-${PIOENV}.bin 0x0 ${BUILD_DIR}/bootloader.bin 0x8000 ${BUILD_DIR}/partitions.bin 0x10000 ${BUILD_DIR}/firmware.bin',
    "Build Firmware",
)

env.AddCustomTarget(
    name="upload-nobuild",
    dependencies=None,
    actions=[
        "platformio run -t upload -t nobuild -e ${PIOENV}",
    ],
    title="Upload Nobuild",
    description="Runs pio upload without building new firmware",
)

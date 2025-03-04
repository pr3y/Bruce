from typing import Any, TYPE_CHECKING

if TYPE_CHECKING:
    Import: Any = None
    env: Any = {}

Import("env")
env.AddCustomTarget(
    "build-firmware",
    ["$BUILD_DIR/bootloader.bin","$BUILD_DIR/partitions.bin","$BUILD_DIR/firmware.bin"],
    'pio pkg exec -p "tool-esptoolpy" -- esptool.py --chip ${BOARD_MCU} merge_bin --output Bruce-${PIOENV}.bin 0x0 ${BUILD_DIR}/bootloader.bin 0x8000 ${BUILD_DIR}/partitions.bin 0x10000 ${BUILD_DIR}/firmware.bin'   
)
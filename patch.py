from os.path import join, isfile
from os import rename, remove
import sys

Import("env")

FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
patchflag_path = join(FRAMEWORK_DIR, ".patched")
board_mcu = env.BoardConfig()
mcu = board_mcu.get("build.mcu", "")

# patch file only if we didn't do it befored
if not isfile(join(FRAMEWORK_DIR, ".patched")):
    original_file = join(FRAMEWORK_DIR, "tools", "sdk", mcu, "lib", "libnet80211.a")
    patched_file = join(FRAMEWORK_DIR, "tools", "sdk", mcu, "lib", "libnet80211.a.patched")

    env.Execute("pio pkg exec -p toolchain-xtensa-%s -- xtensa-%s-elf-objcopy  --weaken-symbol=s %s %s" % (mcu, mcu, original_file, patched_file))
    if(isfile("%s.old"%(original_file))):
        remove("%s.old"%(original_file))
    rename(original_file,"%s.old"%(original_file))
    env.Execute("pio pkg exec -p toolchain-xtensa-%s -- xtensa-%s-elf-objcopy  --weaken-symbol=ieee80211_raw_frame_sanity_check %s %s" % (mcu, mcu, patched_file, original_file))
    remove("%s.old"%(original_file))

    def _touch(path):
        with open(path, "w") as fp:
            fp.write("")

    env.Execute(lambda *args, **kwargs: _touch(patchflag_path))

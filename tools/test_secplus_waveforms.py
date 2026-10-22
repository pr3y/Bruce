"""Compare Bruce's actual C++ pulse builder to argilo's independent Python encoder.

Usage: python tools/test_secplus_waveforms.py --compiler /path/to/zig.exe
Requires the sibling secplus checkout. No radio or Arduino dependencies.
"""
import argparse
import importlib.util
from pathlib import Path
import random
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    spec = importlib.util.spec_from_file_location("reference", root.parent / "secplus/secplus.py")
    reference = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(reference)
    with tempfile.TemporaryDirectory(dir=root.parent) as tmp:
        tmp = Path(tmp)
        harness = tmp / "main.cpp"
        harness.write_text('''
#include "rf_secplus.h"
#include <cstdio>
#include <cstdlib>
int main(int argc, char **argv) {
    if (argc != 4) return 2;
    std::vector<int> out;
    if (!rf_secplus_durations(atoi(argv[1]) != 0, strtoul(argv[2], nullptr, 10),
                             strtoull(argv[3], nullptr, 10), out)) return 3;
    for (int d : out) printf("%d ", d);
}
''')
        exe = tmp / "waveforms.exe"
        obj = tmp / "secplus.o"
        subprocess.run([args.compiler, "cc", "-O1", "-c", str(root / "lib/secplus/src/secplus.c"),
                        "-o", str(obj)], check=True, capture_output=True)
        subprocess.run([args.compiler, "c++", "-std=c++17", "-O1", "-o", str(exe),
                        "-I" + str(root / "src/modules/rf/protocols"),
                        "-I" + str(root / "lib/secplus/src"), str(harness),
                        str(root / "src/modules/rf/protocols/rf_secplus.cpp"),
                        str(obj)], check=True, capture_output=True)
        rng = random.Random(0x5EC)
        count = 0
        for v2 in (False, True):
            cases = [(0, 0), (2, 10), (0x0ffffffe if v2 else 0xfffffffc,
                                      (1 << 40) - 1 if v2 else 3486784400)]
            cases += [(rng.randrange(1 << 28) if v2 else rng.randrange(1 << 31) * 2,
                       rng.randrange(1 << 40) if v2 else rng.randrange(3486784401))
                      for _ in range(100)]
            for rolling, fixed in cases:
                result = subprocess.check_output([str(exe), str(int(v2)), str(rolling), str(fixed)], text=True)
                actual = list(map(int, result.split()))
                bits = (reference.encode_v2_manchester(rolling, fixed, fast=False) if v2
                        else reference.encode_ook(rolling, fixed, fast=False)) * 4
                expected = []
                for bit in bits:
                    value = (250 if v2 else 500) * (1 if bit else -1)
                    if expected and (expected[-1] > 0) == (value > 0):
                        expected[-1] += value
                    else:
                        expected.append(value)
                assert actual == expected, (v2, rolling, fixed)
                assert sum(map(abs, actual)) == 800000
                assert actual[-1] < 0
                count += 1
        for v2, rolling, fixed in [(0, 0, 3486784401), (1, 1 << 28, 0), (1, 0, 1 << 40)]:
            result = subprocess.run([str(exe), str(v2), str(rolling), str(fixed)], capture_output=True)
            assert result.returncode == 3 and not result.stdout
        print(f"PASS: {count} reference waveform comparisons, plus invalid-input checks")


if __name__ == "__main__":
    main()

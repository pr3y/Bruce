# Rolling Code (CC1101)

RF → Rolling Code creates up to four independent Security+ or Security+ 2.0
remote profiles in ESP32 NVS. Designed for the T-Embed CC1101; other CC1101
boards use their configured pins. This feature is excluded from LITE menus.

1. Select New remote, the protocol, and the frequency matching your opener.
2. Name the profile. A new transmitter identity is generated and saved.
3. Put your opener into its normal remote-learning mode using its Learn button.
4. Open the profile and select Send. Follow the opener's pairing instructions;
   some models need more than one press. Then use Send for normal operation.

Security+ and Security+ 2.0 are different protocols. Check the opener manual
for its protocol and frequency. This sends single-frequency OOK frames;
compatibility with every multi-frequency opener has not been established.
Each press sends four identical frame pairs (about 800 ms).
Security+ 2.0 uses short wireless frames without supplemental keypad/PIN data.

The next counter is committed before transmission, advancing by two for
Security+ or one for Security+ 2.0. A TX failure consumes that counter. A storage
failure prevents transmission. Exhausted counters require a new paired profile.
Do not restore older flash/NVS backups: this can roll counters backward.
Deleting a Bruce profile does not remove it from the opener's learned remotes.
Erasing NVS or reflashing with a full erase loses profiles and requires pairing
again. Profiles are separate from Scan/copy and saved `.sub` files.

The encoder is vendored from argilo/secplus; the waveform builder is tested
against its independent Python implementation. Actual RF range, pairing, and
operation require testing on the device and opener.

## Reference waveform test

With Zig installed, clone the independent Python reference beside the firmware
checkout and pin it to the version used by the vendored library:

```sh
git clone https://github.com/argilo/secplus ../secplus
git -C ../secplus checkout c35de10b1da5ad97c996402ea9ba18a65932a3cc
python tools/test_secplus_waveforms.py --compiler /path/to/zig
```

The test compiles the actual C++ pulse builder and C library, compares 206
boundary/random cases against the Python encoder, and checks invalid inputs.
Build the target with `pio run -e lilygo-t-embed-cc1101`.

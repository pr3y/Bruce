"""Independent synthetic RX timings; no radio or transmitter is used.

These invented payloads satisfy the imported decoder's integrity relations.
They are not captures from a vehicle. The test checks receive framing, jitter,
noise recovery and truncation, not acceptance by a real vehicle.
"""
from pathlib import Path
import json
import os
import random
import subprocess
import sys

root = Path(__file__).resolve().parent
env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
header = ('Filetype: Flipper SubGhz RAW File\nVersion: 1\nFrequency: 315000000\n'
          'Preset: Ook650Async\nProtocol: RAW\nBruce_RX_Preset: Ook650Async\n'
          'Bruce_Buffer_Full: 0\n')


def waveform(payload):
    pulses = [300, -3700] * 24 + [300, -15600]
    for byte in bytes.fromhex(payload):
        for bit in range(7, -1, -1):
            pulses += [600, -3400] if byte & (1 << bit) else [300, -3700]
    return pulses + [300, -15600]


def raw(pulses):
    # Deliberately split mid-frame and mid-symbol; lines are not frame boundaries.
    return header + ''.join('RAW_Data: ' + ' '.join(map(str, pulses[i:i+73])) + '\n'
                            for i in range(0, len(pulses), 73))


calls = 0


def decode(pulses):
    global calls
    calls += 1
    result = subprocess.run([str(root / 'build/pp_cli'), 'decode'], input=raw(pulses),
                            text=True, capture_output=True, env=env)
    assert result.returncode == 0 and not result.stderr, result.stderr
    decoded = json.loads(result.stdout)
    assert decoded['ok'] and not decoded['serializationError'], decoded
    return decoded


def chrysler(decoded):
    return [f for f in decoded['frames'] if f['protocol'] == 'Chrysler V0']


a = waveform('00 12 34 56 78 D1 16 9A BC DE')
b = waveform('FC 12 34 56 78 12 70 9A BC DE')
burst = (a + b) * 3
duration_us = sum(map(abs, burst))
assert duration_us == 2686800
decoded = decode(burst)
frames = chrysler(decoded)
assert frames and all('Chk:OK' in f['text'] for f in frames), decoded
assert any('Plain_A:' in f['data'] and 'Plain_B:' in f['data'] for f in frames)
assert chrysler(decode(a)) and chrysler(decode(b))

rng = random.Random(2961)
for case in range(32):
    # Radio slicing introduces width error, while noise can precede the preamble.
    jittered = [(1 if p > 0 else -1) * (abs(p) + rng.randint(-60, 60)) for p in burst]
    noisy = [35, -45, 70, -80, 100, -90, 40, -50] + jittered
    actual = chrysler(decode(noisy))
    assert actual and actual[0]['data'] == frames[0]['data'], (case, actual)

# Neither an incomplete payload nor a malformed preamble is an identification.
assert not chrysler(decode(a[:50 + 79 * 2]))
assert not chrysler(decode(a[20:]))  # only 14 preamble pairs survive
bad = waveform('00 12 34 56 78 D0 16 9A BC DE')
assert not chrysler(decode(bad))

report = {'status': 'passed', 'protocol': 'Chrysler V0', 'native_decode_calls': calls,
          'synthetic_burst_ms': duration_us / 1000,
          'rmt_completion_ms_including_idle': duration_us / 1000 + 30,
          'checks': ['complete paired frames', 'each frame independently',
                     '32 noise and timing jitter cases', 'multiple RAW lines',
                     'reject truncated payload', 'reject short preamble',
                     'reject integrity failure'],
          'sanitizers': ['AddressSanitizer', 'UndefinedBehaviorSanitizer'],
          'hardware_tested': False}
if len(sys.argv) > 1:
    Path(sys.argv[1]).write_text(json.dumps(report, indent=2) + '\n')
if len(sys.argv) > 2:
    Path(sys.argv[2]).write_text(json.dumps({'raw': raw(burst), 'decoded': decoded,
                                           'duration_ms': duration_us / 1000 + 30}))
print(json.dumps(report, indent=2))

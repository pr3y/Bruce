#!/usr/bin/env python3
"""Import the pinned upstream engine plus documented Bruce compatibility fixes."""
from pathlib import Path
import hashlib, json, shutil, subprocess, sys
root = Path(__file__).resolve().parents[1]
source, flipper = map(Path, sys.argv[1:3])
commit = '4c9afe58f881a28ddb51015f63b84578e101068d'
actual = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=source, text=True).strip()
if actual != commit:
    raise SystemExit('Expected ProtoPirate commit ' + commit + ', got ' + actual)
flipper_manifest = json.loads((root / 'lib/ProtoPirate/FLIPPER_UPSTREAM.json').read_text())
for relative, expected in flipper_manifest['files'].items():
    if hashlib.sha256((flipper / relative).read_bytes()).hexdigest() != expected:
        raise SystemExit('Flipper source hash mismatch: ' + relative)
dest = root / 'lib/ProtoPirate/src'
dest.mkdir(parents=True, exist_ok=True)
skip = {'bruteforce_core', 'psa_crypto_bf', 'protocol_timings'}
manifest = {}
for p in (source / 'protocols').glob('*'):
    if p.suffix not in ('.c', '.h') or p.stem in skip: continue
    target = dest / 'protocols' / p.name
    target.parent.mkdir(parents=True, exist_ok=True)
    text = p.read_text().replace('#include "../protopirate_app_i.h"', '#include "../pp_platform.h"')
    target.write_text(text)
    manifest[str(p.relative_to(source))] = hashlib.sha256(p.read_bytes()).hexdigest()
shutil.copyfile(source / 'defines.h', dest / 'defines.h')
shutil.copyfile(source / 'LICENSE', dest.parent / 'LICENSE')
for relative in flipper_manifest['files']:
    p = flipper / relative
    if p.name == 'generic.c': continue
    target = dest / relative
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(p, target)
patch = dest.parent / 'bruce-protocol-fixes.patch'
subprocess.run(['git', 'apply', '--directory=lib/ProtoPirate/src', str(patch)], cwd=root, check=True)
metadata = {
    'commit': commit, 'version': '4.0',
    'repository': 'https://github.com/RocketGod-git/ProtoPirate',
    'files': manifest,
    'local_changes': ['Replace Flipper UI umbrella include with pp_platform.h',
        'Kia V1/V2: leading Manchester bit, decoder bit count, full serial range, saved fields',
        'Ford V0: first Manchester bit, upload capacity, field ranges, saved fields',
        'Subaru: remove undefined signed left shift'],
    'local_patch_sha256': hashlib.sha256(patch.read_bytes()).hexdigest(),
    'comparison_reference': {'repository': 'https://github.com/salikisazazil/Flipper-Zero-ESP32-Port',
        'branch': 'pr/protopirate-3.6.3', 'commit': '4c5e129911607afab93218051f5a4f57a7ac1b06'}
}
(dest.parent / 'UPSTREAM.json').write_text(json.dumps(metadata, indent=2) + '\n')

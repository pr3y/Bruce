"""Strict protocol gate: report every synthetic-loopback failure, then exit nonzero."""
from pathlib import Path
import json, os, subprocess, sys
root = Path(__file__).resolve().parent
env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
def run(op, data=''):
    r = subprocess.run([str(root/'build/pp_cli'), op], input=data, capture_output=True, text=True, env=env)
    if r.returncode or r.stderr: raise AssertionError(r.stderr or r.stdout)
    return json.loads(r.stdout)
def file(protocol, bits, fields='', preset='Ook650Async'):
    return f'Filetype: Flipper SubGhz Key File\nVersion: 1\nFrequency: 433920000\nPreset: {preset}\nProtocol: {protocol}\nBit: {bits}\nKey: 00 00 00 00 00 00 00 01\n{fields}'
def value(text, name):
    for line in text.splitlines():
        if line.startswith(name+':'): return line.split(':',1)[1].strip()
catalog=run('catalog')
assert len(catalog['protocols'])==26
assert sum(p['encoder'] for p in catalog['protocols'])==23
assert catalog['emulationEnabled'] is True
for protocol in ['Fiat V2','Ford V3','Scher-Khan']:
    assert not run('encode',file(protocol,64))['ok']
raw='Filetype: Flipper SubGhz RAW File\nVersion: 1\nFrequency: 433920000\nPreset: Ook650Async\nProtocol: RAW\nRAW_Data: '
for invalid in ['','0','12x -100','2147483648','1000001','-2147483648']:
    assert not run('decode',raw+invalid)['ok'],invalid
assert not run('decode',raw+'100 -100\nFrequency: 315000000\n')['ok']
assert run('decode',raw+'100 -100 100 -100')['frames']==[]
cases=[('Kia V0',61,'Type: 1\n','2FSKDev476Async'),('Kia V1',57,'','Ook650Async'),('Kia V2',53,'','2FSKDev476Async'),('Ford V0',64,'','Ook650Async'),('Subaru',64,'','Ook650Async'),('Mazda V0',64,'','Ook650Async')]
results=[]
for proto,bits,extra,preset in cases:
    result={'protocol':proto,'status':'failed'}
    try:
        source=file(proto,bits,'Serial: 123456\nBtn: 1\nCnt: 10\nRepeat: 3\n'+extra,preset)
        encoded=run('encode',source)
        assert encoded['ok'],encoded
        result['pulses']=encoded['pulses']
        decoded=run('decode',encoded['raw'])
        assert decoded['ok'],decoded
        matching=[f for f in decoded['frames'] if f['protocol']==proto]
        assert matching,'No matching decoded frame'
        record=matching[0]
        assert record['validation']!='failed',record['text']
        for name,expected in [('Serial','123456'),('Btn','1'),('Cnt','10')]:
            assert value(record['data'],name)==expected,record['text']
        annotated=record['data']+'Test_metadata: preserved\n'
        reloaded=run('decode',annotated)
        assert reloaded['frames'][0]['data']==annotated
        assert run('encode',record['data'])['ok'],record
        result['status']='passed'
    except (AssertionError,KeyError) as e:
        result['error']=str(e)
    results.append(result)
report={'emulation_enabled_by_default':True,'catalog':26,'encoder_modules':23,
        'malformed_input_and_api_contract':'passed','fixture_type':'synthetic',
        'roundtrips':results,'hardware_tested':False}
rendered=json.dumps(report,indent=2)+'\n'
if len(sys.argv)>1: Path(sys.argv[1]).write_text(rendered)
print(rendered,end='')
sys.exit(any(r['status']!='passed' for r in results))

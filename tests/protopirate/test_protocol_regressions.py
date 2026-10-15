from pathlib import Path
import json,os,random,subprocess,sys
root=Path(__file__).resolve().parent
proc=subprocess.Popen([str(root/'build/pp_cli'),'batch'],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0'))
calls=0
def run(op,data=''):
    global calls
    calls+=1
    proc.stdin.write(f'{op} {len(data.encode())}\n'+data);proc.stdin.flush()
    line=proc.stdout.readline()
    assert line,proc.stderr.read()
    return json.loads(line)
def fields(text):
    return dict(line.split(': ',1) for line in text.splitlines() if ': ' in line)
def replace(text,name,value):
    return '\n'.join(name+': '+str(value) if line.startswith(name+':') else line for line in text.split('\n'))
def check_frame(protocol,encoded,serial,button,counter):
    assert encoded['ok'],encoded
    decoded=run('decode',encoded['raw'])
    records=[r for r in decoded['frames'] if r['protocol']==protocol]
    assert len(records)==1,(protocol,decoded)
    r=records[0];f=fields(r['data']);u=fields(encoded['updated'])
    assert r['encoder'] and r['validation']!='failed',(protocol,r)
    assert (int(f['Serial']),int(f['Btn']),int(f['Cnt']))==(serial,button,counter),(protocol,f)
    assert f['Key']==u['Key'],(protocol,'saved key differs from transmitted key',f,u)
    for name in ('CRC','RawCnt','Checksum'):
        if name in f: assert u[name]==f[name],(protocol,name,u,f)
    loaded=run('decode',encoded['updated'])['frames'][0]
    assert loaded['text']==r['text'],(protocol,'file reload differs from RAW decode',loaded,r)
    return r
rng=random.Random(2961);summary=[]
for protocol,bits,max_button,max_counter,preset in [('Kia V1',57,255,4095,'Ook650Async'),('Kia V2',53,15,4095,'2FSKDev476Async'),('Ford V0',64,15,524287,'Ook650Async')]:
    cases=[(0,0,0),(0xFFFFFFFF,max_button,max_counter),(1,1,1),(0x80000000,2,256),(0x7FFFFFFF,4,255),(123456,1,10)]
    cases += [(rng.getrandbits(32),rng.randrange(max_button+1),rng.randrange(max_counter+1)) for _ in range(94)]
    for i,(serial,button,counter) in enumerate(cases):
        header=(0,0x40,0x7F)[i%3] if protocol=='Ford V0' else 0
        source=f'Filetype: Flipper SubGhz Key File\nVersion: 1\nFrequency: 433920000\nPreset: {preset}\nProtocol: {protocol}\nBit: {bits}\nKey: {header:02X} 00 00 00 00 00 00 01\nSerial: {serial}\nBtn: {button}\nCnt: {counter}\nRepeat: 1\n'
        record=check_frame(protocol,run('encode',source),serial,button,counter)
        # Exercise editing a captured record, not only fresh synthetic files.
        new_button=(button+1)%(max_button+1);new_counter=(counter+1)%(max_counter+1)
        edited=replace(replace(replace(record['data'],'Btn',new_button),'Cnt',new_counter),'Repeat',1)
        if 'Repeat: ' not in edited: edited+='Repeat: 1\n'
        check_frame(protocol,run('encode',edited),serial,new_button,new_counter)
    invalid=replace(source,'Cnt',max_counter+1)
    assert not run('encode',invalid)['ok']
    invalid=replace(source,'Btn',max_button+1)
    assert not run('encode',invalid)['ok']
    summary.append({'protocol':protocol,'input_cases':len(cases),'encode_decode_save_reload_cycles':len(cases)*2,'status':'passed'})
proc.stdin.close()
err=proc.stderr.read();assert proc.wait()==0 and not err,err
report={'status':'passed','native_calls_in_one_process':calls,'protocols':summary,'sanitizers':['AddressSanitizer','UndefinedBehaviorSanitizer'],'fixtures':'synthetic boundary and deterministic random records','physical_radio_tested':False}
rendered=json.dumps(report,indent=2)+'\n'
if len(sys.argv)>1: Path(sys.argv[1]).write_text(rendered)
print(rendered,end='')

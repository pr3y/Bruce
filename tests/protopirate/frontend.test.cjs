const fs=require('node:fs'),vm=require('node:vm'),assert=require('node:assert/strict');
const source=fs.readFileSync(__dirname+'/../../sd_files/ProtoPirate.js','utf8');
const ctx={PP_TEST:true};vm.createContext(ctx);vm.runInContext(source,ctx);
const P=ctx.PP;
const raw='Filetype: Flipper SubGhz RAW File\nVersion: 1\nFrequency: 315000000\nPreset: Ook650Async\nProtocol: RAW\nRAW_Data: 250 -500 70000\nRAW_Data: -5 1 -20\n';
assert.deepEqual(Array.from(P.rawPulses(raw)),[250,-500,70000,-5,1,-20]);
assert.equal(P.timings(raw).count,6);
assert.equal(P.timings(raw).duration,70776);
assert.equal(P.field(raw,'Frequency'),'315000000');
assert.equal(P.numeric(raw,'Cnt'),null);
assert.equal(P.setField('Cnt: 1\nCnt: 2\n','Cnt',3),'Cnt: 3\n');
assert.equal(P.field(P.emulationData('Serial: 7\nCnt: 9\nBtn: 1\n',2,10),'Cnt'),'10');
assert.throws(()=>P.emulationData('',1,-1));
assert.throws(()=>P.rawPulses(raw+'RAW_Data: 12junk\n'));
assert.throws(()=>P.rawPulses(raw+'RAW_Data: 0\n'));
assert.throws(()=>P.api('{"ok":false,"error":"encoder rejected"}'));
assert.equal(P.api('{"ok":true}').ok,true);
assert.equal(P.receiveProfile(raw).frequency,315);
assert.equal(P.receiveProfile(raw.replace('Ook650Async','FuriHalSubGhzPresetOok650Async')).preset,'Ook650Async');
assert.throws(()=>P.receiveProfile(raw.replace('315000000','100000000')));
assert.throws(()=>P.receiveProfile(raw.replace('Ook650Async','FuriHalSubGhzPresetCustom')));
// UI exits safely from the main menu and missing-firmware message.
function ui(support){
 let reads=0,tx=0;
 const display={width:()=>320,height:()=>170,color:()=>0,drawFillRect(){},drawString(){},setTextSize(){},setTextColor(){}};
 const keyboard={getEscPress:()=>true,getSelPress:()=>false,getPrevPress:()=>false,getNextPress:()=>false};
 const rf=support?{protoInfo:()=>'{"ok":true,"api":1,"emulationEnabled":true,"protocols":[]}',protoDecode(){reads++;},protoEncode(){},protoTransmit(){tx++;}}:{};
 const context={delay(){},require:n=>({display,keyboard,subghz:rf,storage:{}}[n])};
 vm.runInNewContext(source,context,{timeout:1000});assert.equal(reads,0);assert.equal(tx,0);
}
ui(true);ui(false);
assert.equal(P.nextCounter('Kia V1',4095),0);
assert.equal(P.nextCounter('Kia V2',4095),0);
assert.equal(P.nextCounter('Ford V0',524287),0);
assert.equal(P.nextCounter('Renault V0',255),0);
assert.equal(P.nextCounter('Subaru',10),11);
assert.equal(P.nextCounter('Subaru',null),null);
// Emulation is accessible immediately. Back on the confirmation must never TX;
// explicit Send must submit the encoder's complete RAW output exactly once.
function emulationFlow(send) {
 let calls=0, encoded=0, sent=0;
 const events=['next','sel','next','sel','next','next','sel',
   'prev','prev','prev','prev','sel',
   ...(send?['prev','sel','esc']:['esc']), 'esc','esc','esc','esc'];
 const press=n=>{if(++calls>2000)throw new Error('UI did not exit');if(events[0]===n){events.shift();return true;}return false;};
 const keyboard={getEscPress:()=>press('esc'),getSelPress:()=>press('sel'),getPrevPress:()=>press('prev'),getNextPress:()=>press('next')};
 const shown=[];
 const display={width:()=>320,height:()=>170,color:()=>0,drawFillRect(){},drawString(s){shown.push(s);},setTextSize(){},setTextColor(){}};
 const data='Protocol: Subaru\nCnt: 10\nBtn: 1\n';
 const rf={protoInfo:()=>JSON.stringify({ok:true,api:1,emulationEnabled:true,protocols:[]}),
  protoDecode:()=>JSON.stringify({ok:true,frames:[{protocol:'Subaru',data,text:'Subaru',encoder:true,validation:'not-verified'}]}),
  protoEncode:s=>{encoded++;assert.equal(P.field(s,'Cnt'),'11');return JSON.stringify({ok:true,pulses:6,raw,updated:data});},
  protoTransmit:s=>{sent++;assert.equal(s,raw);return true;}};
 const storage={readdir:()=>[{name:'fixture.sub',isDirectory:false,size:data.length}],read:()=>data};
 const context={delay(){},require:n=>({display,keyboard,subghz:rf,storage}[n])};
 vm.runInNewContext(source,context,{timeout:1000});
 assert.equal(events.length,0);assert.equal(encoded,1);assert.equal(sent,send?1:0);
 assert(shown.includes('4.0 DEV RX1 / Emulation ON'));
}
emulationFlow(false);emulationFlow(true);
console.log('Frontend: RAW parsing, metadata, counter editing, limits, error propagation and exit paths passed');

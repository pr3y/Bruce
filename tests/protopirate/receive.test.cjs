// Real native-decoder output, simulated readRaw completion and keyboard/display.
// Generate the fixture with test_chrysler_receive.py before running this test.
const fs=require('node:fs'),vm=require('node:vm'),assert=require('node:assert/strict');
const source=fs.readFileSync(__dirname+'/../../sd_files/ProtoPirate.js','utf8');
const fixture=JSON.parse(fs.readFileSync(process.argv[2],'utf8'));
const noise=fixture.raw.split('RAW_Data:')[0]+'RAW_Data: 45 -50 60 -70 80 -90\n';
const frame=fixture.decoded.frames.find(f=>f.protocol==='Chrysler V0');
assert(frame);

function run(kind) {
    let clock=0,checks=0,reads=0,decodes=0,frequency=0,writes=0,tx=0;
    const shown=[],profiles=[],saved=[];
    const matching=kind==='matching';
    const events=matching?['next','sel','next','sel']:['sel'];
    let stopped=false,listing=false,openedUnknown=false;
    function press(name) {
        if(++checks>15000)throw new Error(kind+': UI stalled before next receive');
        if(events[0]===name){events.shift();return true;}return false;
    }
    const keyboard={getEscPress:()=>press('esc'),getSelPress:()=>press('sel'),getPrevPress:()=>press('prev'),getNextPress:()=>press('next')};
    const display={width:()=>320,height:()=>170,color:()=>0,drawFillRect(){},setTextSize(){},setTextColor(){},
        drawString(s){
            shown.push(s);
            if(s==='Signal history'&&!listing){listing=true;events.push('sel');}
            if(s==='Unrecognized signal'&&kind==='history'&&!openedUnknown){
                openedUnknown=true;
                // Save the retained capture, then close result/history/main.
                events.push('next','next','next','sel','esc','esc','esc');
            }
        }};
    const radio={
        protoInfo:()=>JSON.stringify({ok:true,api:1,emulationEnabled:true,protocols:[]}),
        protoEncode:()=>{throw new Error('Receive must not encode');},
        protoTransmit:()=>{tx++;throw new Error('Receive must not transmit');},
        setFrequency:f=>{frequency=f;},
        readRaw:(timeout,preset)=>{
            reads++;profiles.push([frequency,preset,timeout]);
            assert(timeout*1000>fixture.duration_ms,'Burst is interrupted before RMT completion');
            if(kind==='scan'){
                clock+=timeout*1000;
                if(reads===2){events.push('esc','esc');stopped=true;}
                return '';
            }
            if(kind==='history'){
                if(reads===2){events.push('esc','next','next','sel');stopped=true;return '';}
                clock+=100;return noise;
            }
            if(reads===1&&!matching){clock+=100;return noise;}
            clock+=fixture.duration_ms;
            return fixture.raw.replace(/315000000/g,String(Math.round(frequency*1000000)));
        },
        protoDecode:data=>{
            decodes++;
            if(matching&&reads===0){
                events.push('next','next','next','next','next','next','sel');
                return JSON.stringify({ok:true,frames:[frame]});
            }
            if(data===noise)return JSON.stringify({ok:true,frames:[]});
            assert(data.includes('RAW_Data:'));
            // Close decoded result, stop listening, exit main. One frame avoids
            // unrelated frame-picker interaction; it is actual native output.
            events.push('esc','esc','esc');stopped=true;
            return JSON.stringify({ok:true,frames:[frame]});
        }
    };
    const storage={readdir:()=>[{name:'synthetic.sub',isDirectory:false,size:frame.data.length}],read:()=>frame.data,
        write:(path,data)=>{writes++;saved.push(data);return true;}};
    class Clock extends Date {static now(){return clock;}}
    vm.runInNewContext(source,{Date:Clock,delay:ms=>{clock+=ms;},require:name=>({display,keyboard,subghz:radio,storage}[name])},{timeout:3000});
    assert(stopped);assert.equal(events.length,0);assert.equal(tx,0);
    if(kind==='noise'){
        assert.equal(reads,2);assert.equal(decodes,2);
        assert.equal(profiles[0][1],profiles[1][1],'Noise must not advance preset');
        assert(!shown.includes('Unrecognized signal'),'Noise opened a modal result');
        assert(shown.includes('Chrysler V0'),'Complete live frame was not shown');
    }else if(matching){
        assert.equal(reads,1);assert.equal(frequency,315);assert.equal(profiles[0][1],'Ook650Async');
        assert(shown.includes('Locked to this frequency / preset'));
    }else if(kind==='scan'){
        assert.equal(reads,2);assert.deepEqual(profiles.map(p=>p[1]),['Ook650Async','2FSKDev476Async']);
    }else if(kind==='history'){
        assert.equal(reads,2);assert.equal(writes,1);assert.equal(saved[0],noise);
    }
    console.log('PASS: receive '+kind);
}
for(const kind of ['noise','matching','scan','history'])run(kind);

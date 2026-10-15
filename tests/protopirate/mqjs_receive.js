// Run after ProtoPirate.js (PP_TEST=true) and a generated PP_RX_FIXTURE object.
// Exercise the complete UI in Bruce's actual interpreter, without radio hardware.
var rxClock=0,rxChecks=0,rxReads=0,rxDecodes=0,rxSeen=false,rxEvents=['sel'];
Date.now=function(){return rxClock;};
function delay(ms){rxClock+=ms;}
function rxPress(key){
    if(++rxChecks>5000)throw new Error('Receive UI stalled');
    if(rxEvents[0]===key){rxEvents.shift();return true;}return false;
}
var rxKeyboard={getEscPress:function(){return rxPress('esc');},getSelPress:function(){return rxPress('sel');},getPrevPress:function(){return rxPress('prev');},getNextPress:function(){return rxPress('next');}};
var rxDisplay={width:function(){return 320;},height:function(){return 170;},color:function(){return 0;},drawFillRect:function(){},setTextSize:function(){},setTextColor:function(){},drawString:function(s){if(s==='Chrysler V0')rxSeen=true;if(s==='Unrecognized signal')throw new Error('Unmatched capture interrupted reception');}};
var rxRadio={
    protoInfo:function(){return '{"ok":true,"api":1,"emulationEnabled":true,"protocols":[]}';},
    protoEncode:function(){throw new Error('Unexpected encode');},
    protoTransmit:function(){throw new Error('Unexpected transmission');},
    setFrequency:function(){},
    readRaw:function(seconds,preset){
        if(seconds*1000<=PP_RX_FIXTURE.duration_ms)throw new Error('Receive window is too short');
        if(preset!=='Ook650Async')throw new Error('Noise changed the preset');
        rxReads++;
        if(rxReads===1){rxClock+=100;return 'Protocol: RAW\nBruce_RX_Preset: Ook650Async\nRAW_Data: 50 -60 70 -80\n';}
        if(rxReads!==2)throw new Error('Unexpected receive retry');
        rxClock+=PP_RX_FIXTURE.duration_ms;return PP_RX_FIXTURE.raw;
    },
    protoDecode:function(){
        rxDecodes++;
        if(rxDecodes===1)return '{"ok":true,"frames":[]}';
        rxEvents.push('esc');rxEvents.push('esc');rxEvents.push('esc');
        return JSON.stringify({ok:true,frames:[PP_RX_FIXTURE.decoded.frames[0]]});
    }
};
function require(name){if(name==='display')return rxDisplay;if(name==='keyboard')return rxKeyboard;if(name==='subghz')return rxRadio;return {};}
runProtoPirate();
if(rxReads!==2||rxDecodes!==2||!rxSeen||rxEvents.length)throw new Error('Receive flow did not finish correctly');
print('MQuickJS 0.0.6: noise retry, full Chrysler frame, same preset, result and Back passed');

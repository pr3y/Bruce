function check(v,msg){if(!v)throw new Error(msg);}
check(PP.field('Frequency: 433920000\n','Frequency')==='433920000','field');
check(PP.rawPulses('RAW_Data: 250 -500\nRAW_Data: 70000 -5\n').length===4,'multiline');
check(PP.timings('RAW_Data: 100 -200\n').mean===150,'timings');
check(PP.field(PP.emulationData('Cnt: 1\nBtn: 2\n',3,4),'Cnt')==='4','counter');
check(PP.receiveProfile('Frequency: 315000000\nPreset: FuriHalSubGhzPresetOok650Async\n').frequency===315,'receive frequency');
check(PP.receiveProfile('Frequency: 315000000\nPreset: FuriHalSubGhzPresetOok650Async\n').preset==='Ook650Async','receive preset');
print('MQuickJS 0.0.6 frontend smoke passed');

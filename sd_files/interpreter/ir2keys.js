

// flirc-like IR2Keyboard script
// use any IR remote to send commands to your pc
// requires: IR receiver + USBHID-capable board

var running=true;
badusbSetup();

while(running) {    
    print("waiting for IR signals...", 0 , 0);
    var curr_ir_signal = irRead(1); // 1s timeout
    
    print("received:");
    print(curr_ir_signal);
    
    if(curr_ir_signal) {
        // switch on curr_ir_signal
        //  https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDConsumerControl.h
        if(curr_ir_signal.includes("address: 04 00 00 00\ncommand: B0 00 00 00")) {
            print("pressed play");
            //badusbPress(0x20);  // KEY_SPACE
            badusbPress(0x2c);  // KEY_SPACE
            //badusbPressSpecial(0x00CD);  // HID_USAGE_CONSUMER_PLAY_PAUSE
        }
        else if(curr_ir_signal.includes("address: 04 00 00 00\ncommand: 8E 00 00 00")) {
            print("pressed next");
            badusbPrint("n");  // vlc next shortcut
            badusbPressRaw(0x11);  // HID_KEY_N
            //badusbPressSpecial(0x00B5); // HID_USAGE_CONSUMER_SCAN_NEXT
        }
        else if(curr_ir_signal.includes("address: 04 00 00 00\ncommand: 8F 00 00 00")) {
            print("pressed prev");
            badusbPrint("p");  // vlc next shortcut
            badusbPressRaw(0x13);  // HID_KEY_P
            //badusbPressSpecial(0x00B6);  // HID_USAGE_CONSUMER_SCAN_PREVIOUS
        }
        else if(curr_ir_signal.includes("address: 04 00 00 00\ncommand: 8F 00 00 00")) {
            print("pressed stop");
            //badusbPressSpecial(0x00B7);  // HID_USAGE_CONSUMER_STOP
        }
        else
            print("unknown button pressed");
    }
    
    var cmd = serialReadln(10);
    if(cmd.trim()=="q") running=false;
}

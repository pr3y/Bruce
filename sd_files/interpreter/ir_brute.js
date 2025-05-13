
// bruteforce IR signals 
// use at your own risk, some devices may lock you out as a security mechanism when spammed with this!

var value_prefix = 0x20DF0000; // enter a fixed value to use a prefix, if you know it
var no_bits = 16;  // 2-bytes range -> 2^16=65536 values to try
var delay_ms = 200;  // delay after each try
var protocol = "NEC";  // one of the most common protocols


function brute_force() {
    var max_val = value_prefix + (1 << no_bits);
    
    for ( var brute_val = value_prefix; brute_val < max_val ; brute_val++) {
        var curr_val = brute_val.toString(16).toUpperCase();
        
        drawString("sending", 3 , 0);
        drawString(curr_val, 3 , 16);
        drawString("hold any key to stop", 3 , 32);
        
        if(getAnyPress()) break;
            
        // example full cmd: IRSend {"Protocol":"NEC","Bits":32,"Data":"0x20DF10EF"}
        serialCmd("ir send {\"Protocol\":\"" + protocol + "\",\"Bits\":32,\"Data\":\"0x" + curr_val + "\"}");
            
        delay(delay_ms);
        fillScreen(0);
    }
}


while(true)
{  
  var choice = dialogChoice([
    "Init value:" + value_prefix, "value_prefix",
    "Range bits:" + no_bits, "no_bits",
    "Delay: " + delay_ms, "delay_ms",
    "Protocol:" + protocol, "protocol",
    "Start attack", "attack",
    ]
  )
  
  if(choice=="") break;  // quit
  else if(choice=="value_prefix") value_prefix = parseInt(keyboard(String(value_prefix), 32, "starting value"));
  else if(choice=="no_bits") no_bits = parseInt(keyboard(String(no_bits), 32, "bits to iterate"));
  else if(choice=="delay_ms") delay_ms = parseInt(keyboard(String(delay_ms), 32, "delay afear each try (in ms)"));
  else if(choice=="protocol") protocol = keyboard(protocol, 32, "Protocol");
  else if(choice=="attack") {
      if(!value_prefix || !no_bits || !delay_ms || !protocol) {
          dialogError("invalid params");
          continue;
      }
      brute_force();
  }
  
  fillScreen(0); // clear screen
}

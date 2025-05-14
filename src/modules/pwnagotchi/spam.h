#ifndef SPAM_H
#define SPAM_H

/*
Thanks to thoses developers for their projects:
* @7h30th3r0n3 : https://github.com/7h30th3r0n3/Evil-M5Core2 and https://github.com/7h30th3r0n3/PwnGridSpam
* @viniciusbo : https://github.com/viniciusbo/m5-palnagotchi
* @sduenasg : https://github.com/sduenasg/pio_palnagotchi

Thanks to @bmorcelli for his help doing a better code.
*/

void send_pwnagotchi_beacon_main();

extern volatile bool spamRunning;
extern volatile bool stop_beacon;
extern volatile bool dos_pwnd;
extern volatile bool change_identity;

#endif // SPAM_H

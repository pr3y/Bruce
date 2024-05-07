/*
Last Updated: 30 Mar. 2018
By Anton Grimpelhuber (anton.grimpelhuber@gmail.com)
                                                                                        
-----------------------------------------------------------  
Semver (http://semver.org/) VERSION HISTORY (newest on top):  
(date format: yyyymmdd; ex: 20161022 is 22 Oct. 2016)  
------------------------------------------------------------  
 - 20180330 - v1.4 - First port to ESP8266 (tested: wemos D1 mini) by Anton Grimpelhuber  
 - 20161022 - v1.3 - Semver versioning implemented; various code updates, clarifications, & comment additions, and changes to fix incompatibilities so it will now compile with latest versions of gcc compiler; also improved blink indicator routines & added the ability to stop the code-sending sequence once it has begun; by Gabriel Staples (http://www.ElectricRCAircraftGuy.com)  
 - 20101023 - v1.2 - Latest version posted by Ken Shirriff on his website here (http://www.righto.com/2010/11/improved-arduino-tv-b-gone.html) (direct download link here: http://arcfn.com/files/arduino-tv-b-gone-1.2.zip)  
 - 20101018 - v1.2 - Universality for EU (European Union) & NA (North America) added by Mitch Altman; sleep mode added by ka1kjz  
 - 2010____ - v1.2 - code ported to Arduino; by Ken Shirriff  
 - 20090816 - v1.2 - for ATtiny85v, by Mitch Altman & Limor Fried (https://www.adafruit.com/), w/some code by Kevin Timmerman & Damien Good  

TV-B-Gone for Arduino version 1.2, Oct 23 2010
Ported to Arduino by Ken Shirriff
See here: http://www.arcfn.com/2009/12/tv-b-gone-for-arduino.html and here: http://www.righto.com/2010/11/improved-arduino-tv-b-gone.html (newer)

I added universality for EU (European Union) or NA (North America),
and Sleep mode to Ken's Arduino port
 -- Mitch Altman  18-Oct-2010
Thanks to ka1kjz for the code for adding Sleep
 <http://www.ka1kjz.com/561/adding-sleep-to-tv-b-gone-code/>
 
The original code is:
TV-B-Gone Firmware version 1.2
 for use with ATtiny85v and v1.2 hardware
 (c) Mitch Altman + Limor Fried 2009
 Last edits, August 16 2009

With some code from:
Kevin Timmerman & Damien Good 7-Dec-07

------------------------------------------------------------
CIRCUIT:
------------------------------------------------------------
-NB: SEE "main.h" TO VERIFY DEFINED PINS TO USE
The hardware for this project uses a wemos D1 mini ESP8266-based board:
 Connect an IR LED to pin 14 / D5 (IRLED).
 Uses the built-in LED via pin 2.
 Connect a push-button between pin 12 / D6 (TRIGGER) and ground.
 Pin 5 / D1 (REGIONSWITCH) must be left floating for North America, or wire it to ground to have it output European codes.

More about the wiring is written in the readme.
------------------------------------------------------------
LICENSE:
------------------------------------------------------------
Distributed under Creative Commons 2.5 -- Attribution & Share Alike

*/

#include "TV-B-Gone.h"
#include "display.h"
#include "mykeyboard.h"
#include "WORLD_IR_CODES.h"

uint8_t read_bits(uint8_t count);
uint16_t rawData[300];


#define putstring_nl(s) Serial.println(s)
#define putstring(s) Serial.print(s)
#define putnum_ud(n) Serial.print(n, DEC)
#define putnum_uh(n) Serial.print(n, HEX)

#define MAX_WAIT_TIME 65535 //tens of us (ie: 655.350ms)

IRsend irsend(LED);  // Set the GPIO to be used to sending the message.

uint8_t bitsleft_r = 0;
uint8_t bits_r=0;
uint8_t code_ptr;
volatile const IrCode * powerCode;
uint8_t num_NAcodes = NUM_ELEM(NApowerCodes);
uint8_t num_EUcodes = NUM_ELEM(EUpowerCodes);
uint16_t ontime, offtime;
uint8_t i,num_codes;
uint8_t region;
/*
This project transmits a bunch of TV POWER codes, one right after the other,
 with a pause in between each.  (To have a visible indication that it is
 transmitting, it also pulses a visible LED once each time a POWER code is
 transmitted.)  That is all TV-B-Gone does.  The tricky part of TV-B-Gone
 was collecting all of the POWER codes, and getting rid of the duplicates and
 near-duplicates (because if there is a duplicate, then one POWER code will
 turn a TV off, and the duplicate will turn it on again (which we certainly
 do not want).  I have compiled the most popular codes with the
 duplicates eliminated, both for North America (which is the same as Asia, as
 far as POWER codes are concerned -- even though much of Asia USES PAL video)
 and for Europe (which works for Australia, New Zealand, the Middle East, and
 other parts of the world that use PAL video).

 Before creating a TV-B-Gone Kit, I originally started this project by hacking
 the MiniPOV kit.  This presents a limitation, based on the size of
 the Atmel ATtiny2313 internal flash memory, which is 2KB.  With 2KB we can only
 fit about 7 POWER codes into the firmware's database of POWER codes.  However,
 the more codes the better! Which is why we chose the ATtiny85 for the
 TV-B-Gone Kit.

 This version of the firmware has the most popular 100+ POWER codes for
 North America and 100+ POWER codes for Europe. You can select which region
 to use by soldering a 10K pulldown resistor.
 */


/*
This project is a good example of how to use the AVR chip timers.
 */

/* This is kind of a strange but very useful helper function
 Because we are using compression, we index to the timer table
 not with a full 8-bit byte (which is wasteful) but 2 or 3 bits.
 Once code_ptr is set up to point to the right part of memory,
 this function will let us read 'count' bits at a time which
 it does by reading a byte into 'bits_r' and then buffering it. */

// we cant read more than 8 bits at a time so dont try!
uint8_t read_bits(uint8_t count)
{
  uint8_t i;
  uint8_t tmp=0;

  // we need to read back count bytes
  for (i=0; i<count; i++) {
    // check if the 8-bit buffer we have has run out
    if (bitsleft_r == 0) {
      // in which case we read a new byte in
      bits_r = powerCode->codes[code_ptr++];
      DEBUGP(putstring("\n\rGet byte: ");
      putnum_uh(bits_r);
      );
      // and reset the buffer size (8 bites in a byte)
      bitsleft_r = 8;
    }
    // remove one bit
    bitsleft_r--;
    // and shift it off of the end of 'bits_r'
    tmp |= (((bits_r >> (bitsleft_r)) & 1) << (count-1-i));
  }
  // return the selected bits in the LSB part of tmp
  return tmp;
}


/* Legacy explanation from old Arduino Code for reference
The C compiler creates code that will transfer all constants into RAM when
 the microcontroller resets.  Since this firmware has a table (powerCodes)
 that is too large to transfer into RAM, the C compiler needs to be told to
 keep it in program memory space.  This is accomplished by the macro
 (this is used in the definition for powerCodes).  Since the C compiler assumes
 that constants are in RAM, rather than in program memory, when accessing
 powerCodes, we need to use the pgm_read_word() and pgm_read_byte macros, and
 we need to use powerCodes as an address.  This is done with PGM_P, defined
 below.
 For example, when we start a new powerCode, we first point to it with the
 following statement:
 PGM_P thecode_p = pgm_read_word(powerCodes+i);
 The next read from the powerCode is a byte that indicates the carrier
 frequency, read as follows:
 const uint8_t freq = pgm_read_byte(code_ptr++);
 After that is a byte that tells us how many 'onTime/offTime' pairs we have:
 const uint8_t numpairs = pgm_read_byte(code_ptr++);
 The next byte tells us the compression method. Since we are going to use a
 timing table to keep track of how to pulse the LED, and the tables are
 pretty short (usually only 4-8 entries), we can index into the table with only
 2 to 4 bits. Once we know the bit-packing-size we can decode the pairs
 const uint8_t bitcompression = pgm_read_byte(code_ptr++);
 Subsequent reads from the powerCode are n bits (same as the packing size)
 that index into another table in ROM that actually stores the on/off times
 const PGM_P time_ptr = (PGM_P)pgm_read_word(code_ptr);
 */

void chooseRegion(int reg) {
region = reg;
  if (reg) {
    DEBUGP(putstring_nl("NA"));
    num_codes=num_NAcodes;
  }
  else {
    num_codes=num_EUcodes;
    DEBUGP(putstring_nl("EU"));
  }
}



void StartTvBGone()   
{
  Serial.begin(115200);

  irsend.begin();

  digitalWrite(LED, LED_ON);
  pinMode(LED, OUTPUT);

  delay_ten_us(5000); //50ms (5000x10 us) delay: let everything settle for a bit

  // determine region
  options = {
      {"Region NA", [=]() { chooseRegion(NA); }},
      {"Region EU", [=]()    { chooseRegion(EU); }},
  };
  delay(200);
  loopOptions(options);
  delay(200);

  // Debug output: indicate how big our database is
  if (region) {
    DEBUGP(putstring("\n\rNA Codesize: ");
    putnum_ud(num_NAcodes);
    );
  } else {
    DEBUGP(putstring("\n\rEU Codesize: ");
    putnum_ud(num_EUcodes);
    );
  }
/*
  // Tell the user what region we're in  - 3 flashes is NA, 6 is EU
  if (region == NA)
    quickflashLEDx(3);
  else //region == EU
    quickflashLEDx(6);
*/    
  sendAllCodes();
}

void sendAllCodes() 
{
  bool endingEarly = false; //will be set to true if the user presses the button during code-sending 
    

  // for every POWER code in our collection
  checkSelPress();
  for (i=0 ; i<num_codes; i++) 
  {

    // print out the code # we are about to transmit
    DEBUGP(putstring("\n\r\n\rCode #: ");
    putnum_ud(i));

    // point to next POWER code, from the right database
    if (region == NA) {
      powerCode = NApowerCodes[i];
    }
    else {
      powerCode = EUpowerCodes[i];
    }
    
    // Read the carrier frequency from the first byte of code structure
    const uint8_t freq = powerCode->timer_val;
    // set OCR for Timer1 to output this POWER code's carrier frequency

    // Print out the frequency of the carrier and the PWM settings
    DEBUGP(putstring("\n\rFrequency: ");
    putnum_ud(freq);
    );
    
    DEBUGP(uint16_t x = (freq+1) * 2;
    putstring("\n\rFreq: ");
    putnum_ud(F_CPU/x);
    );

    // Get the number of pairs, the second byte from the code struct
    const uint8_t numpairs = powerCode->numpairs;
    DEBUGP(putstring("\n\rOn/off pairs: ");
    putnum_ud(numpairs));

    // Get the number of bits we use to index into the timer table
    // This is the third byte of the structure
    const uint8_t bitcompression = powerCode->bitcompression;
    DEBUGP(putstring("\n\rCompression: ");
    putnum_ud(bitcompression);
    putstring("\n\r"));

    // For EACH pair in this code....
    code_ptr = 0;
    for (uint8_t k=0; k<numpairs; k++) {
      uint16_t ti;

      // Read the next 'n' bits as indicated by the compression variable
      // The multiply by 4 because there are 2 timing numbers per pair
      // and each timing number is one word long, so 4 bytes total!
      ti = (read_bits(bitcompression)) * 2;

      // read the onTime and offTime from the program memory
      ontime = powerCode->times[ti];  // read word 1 - ontime
      offtime = powerCode->times[ti+1];  // read word 2 - offtime

      DEBUGP(putstring("\n\rti = ");
      putnum_ud(ti>>1);
      putstring("\tPair = ");
      putnum_ud(ontime));
      DEBUGP(putstring("\t");
      putnum_ud(offtime));      

      rawData[k*2] = ontime * 10;
      rawData[(k*2)+1] = offtime * 10;
      yield();
    }

    progressHandler(i, num_codes);
    // Send Code with library
    irsend.sendRaw(rawData, (numpairs*2) , freq);
    Serial.print("\n");
    yield();
    //Flush remaining bits, so that next code starts
    //with a fresh set of 8 bits.
    bitsleft_r=0;

    // visible indication that a code has been output.
    quickflashLED();
    
    // delay 205 milliseconds before transmitting next POWER code
    delay_ten_us(20500);

    // if user is pushing (holding down) TRIGGER button, stop transmission early 
    if (checkSelPress()) // Pause TV-B-Gone
    {
      while (checkSelPress()){
        yield();
      }
      displayRedStripe("Paused", TFT_WHITE, BGCOLOR);

      while (!checkSelPress()){ // Se apertar Select denovo, continua
        #ifndef CARDPUTER
        if(checkPrevPress()) { // Cancela o TV-B-Gone
            endingEarly = true;
            break;
        }
        #else
        if(Keyboard.isKeyPressed('`')) break;; // apertar ESC no Cardputer // Cancela o TV-B-Gone
        #endif
      }
      while (checkSelPress()){
        yield();
      }
      if (endingEarly) break; // Cancela o TV-B-Gone
      displayRedStripe("Running, Wait", TFT_WHITE, FGCOLOR);
    }
    
  } //end of POWER code for loop


  if (endingEarly==false)
  {
    displayRedStripe("All codes sent!", TFT_WHITE, FGCOLOR);
    //pause for ~1.3 sec, then flash the visible LED 8 times to indicate that we're done
    delay_ten_us(MAX_WAIT_TIME); // wait 655.350ms
    delay_ten_us(MAX_WAIT_TIME); // wait 655.350ms
    //quickflashLEDx(8);
  } else {
    displayRedStripe("User Stoped");
  }

  //turnoff LED
  digitalWrite(LED,LED_OFF);

} //end of sendAllCodes

/****************************** LED AND DELAY FUNCTIONS ********/


// This function delays the specified number of 10 microseconds
// it is 'hardcoded' and is calibrated by adjusting DELAY_CNT
// in main.h Unless you are changing the crystal from 8MHz, dont
// mess with this.
//-due to uint16_t datatype, max delay is 65535 tens of microseconds, or 655350 us, or 655.350 ms. 
//-NB: DELAY_CNT has been increased in main.h from 11 to 25 (last I checked) in order to allow this function
// to work properly with 16MHz Arduinos now (instead of 8MHz).
void delay_ten_us(uint16_t us) {
  uint8_t timer;
  while (us != 0) {
    // for 8MHz we want to delay 80 cycles per 10 microseconds
    // this code is tweaked to give about that amount.
    for (timer=0; timer <= DELAY_CNT; timer++) {
      NOPP;
      NOPP;
    }
    NOPP;
    us--;
  }
}


// This function quickly pulses the visible LED (connected to PB0, pin 5)
// This will indicate to the user that a code is being transmitted
void quickflashLED( void ) {
  digitalWrite(LED, LED_ON);
  delay_ten_us(3000);   // 30 ms ON-time delay
  digitalWrite(LED, LED_OFF);
}

// This function just flashes the visible LED a couple times, used to
// tell the user what region is selected
void quickflashLEDx( uint8_t x ) {
  quickflashLED();
  while(--x) {
    delay_ten_us(25000);     // 250 ms OFF-time delay between flashes
    quickflashLED();
  }
}


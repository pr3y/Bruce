#include "interface.h"
#include "core/powerSave.h"

#include <Keyboard.h>
Keyboard_Class Keyboard;

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    Keyboard.begin();
    pinMode(0, INPUT);
    pinMode(10, INPUT);     // Pin that reads the
}

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <soc/soc_caps.h>
#include <soc/adc_channel.h>
/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    uint8_t percent;
    uint8_t _batAdcCh = ADC1_GPIO10_CHANNEL;
    uint8_t _batAdcUnit = 1;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten((adc1_channel_t)_batAdcCh, ADC_ATTEN_DB_12);
    static esp_adc_cal_characteristics_t* adc_chars = nullptr;
    static constexpr int BASE_VOLATAGE = 3600;
    adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize((adc_unit_t)_batAdcUnit, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, BASE_VOLATAGE, adc_chars);
    int raw;
    raw = adc1_get_raw((adc1_channel_t)_batAdcCh);
    uint32_t volt = esp_adc_cal_raw_to_voltage(raw, adc_chars);

    float mv = volt * 2;
    percent = (mv - 3300) * 100 / (float)(4150 - 3350);

    return  (percent < 0) ? 0
        : (percent >= 100) ? 100
        :  percent;
}


/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if(brightval == 0){
      analogWrite(TFT_BL, brightval);
    } else {
      int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval /100 ));
      analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables checkPrevPress, checkNextPress, checkSelPress, checkAnyKeyPress and checkEscPress
**********************************************************************/
void InputHandler(void) {
    checkPowerSaveTime();
    checkPrevPress    = false;
    checkNextPress    = false;
    checkSelPress     = false;
    checkAnyKeyPress  = false;
    checkEscPress     = false;
    
    Keyboard.update();
    if(Keyboard.isPressed()) {
        if(!wakeUpScreen()) checkAnyKeyPress = true;
        else goto END;
    }
    if(Keyboard.isKeyPressed(',') || Keyboard.isKeyPressed(';')) {
        checkPrevPress = true;
    }
    if(Keyboard.isKeyPressed('`') || Keyboard.isKeyPressed(KEY_BACKSPACE)) {
        checkEscPress = true;
    }
    if(Keyboard.isKeyPressed('/') || Keyboard.isKeyPressed('.')) {
        checkNextPress = true;
    }
    if(Keyboard.isKeyPressed(KEY_ENTER) || digitalRead(0)==LOW) {
        checkSelPress = true;
    }
    END:
    if(checkAnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (Keyboard.isPressed());
    }
}


/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** Starts keyboard to type data
**********************************************************************/
String keyboard(String mytext, int maxSize, String msg) {
    String _mytext = mytext;

    resetTftDisplay();
    bool caps=false;
    int x=0;
    int y=-1;
    int x2=0;
    int y2=0;
    char keys[4][12][2] = { //4 lines, with 12 characteres, low and high caps
        {
            { '1', '!' },//1
            { '2', '@' },//2
            { '3', '#' },//3
            { '4', '$' },//4
            { '5', '%' },//5
            { '6', '^' },//6
            { '7', '&' },//7
            { '8', '*' },//8
            { '9', '(' },//9
            { '0', ')' },//10
            { '-', '_' },//11
            { '=', '+' } //12
        },
        {
            { 'q', 'Q' },//1
            { 'w', 'W' },//2
            { 'e', 'E' },//3
            { 'r', 'R' },//4
            { 't', 'T' },//5
            { 'y', 'Y' },//6
            { 'u', 'U' },//7
            { 'i', 'I' },//8
            { 'o', 'O' },//9
            { 'p', 'P' },//10
            { '[', '{' },//11
            { ']', '}' } //12
        },
        {
            { 'a', 'A' },//1
            { 's', 'S' },//2
            { 'd', 'D' },//3
            { 'f', 'F' },//4
            { 'g', 'G' },//5
            { 'h', 'H' },//6
            { 'j', 'J' },//7
            { 'k', 'K' },//8
            { 'l', 'L' },//9
            { ';', ':' },//10
            { '"', '\'' },//11
            { '|', '\\' } //12
        },
        {
            { '\\', '|' },//1
            { 'z', 'Z' },//2
            { 'x', 'X' },//3
            { 'c', 'C' },//4
            { 'v', 'V' },//5
            { 'b', 'B' },//6
            { 'n', 'N' },//7
            { 'm', 'M' },//8
            { ',', '<' },//9
            { '.', '>' },//10
            { '?', '/' },//11
            { '/', '/' } //12
        }
    };
    int _x = tftWidth/12;
    int _y = (tftHeight - 54)/4;
    int _xo = _x/2-3;

    int i=0;
    int j=-1;
    bool redraw=true;

    int cX =0;
    int cY =0;
    tft.fillScreen(bruceConfig.bgColor);
    while(1) {
        if(redraw) {
            tft.setCursor(0,0);
            tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
            tft.setTextSize(FM);

            //Draw the rectangles
            if(y<0) {
                tft.fillRect(0,1,tftWidth,22,bruceConfig.bgColor);
                tft.drawRect(7,2,46,20,TFT_WHITE);       // Ok Rectangle
                tft.drawRect(55,2,50,20,TFT_WHITE);      // CAP Rectangle
                tft.drawRect(107,2,50,20,TFT_WHITE);     // DEL Rectangle
                tft.drawRect(159,2,74,20,TFT_WHITE);     // SPACE Rectangle
                tft.drawRect(3,32,tftWidth-3,20,bruceConfig.priColor); // mystring Rectangle


                if(x==0 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(7,2,50,20,TFT_WHITE); }
                else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
                tft.drawString("OK", 18, 4);


                if(x==1 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(55,2,50,20,TFT_WHITE); }
                else if(caps) { tft.fillRect(55,2,50,20,TFT_DARKGREY); tft.setTextColor(TFT_WHITE, TFT_DARKGREY); }
                else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
                tft.drawString("CAP", 64, 4);


                if(x==2 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(107,2,50,20,TFT_WHITE); }
                else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
                tft.drawString("DEL", 115, 4);

                if(x>2 && y==-1) { tft.setTextColor(bruceConfig.bgColor, TFT_WHITE); tft.fillRect(159,2,74,20,TFT_WHITE); }
                else tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
                tft.drawString("SPACE", 168, 4);
            }

            tft.setTextSize(FP);
            tft.setTextColor(TFT_WHITE, 0x5AAB);
            tft.drawString(msg.substring(0,38), 3, 24);

            tft.setTextSize(FM);

            // reseta o quadrado do texto
            if (mytext.length() == 19 || mytext.length() == 20 || mytext.length() == 38 || mytext.length() == 39) tft.fillRect(3,32,tftWidth-3,20,bruceConfig.bgColor); // mystring Rectangle
            // escreve o texto
            tft.setTextColor(TFT_WHITE);
            if(mytext.length()>19) {
                tft.setTextSize(FP);
                if(mytext.length()>38) {
                    tft.drawString(mytext.substring(0,38), 5, 34);
                    tft.drawString(mytext.substring(38,mytext.length()), 5, 42);
                }
                else {
                    tft.drawString(mytext, 5, 34);
                }
            } else {
                tft.drawString(mytext, 5, 34);
            }
            //desenha o retangulo colorido
            tft.drawRect(3,32,tftWidth-3,20,bruceConfig.priColor); // mystring Rectangle


            tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
            tft.setTextSize(FM);

            for(i=0;i<4;i++) {
                for(j=0;j<12;j++) {
                    //use last coordenate to paint only this letter
                    if(x2==j && y2==i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); tft.fillRect(j*_x,i*_y+54,_x,_y,bruceConfig.bgColor);}
                    /* If selected, change font color and draw Rectangle*/
                    if(x==j && y==i) { tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor); tft.fillRect(j*_x,i*_y+54,_x,_y,~bruceConfig.bgColor);}

                    /* Print the letters */
                    if(!caps) tft.drawChar(keys[i][j][0], (j*_x+_xo), (i*_y+56));
                    else tft.drawChar(keys[i][j][1], (j*_x+_xo), (i*_y+56));

                    /* Return colors to normal to print the other letters */
                    if(x==j && y==i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
                }
            }
            // save actual key coordenate
            x2=x;
            y2=y;
            redraw = false;
            #if defined(HAS_TOUCH)
            TouchFooter();
            #endif
        }

        //cursor handler
        if(mytext.length()>19) {
            tft.setTextSize(FP);
            if(mytext.length()>38) {
                cY=42;
                cX=5+(mytext.length()-38)*LW;
            }
            else {
                cY=34;
                cX=5+mytext.length()*LW;
            }
        } else {
            cY=34;
            cX=5+mytext.length()*LW*2;
        }

        /* When Select a key in keyboard */
        Keyboard.update();
        if (Keyboard.isPressed()) {
            wakeUpScreen();
            tft.setCursor(cX,cY);
            Keyboard_Class::KeysState status = Keyboard.keysState();

            bool Fn = status.fn;
            if(Fn && Keyboard.isKeyPressed('`')) {
                mytext = _mytext; // return the old name
                returnToMenu=true;// try to stop all the code
                break;
            }

            for (auto i : status.word) {
                if(mytext.length()<maxSize) {
                    mytext += i;
                    if(mytext.length()!=20 && mytext.length()!=20) tft.print(i);
                    cX=tft.getCursorX();
                    cY=tft.getCursorY();
                    if(mytext.length()==20) redraw = true;
                    if(mytext.length()==39) redraw = true;
                }
            }
            if (status.del && mytext.length() > 0) {
                // Handle backspace key
                mytext.remove(mytext.length() - 1);
                int fS=FM;
                if(mytext.length()>19) { tft.setTextSize(FP); fS=FP; }
                else tft.setTextSize(FM);
                tft.setCursor((cX-fS*LW),cY);
                tft.setTextColor(bruceConfig.priColor,bruceConfig.bgColor);
                tft.print(" ");
                tft.setTextColor(TFT_WHITE, 0x5AAB);
                tft.setCursor(cX-fS*LW,cY);
                cX=tft.getCursorX();
                cY=tft.getCursorY();
                if(mytext.length()==19) redraw = true;
                if(mytext.length()==38) redraw = true;
            }
            if (status.enter) {
                break;
            }
            delay(200);
        }

        if(checkSelPress) break;
    }

    //Resets screen when finished writing
    tft.fillRect(0,0,tftWidth,tftHeight,bruceConfig.bgColor);
    resetTftDisplay();

    return mytext;
}



/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() { }


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() { }




/*********************************************************************
** Function: _checkKeyPress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
keyStroke _getKeyPress() {
    keyStroke key;
    Keyboard.update();
    if (Keyboard.isPressed()) {
        wakeUpScreen();
        Keyboard_Class::KeysState status = Keyboard.keysState();
        for (auto i : status.hid_keys) key.hid_keys.push_back(i);
        for (auto i : status.word)  {
            key.word.push_back(i);
            if(i=='`') key.exit_key=true; // key pressed to try to exit
        }
        for (auto i : status.modifier_keys) key.modifier_keys.push_back(i);
        if (status.del)     key.del=true;
        if (status.enter)   key.enter=true;
        if (status.fn)      key.fn=true;
        key.pressed=true;
    } else key.pressed=false;
    return key;
}


/*********************************************************************
** Function: _checkNextPagePress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
bool _checkNextPagePress() {
    Keyboard.update();
    if(Keyboard.isKeyPressed('/')) return true;  // right arrow
    return false;
}

/*********************************************************************
** Function: _checkPrevPagePress
** location: mykeyboard.cpp
** returns the key from the keyboard
**********************************************************************/
bool _checkPrevPagePress() {
    Keyboard.update();
    if(Keyboard.isKeyPressed(',')) return true;  // left arrow
    return false;
}
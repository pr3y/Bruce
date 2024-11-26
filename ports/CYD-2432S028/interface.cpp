#include "interface.h"
#include "core/powerSave.h"

#if defined(HAS_CAPACITIVE_TOUCH)
    #include "CYD28_TouchscreenC.h"
    #define CYD28_DISPLAY_HOR_RES_MAX 240
    #define CYD28_DISPLAY_VER_RES_MAX 320
    CYD28_TouchC touch(CYD28_DISPLAY_HOR_RES_MAX, CYD28_DISPLAY_VER_RES_MAX);
#else
    #include "CYD28_TouchscreenR.h"
    #define CYD28_DISPLAY_HOR_RES_MAX 320
    #define CYD28_DISPLAY_VER_RES_MAX 240  
    CYD28_TouchR touch(CYD28_DISPLAY_HOR_RES_MAX, CYD28_DISPLAY_VER_RES_MAX);
#endif

#define PREV 0
#define SEL 1
#define NEXT 2
#define ALL 3

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() { 
    pinMode(XPT2046_CS, OUTPUT);
    //touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    if(!touch.begin()) {
        Serial.println("Touch IC not Started");
        log_i("Touch IC not Started");
    } else log_i("Touch IC Started");
    digitalWrite(XPT2046_CS, LOW);

}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() { 
    // Brightness control must be initialized after tft in this case @Pirata
    pinMode(TFT_BL,OUTPUT);
    ledcSetup(TFT_BRIGHT_CHANNEL,TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits); //Channel 0, 10khz, 8bits
    ledcAttachPin(TFT_BL, TFT_BRIGHT_CHANNEL);
    ledcWrite(TFT_BRIGHT_CHANNEL,255);
}


/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { }


/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { 
    int dutyCycle;
    if (brightval==100) dutyCycle=255;
    else if (brightval==75) dutyCycle=130;
    else if (brightval==50) dutyCycle=70;
    else if (brightval==25) dutyCycle=20;
    else if (brightval==0) dutyCycle=0;
    else dutyCycle = ((brightval*255)/100);

    log_i("dutyCycle for bright 0-255: %d",dutyCycle);
    ledcWrite(TFT_BRIGHT_CHANNEL,dutyCycle); // Channel 0
}

struct box_t
{
  int x;
  int y;
  int w;
  int h;
  std::uint16_t color;
  int touch_id = -1;
  char key;
  char key_sh;

  void clear(void)
  {
    for (int i = 0; i < 8; ++i)
    {
      tft.fillRect(x, y, w, h,bruceConfig.bgColor);
    }
  }
  void draw(void)
  {
    int ie = touch_id < 0 ? 4 : 8;
    for (int i = 0; i < ie; ++i)
    {
      tft.drawRect(x, y, w, h,color);
      tft.setTextColor(color);
      tft.drawChar(key,x+w/2-FM*LW/2,y+h/2-FM*LH/2);
    }
  }
  bool contain(int x, int y)
  {
    return this->x <= x && x < (this->x + this->w)
        && this->y <= y && y < (this->y + this->h);
  }
};

static constexpr std::size_t box_count = 52;
static box_t box_list[box_count];

bool menuPress(int bot) {
  //0 - prev
  //1 - Sel
  //2 - next
  int terco=WIDTH/3;
  if (touch.touched()) { //touch.tirqTouched() &&
    auto t = touch.getPointScaled();
    t = touch.getPointScaled();
    //log_i("Touchscreen Pressed at x=%d, y=%d, z=%d", t.x,t.y,t.z);
        if(bruceConfig.rotation==3) {
          t.y = (HEIGHT+20)-t.y;
          t.x = WIDTH-t.x;
        }

    if(t.y>(HEIGHT) && ((t.x>terco*bot && t.x<terco*(1+bot)) || bot==ALL)) {
      t.x=WIDTH+1;
      t.y=HEIGHT+11;
      return true;
    } else return false;
  } else return false;

}

/*********************************************************************
** Function: checkNextPress
** location: mykeyboard.cpp
** Verifies Upper Btn to go to previous item
**********************************************************************/
bool checkNextPress(){ 
    if(menuPress(NEXT))     
    {
        if(wakeUpScreen()) {
        delay(200);
        return false;
        }
        return true;
    }

    else return false;
}


/*********************************************************************
** Function: checkPrevPress
** location: mykeyboard.cpp
** Verifies Down Btn to go to next item
**********************************************************************/
bool checkPrevPress() { 
    if(menuPress(PREV)) {
        if(wakeUpScreen()) {
        delay(200);
        return false;
        }
        return true;
    }

  else return false;
}


/*********************************************************************
** Function: checkSelPress
** location: mykeyboard.cpp
** Verifies if Select or OK was pressed
**********************************************************************/
bool checkSelPress() { 
    checkPowerSaveTime();
    if(menuPress(SEL)) {
        if(wakeUpScreen()) {
        delay(200);
        return false;
        }
        return true;
    }

  else return false;
}


/*********************************************************************
** Function: checkEscPress
** location: mykeyboard.cpp
** Verifies if Escape btn was pressed
**********************************************************************/
bool checkEscPress() {
    if(menuPress(PREV))
    {
        if(wakeUpScreen()){
        delay(200);
        return false;
        }
        returnToMenu=true;
        return true;
    }
    else { return false; }
}


/*********************************************************************
** Function: checkAnyKeyPress
** location: mykeyboard.cpp
** Verifies id any of the keys was pressed
**********************************************************************/
bool checkAnyKeyPress() {
    if(menuPress(ALL)) return true;

    return false;
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
    int _x = WIDTH/12;
    int _y = (HEIGHT - 54)/4;
    int _xo = _x/2-3;

    int k=0;
    for(x2=0; x2<12;x2++) {
        for(y2=0; y2<4; y2++) {
        box_list[k].key=keys[y2][x2][0];
        box_list[k].key_sh=keys[y2][x2][1];
        box_list[k].color = ~bruceConfig.bgColor;
        box_list[k].x=x2*_x;
        box_list[k].y=y2*_y+54;
        box_list[k].w=_x;
        box_list[k].h=_y;
        k++;
        }
    }
    // OK
    box_list[k].key=' ';
    box_list[k].key_sh=' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x=0;
    box_list[k].y=0;
    box_list[k].w=53;
    box_list[k].h=22;
    k++;
    // CAP
    box_list[k].key=' ';
    box_list[k].key_sh=' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x=55;
    box_list[k].y=0;
    box_list[k].w=50;
    box_list[k].h=22;
    k++;
    // DEL
    box_list[k].key=' ';
    box_list[k].key_sh=' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x=107;
    box_list[k].y=0;
    box_list[k].w=50;
    box_list[k].h=22;
    k++;
    // SPACE
    box_list[k].key=' ';
    box_list[k].key_sh=' ';
    box_list[k].color = ~bruceConfig.bgColor;
    box_list[k].x=159;
    box_list[k].y=0;
    box_list[k].w=WIDTH-164;
    box_list[k].h=22;

    k=0;
    x2=0;
    y2=0;

    int i=0;
    int j=-1;
    bool redraw=true;
    delay(200);
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
            tft.fillRect(0,1,WIDTH,22,bruceConfig.bgColor);
            tft.drawRect(7,2,46,20,TFT_WHITE);       // Ok Rectangle
            tft.drawRect(55,2,50,20,TFT_WHITE);      // CAP Rectangle
            tft.drawRect(107,2,50,20,TFT_WHITE);     // DEL Rectangle
            tft.drawRect(159,2,74,20,TFT_WHITE);     // SPACE Rectangle
            tft.drawRect(3,32,WIDTH-3,20,bruceConfig.priColor); // mystring Rectangle


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
        if (mytext.length() == 19 || mytext.length() == 20 || mytext.length() == 38 || mytext.length() == 39) tft.fillRect(3,32,WIDTH-3,20,bruceConfig.bgColor); // mystring Rectangle
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
        tft.drawRect(3,32,WIDTH-3,20,bruceConfig.priColor); // mystring Rectangle


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
        TouchFooter();
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

        int z=0;
        if (touch.touched())
       {
        
            auto t = touch.getPointScaled();
            if(bruceConfig.rotation==3) {
            t.y = (HEIGHT+20)-t.y;
            t.x = WIDTH-t.x;
            }
        if (box_list[48].contain(t.x, t.y)) { break; }      // Ok
        if (box_list[49].contain(t.x, t.y)) { caps=!caps; tft.fillRect(0,54,WIDTH,HEIGHT-54,bruceConfig.bgColor); goto THIS_END; } // CAP
        if (box_list[50].contain(t.x, t.y)) goto DEL;               // DEL
        if (box_list[51].contain(t.x, t.y)) { mytext += box_list[51].key; goto ADD; } // SPACE
        for(k=0;k<48;k++){
            if (box_list[k].contain(t.x, t.y)) {
            if(caps) mytext += box_list[k].key_sh;
            else mytext += box_list[k].key;
            }
        }
        wakeUpScreen();
        THIS_END:
        redraw=true;
        delay(200);
        }

        if(checkSelPress())  {
        tft.setCursor(cX,cY);
        if(caps) z=1;
        else z=0;
        if(x==0 && y==-1) break;
        else if(x==1 && y==-1) caps=!caps;
        else if(x==2 && y==-1 && mytext.length() > 0) {
            DEL:
            mytext.remove(mytext.length()-1);
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
        }
        else if(x>2 && y==-1 && mytext.length()<maxSize) mytext += " ";
        else if(y>-1 && mytext.length()<maxSize) {
            ADD:
            mytext += keys[y][x][z];
            if(mytext.length()!=20 && mytext.length()!=20) tft.print(keys[y][x][z]);
            cX=tft.getCursorX();
            cY=tft.getCursorY();
        }
        redraw = true;
        delay(200);
        }

        /* Down Btn to move in X axis (to the right) */
        if(checkNextPress())
        {
        delay(200);
        if(checkNextPress()) { x--; delay(250); } // Long Press
        else x++; // Short Press
        if(y<0 && x>3) x=0;
        if(x>11) x=0;
        else if (x<0) x=11;
        redraw = true;
        }
        /* UP Btn to move in Y axis (Downwards) */
        if(checkPrevPress()) {    
        delay(200);
        if(checkPrevPress()) { y--; delay(250);  }// Long press
        else y++; // short press
      if(y>3) { y=-1; }
      else if(y<-1) y=3;
      redraw = true;
    }

  }

  //Resets screen when finished writing
  tft.fillRect(0,0,WIDTH,HEIGHT,bruceConfig.bgColor);
  resetTftDisplay();

  return mytext;
}


/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() { 
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,LOW); 
    esp_deep_sleep_start();
}


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() { }
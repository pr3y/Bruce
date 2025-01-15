#include "mykeyboard.h"
#include "powerSave.h"
#include "sd_functions.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/rf/rf.h"
#include "modules/others/bad_usb.h"
#include "modules/others/webInterface.h"

#if defined(HAS_TOUCH)
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

#endif


// This will get the value from InputHandler and read add into loopTask, 
// reseting the value after used
keyStroke _getKeyPress() { 
  vTaskSuspend( xHandle );
  keyStroke key=KeyStroke;
  KeyStroke.Clear();
  delay(10);
  vTaskResume( xHandle );
  return key; 
} // must return something that the keyboards won´t recognize by default


/*********************************************************************
** Function: checkNextPagePress
** location: mykeyboard.cpp
** Jumps 5 items from file list
**********************************************************************/
bool checkNextPagePress(){
  if(check(NextPagePress)) return true;
  else return false;
}

/*********************************************************************
** Function: checkPrevPagePress
** location: mykeyboard.cpp
** Jumps -5 items from file list
**********************************************************************/
bool checkPrevPagePress() {
  if(check(PrevPagePress)) return true;
  else return false;
}


/*********************************************************************
** Function: checkShortcutPress
** location: mykeyboard.cpp
** runs a function called by the shortcut action
**********************************************************************/
void checkShortcutPress(){
  // shortctus to quickly starts apps
  keyStroke key = _getKeyPress();
  if(key.pressed) {
    for(auto i: key.word) {
      if(i == 'i')  { otherIRcodes(); returnToMenu=true; }
      if(i == 'r' || i == 's')  { otherRFcodes(); returnToMenu=true; }
      if(i == 'b')  { usb_setup(); returnToMenu=true; }  // badusb
      if(i == 'w')  { loopOptionsWebUi(); returnToMenu=true; }
      if(i == 'f')  { setupSdCard() ? loopSD(SD) : loopSD(LittleFS); returnToMenu=true; }
      if(i == 'l')  { loopSD(LittleFS); returnToMenu=true; }
    }
  }
// TODO: other boards?
// TODO: user-configurable
}

/*********************************************************************
** Function: checkNumberShortcutPress
** location: mykeyboard.cpp
** return the number pressed
**********************************************************************/
int checkNumberShortcutPress() {
    // shortctus to quickly select options
    keyStroke key = _getKeyPress();
    for(auto i: key.word) {
      char c;
      for (c = '1'; c <= '9'; c++) if(i==c) return(c - '1');
    }
    // else
    return -1;
}

/*********************************************************************
** Function: checkLetterShortcutPress
** location: mykeyboard.cpp
** return the letter pressed
**********************************************************************/
char checkLetterShortcutPress() {
  // shortctus to quickly select options
  keyStroke key = _getKeyPress();
  for(auto i: key.word) {
    char c;
    for (c = 'a'; c <= 'z'; c++)
        if(i==c) return(c);
    for (c = 'A'; c <= 'Z'; c++)
        if(i==c) return(c);
  }
  // else
  return -1;
}


/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** keyboard interface.
**********************************************************************/
String keyboard(String mytext, int maxSize, String msg) {
  resetTftDisplay();
  
  String _mytext = mytext;
  bool caps=false;
  bool redraw=true;
  long holdCode=millis(); //to hold the inputs for 250ms before adding other letter
  int cX =0; // Cursor position
  int cY =0; // Cursor position
  int x=0;
  int y=-1; // -1 is where buttons are, out of keys[][][] array
  int z=0;
  int x2=0;
  int y2=0;
  //       [x][y] [z], x2 and y2 are the previous position of x and y, used to redraw only that spot on keyboard screen
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
  const int _x = tftWidth/12;
  const int _y = (tftHeight - 54)/4;
  const int _xo = _x/2-3;

#if defined(HAS_TOUCH)
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
  box_list[k].w=tftWidth-164;
  box_list[k].h=22;

  k=0;
  x2=0;
  y2=0;
#endif

  tft.fillScreen(bruceConfig.bgColor);

#if defined(HAS_3_BUTTONS) // StickCs and Core for long press detection logic
  bool longNextPress = false;
  bool longPrevPress = false;
  long longPressTmp=millis();
#endif  
  while(1) {
    if(redraw) {
      tft.setCursor(0,0);
      tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
      tft.setTextSize(FM);

      //Draw the rectangles
      if(y<0 || y2<0) {
        #if FM>1 // Normal keyboard size
          #define KBLH 20
          int ofs[4][3] = {
            {7  , 46, 18 },
            {55 , 50, 64 },
            {107, 50, 115},
            {159, 74, 168},
          };
        #else // small keyboard size, for small letters (smaller screen, like Marauder Mini and others ;) )
          #define KBLH 10
          int ofs[4][3] = {
            {7 , 20, 10},
            {27, 25, 30},
            {52, 25, 55},
            {77, 50, 80},
          };
        #endif
        tft.fillRect(0,1,tftWidth,22,bruceConfig.bgColor);
        tft.drawRect(ofs[0][0],2,ofs[0][1],KBLH,getComplementaryColor2(bruceConfig.bgColor));       // Ok Rectangle
        tft.drawRect(ofs[1][0],2,ofs[1][1],KBLH,getComplementaryColor2(bruceConfig.bgColor));      // CAP Rectangle
        tft.drawRect(ofs[2][0],2,ofs[2][1],KBLH,getComplementaryColor2(bruceConfig.bgColor));     // DEL Rectangle
        tft.drawRect(ofs[3][0],2,ofs[3][1],KBLH,getComplementaryColor2(bruceConfig.bgColor));     // SPACE Rectangle
        tft.drawRect(3,KBLH+12,tftWidth-3,KBLH,bruceConfig.priColor); // mystring Rectangle


        if(x==0 && y==-1) { tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor)); tft.fillRect(ofs[0][0],2,ofs[0][1],KBLH,getComplementaryColor2(bruceConfig.bgColor)); }
        else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
        tft.drawString("OK", ofs[0][2], 4);


        if(x==1 && y==-1) { tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor)); tft.fillRect(ofs[1][0],2,ofs[1][1],KBLH,getComplementaryColor2(bruceConfig.bgColor)); }
        else if(caps) { tft.fillRect(ofs[1][0],2,ofs[1][1],KBLH,TFT_DARKGREY); tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), TFT_DARKGREY); }
        else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
        tft.drawString("CAP", ofs[1][2], 4);


        if(x==2 && y==-1) { tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor)); tft.fillRect(ofs[2][0],2,ofs[2][1],KBLH,getComplementaryColor2(bruceConfig.bgColor)); }
        else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
        tft.drawString("DEL", ofs[2][2], 4);

        if(x>2 && y==-1) { tft.setTextColor(bruceConfig.bgColor, getComplementaryColor2(bruceConfig.bgColor)); tft.fillRect(ofs[3][0],2,ofs[3][1],KBLH,getComplementaryColor2(bruceConfig.bgColor)); }
        else tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
        tft.drawString("SPACE", ofs[3][2], 4);
      }

      tft.setTextSize(FP);
      tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
      tft.drawString(msg.substring(0,38), 3, KBLH+4);

      tft.setTextSize(FM);

      // reseta o quadrado do texto
      if (mytext.length() == 19 || mytext.length() == 20 || mytext.length() == 38 || mytext.length() == 39) tft.fillRect(3,KBLH+12,tftWidth-3,KBLH,bruceConfig.bgColor); // mystring Rectangle
      // escreve o texto
      tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor));
      if(mytext.length()>19) {
        tft.setTextSize(FP);
        if(mytext.length()>38) {
          tft.drawString(mytext.substring(0,38), 5, KBLH+14);
          tft.drawString(mytext.substring(38,mytext.length()), 5, KBLH+22);
        }
        else {
          tft.drawString(mytext, 5, KBLH+14);
        }
      } else {
        tft.drawString(mytext, 5, KBLH+14);
      }
      //desenha o retangulo colorido
      tft.drawRect(3,32,tftWidth-3,KBLH,bruceConfig.priColor); // mystring Rectangle


      tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), bruceConfig.bgColor);
      tft.setTextSize(FM);


      for(int i=0;i<4;i++) {
        for(int j=0;j<12;j++) {
          //use last coordenate to paint only this letter
          if(x2==j && y2==i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); tft.fillRect(j*_x,i*_y+KBLH*2+14,_x,_y,bruceConfig.bgColor);}
          /* If selected, change font color and draw Rectangle*/
          if(x==j && y==i) { tft.setTextColor(bruceConfig.bgColor, ~bruceConfig.bgColor); tft.fillRect(j*_x,i*_y+KBLH*2+14,_x,_y,~bruceConfig.bgColor);}


          /* Print the letters */
          if(!caps) tft.drawChar(keys[i][j][0], (j*_x+_xo), (i*_y+KBLH*2+16));
          else tft.drawChar(keys[i][j][1], (j*_x+_xo), (i*_y+KBLH*2+16));

          /* Return colors to normal to print the other letters */
          if(x==j && y==i) { tft.setTextColor(~bruceConfig.bgColor, bruceConfig.bgColor); }
        }
      }
      // save actual key coordenate
      x2=x;
      y2=y;
      redraw = false;
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

    if(millis()-holdCode>250) { // allow reading inputs
  
    #if defined(HAS_TOUCH) // CYD, Core2, CoreS3
      auto t = touchPoint;
      if (t.pressed)
      {
        if (box_list[48].contain(t.x, t.y)) { break; }      // Ok
        if (box_list[49].contain(t.x, t.y)) { caps=!caps; tft.fillRect(0,54,tftWidth,tftHeight-54,bruceConfig.bgColor); goto THIS_END; } // CAP
        if (box_list[50].contain(t.x, t.y)) goto DEL;               // DEL
        if (box_list[51].contain(t.x, t.y)) { mytext += box_list[51].key; goto THIS_END; } // SPACE
        for(k=0;k<48;k++){
          if (box_list[k].contain(t.x, t.y)) {
            if(caps) mytext += box_list[k].key_sh;
            else mytext += box_list[k].key;
          }
        }
        wakeUpScreen();
        THIS_END:
        touchPoint.Clear();
        redraw=true;
      }

    #elif defined(HAS_3_BUTTONS) // StickCs and Core
      if(check(SelPress))  {
        goto SELECT;
      }
      /* Down Btn to move in X axis (to the right) */
      if(longNextPress || NextPress) {
        if(!longNextPress) {
          longNextPress = true;
          longPressTmp = millis();
        }
        if(longNextPress && millis()-longPressTmp<200) goto WAITING;
        longNextPress=false;

        if(check(NextPress)) { x--;  /* delay(250); */ } // Long Press
        else x++; // Short Press
        if(y<0 && x>3) x=0;
        if(x>11) x=0;
        else if (x<0) x=11;
        redraw = true;
      }
      /* UP Btn to move in Y axis (Downwards) */
      if(longPrevPress || PrevPress) {
        if(!longPrevPress) {
          longPrevPress = true;
          longPressTmp = millis();
        }
        if(longPrevPress && millis()-longPressTmp<200) goto WAITING;
        longPrevPress=false;

        if(check(PrevPress)) { y--; /* delay(250); */ } // Long press
        else y++; // short press
        if(y>3) { y=-1; }
        else if(y<-1) y=3;
        redraw = true;
      }
    #elif defined (HAS_5_BUTTONS) // Smoochie and Marauder-Mini
      if(check(SelPress))  {
        goto SELECT;
      }
      /* Down Btn to move in X axis (to the right) */
      if(check(NextPress))
      {
        x++;
        if((y<0 && x>3) || x>11) x=0;
        redraw = true;
      }
      if(check(PrevPress))
      {
        x--;
        if(y<0 && x>3) x=3;
        else if(x<0) x=11;
        redraw = true;
      }
      /* UP Btn to move in Y axis (Downwards) */
      if(check(DownPress)) {    
        y++;
        if(y>3) { y=-1; }
        redraw = true;
      }
      if(check(UpPress)) {    
        y--;
        if(y<-1) y=3;
        redraw = true;
      }

    #elif defined (HAS_ENCODER) // T-Embed
      if(check(SelPress))  {
        goto SELECT;
      }
      /* Down Btn to move in X axis (to the right) */
      if(check(NextPress))
      {
        if(check(EscPress)) { y++; }
        else if ((x >= 3 && y < 0) || x == 11) { y++; x = 0; } 
        else x++;

        if (y > 3) y = -1;
        redraw = true;
      }
      /* UP Btn to move in Y axis (Downwards) */
      if(check(PrevPress)) {
        if(check(EscPress)) { y--; }
        else if(x==0) { y--; x--; }
        else x--;

        if(y<-1) { y=3; x=11; }
        else if(y<0 && x<0) x=3;
        else if (x<0) x=11;
        
        redraw = true;
      }

    #elif defined (HAS_KEYBOARD) // Cardputer and T-Deck
        if (KeyStroke.pressed) {
          wakeUpScreen();
          tft.setCursor(cX,cY);
          String keyStr = "";
          for (auto i : KeyStroke.word) {
            if (keyStr != "") {
              keyStr = keyStr + "+" + i;
            } else {
              keyStr += i;
            }
          }

          if(mytext.length()<maxSize && !KeyStroke.enter && !KeyStroke.del) {
            mytext += keyStr;
            if(mytext.length()!=20 && mytext.length()!=20) tft.print(keyStr.c_str());
            cX=tft.getCursorX();
            cY=tft.getCursorY();
            if(mytext.length()==20) redraw = true;
            if(mytext.length()==39) redraw = true;
          }
          if (KeyStroke.del && mytext.length() > 0) { // delete 0x08
            // Handle backspace key
            mytext.remove(mytext.length() - 1);
            int fS=FM;
            if(mytext.length()>19) { tft.setTextSize(FP); fS=FP; }
            else tft.setTextSize(FM);
            tft.setCursor((cX-fS*LW),cY);
            tft.setTextColor(bruceConfig.priColor,bruceConfig.bgColor);
            tft.print(" "); 
            tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
            tft.setCursor(cX-fS*LW,cY);
            cX=tft.getCursorX();
            cY=tft.getCursorY();
            if(mytext.length()==19) redraw = true;
            if(mytext.length()==38) redraw = true;        
          }
          if (KeyStroke.enter) {
            break;
          }
          KeyStroke.Clear();
      }
      if(check(SelPress)) break;

    #endif
    } // end of holdCode detection

    if(false) { // When selecting some letter or something, use these goto addresses(ADD, DEL)
      SELECT:
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
          tft.setTextColor(getComplementaryColor2(bruceConfig.bgColor), 0x5AAB);
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
        holdCode=millis();
    }
    WAITING: // Used in long press detection
    yield();

  }

  //Resets screen when finished writing
  tft.fillRect(0,0,tftWidth,tftHeight,bruceConfig.bgColor);
  resetTftDisplay();

  return mytext;
}

void powerOff() { }

void checkReboot() { }

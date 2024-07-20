#include "mykeyboard.h"


/* Verifies Upper Btn to go to previous item */

bool checkNextPress(){
  #if defined (CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed('/') || Keyboard.isKeyPressed('.'))
  #else    
    if(digitalRead(DW_BTN)==LOW) 
  #endif
  { return true; }

  else return false;
}

/* Verifies Down Btn to go to next item */
bool checkPrevPress() {
  #if defined(STICK_C_PLUS2)
    if(digitalRead(UP_BTN)==LOW) 
  #elif defined(STICK_C_PLUS)
    if(axp192.GetBtnPress())
  #elif defined(CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed(',') || Keyboard.isKeyPressed(';'))
  #endif
  { return true; }

  else return false;
}

/* Verifies if Select or OK was pressed */
bool checkSelPress(){
  #if defined (CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed(KEY_ENTER) || digitalRead(0)==LOW)
  #else
    if(digitalRead(SEL_BTN)==LOW) 
  #endif
  { return true; }

  else return false;
}

bool checkEscPress(){
  #if defined(STICK_C_PLUS2)
    if(digitalRead(UP_BTN)==LOW) 
  #elif defined(STICK_C_PLUS)
    if(axp192.GetBtnPress())
  #elif defined (CARDPUTER)
    Keyboard.update();
    if(Keyboard.isKeyPressed('`'))
  #endif
  { 
     returnToMenu=true;
     return true; 
  }
  else { return false; }
}

#ifndef STICK_C
/* Starts keyboard to type data */
String keyboard(String mytext, int maxSize, String msg) {

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

  int i=0;
  int j=-1;
  bool redraw=true;
  delay(200);
  int cX =0;
  int cY =0;
  tft.fillScreen(BGCOLOR);
  while(1) {
    if(redraw) {
      tft.setCursor(0,0);
      tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.setTextSize(FM);

      //Draw the rectangles
      if(y<0) {
        tft.fillRect(0,1,WIDTH,22,BGCOLOR);
        tft.drawRect(7,2,46,20,TFT_WHITE);       // Ok Rectangle
        tft.drawRect(55,2,50,20,TFT_WHITE);      // CAP Rectangle
        tft.drawRect(107,2,50,20,TFT_WHITE);     // DEL Rectangle
        tft.drawRect(159,2,74,20,TFT_WHITE);     // SPACE Rectangle
        tft.drawRect(3,32,WIDTH-3,20,FGCOLOR); // mystring Rectangle


        if(x==0 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(7,2,50,20,TFT_WHITE); }
        else tft.setTextColor(TFT_WHITE, BGCOLOR);
        tft.drawString("OK", 18, 4);

        
        if(x==1 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(55,2,50,20,TFT_WHITE); }
        else if(caps) { tft.fillRect(55,2,50,20,TFT_DARKGREY); tft.setTextColor(TFT_WHITE, TFT_DARKGREY); }
        else tft.setTextColor(TFT_WHITE, BGCOLOR);
        tft.drawString("CAP", 64, 4);
      

        if(x==2 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(107,2,50,20,TFT_WHITE); }
        else tft.setTextColor(TFT_WHITE, BGCOLOR);
        tft.drawString("DEL", 115, 4);

        if(x>2 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(159,2,74,20,TFT_WHITE); }
        else tft.setTextColor(TFT_WHITE, BGCOLOR);
        tft.drawString("SPACE", 168, 4);
      }

      tft.setTextSize(FP);
      tft.setTextColor(TFT_WHITE, 0x5AAB);
      tft.drawString(msg.substring(0,38), 3, 24);
      
      tft.setTextSize(FM);

      // reseta o quadrado do texto
      if (mytext.length() == 19 || mytext.length() == 20 || mytext.length() == 38 || mytext.length() == 39) tft.fillRect(3,32,WIDTH-3,20,BGCOLOR); // mystring Rectangle
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
      tft.drawRect(3,32,WIDTH-3,20,FGCOLOR); // mystring Rectangle
      

      tft.setTextColor(TFT_WHITE, BGCOLOR);    
      tft.setTextSize(FM);


      for(i=0;i<4;i++) {
        for(j=0;j<12;j++) {
          //use last coordenate to paint only this letter
          if(x2==j && y2==i) { tft.setTextColor(TFT_WHITE, BGCOLOR); tft.fillRect(j*18+11,i*19+54,21,19,BGCOLOR);}
          /* If selected, change font color and draw Rectangle*/
          if(x==j && y==i) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(j*18+11,i*19+54,21,19,TFT_WHITE);}
          
                    
          /* Print the letters */
          if(!caps) tft.drawChar(keys[i][j][0], (j*18+16), (i*19+56));
          else tft.drawChar(keys[i][j][1], (j*18+16), (i*19+56));

          /* Return colors to normal to print the other letters */
          if(x==j && y==i) { tft.setTextColor(TFT_WHITE, BGCOLOR); }
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

    /* When Select a key in keyboard */
    #if defined (CARDPUTER)

    Keyboard.update();
    if (Keyboard.isPressed()) {
      tft.setCursor(cX,cY);
      Keyboard_Class::KeysState status = Keyboard.keysState();
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
        tft.setTextColor(FGCOLOR,BGCOLOR);
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
      delay(150);
    }
    if(checkSelPress()) break;

    #else
    if(checkSelPress())  { 
      tft.setCursor(cX,cY);
      int z=0;
      if(caps) z=1;
      else z=0;
      if(x==0 && y==-1) break;
      else if(x==1 && y==-1) caps=!caps;
      else if(x==2 && y==-1 && mytext.length() > 0) {
        mytext.remove(mytext.length()-1);
        int fS=FM;
        if(mytext.length()>19) { tft.setTextSize(FP); fS=FP; }
        else tft.setTextSize(FM);
        tft.setCursor((cX-fS*LW),cY);
        tft.setTextColor(FGCOLOR,BGCOLOR);
        tft.print(" "); 
        tft.setTextColor(TFT_WHITE, 0x5AAB);
        tft.setCursor(cX-fS*LW,cY);
        cX=tft.getCursorX();
        cY=tft.getCursorY();         
      }
      else if(x>2 && y==-1 && mytext.length()<maxSize) mytext += " ";
      else if(y>-1 && mytext.length()<maxSize) {
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

    #endif

  }
  
  //Resets screen when finished writing
  tft.fillRect(0,0,tft.width(),tft.height(),BGCOLOR);
  resetTftDisplay();

  return mytext;
}

#else

/* Starts keyboard to type data */
String keyboard(String mytext, int maxSize, String msg) {

  resetTftDisplay();
  bool caps=false;
  int x=0;
  int y=-1;
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

  int i=0;
  int j=0;
  delay(200);
  bool redraw = true;
  while(1) {
    if(redraw) {
      tft.setCursor(0,0);
      tft.fillScreen(BGCOLOR);
      tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.setTextSize(FM);

      //Draw the rectangles
      tft.drawRect(7,2,20,10,TFT_WHITE);       // Ok Rectangle
      tft.drawRect(27,2,25,10,TFT_WHITE);      // CAP Rectangle
      tft.drawRect(52,2,25,10,TFT_WHITE);     // DEL Rectangle
      tft.drawRect(77,2,50,10,TFT_WHITE);     // SPACE Rectangle
      tft.drawRect(3,22,WIDTH-3,11,FGCOLOR); // mystring Rectangle


      if(x==0 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(7,2,20,10,TFT_WHITE); }
      else tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.drawString("OK", 10, 4);

      
      if(x==1 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(27,2,25,10,TFT_WHITE); }
      else if(caps) { tft.fillRect(55,2,50,20,TFT_DARKGREY); tft.setTextColor(TFT_WHITE, TFT_DARKGREY); }
      else tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.drawString("CAP", 30, 4);
    

      if(x==2 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(52,2,25,10,TFT_WHITE); }
      else tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.drawString("DEL", 55, 4);

      if(x>2 && y==-1) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(77,2,50,10,TFT_WHITE); }
      else tft.setTextColor(TFT_WHITE, BGCOLOR);
      tft.drawString("SPACE", 80, 4);

      tft.setTextSize(FP);
      tft.setTextColor(TFT_WHITE);
      tft.drawString(msg.substring(0,38), 3, 14);
      
      tft.setTextSize(FM);


      tft.setTextColor(TFT_WHITE, 0x5AAB);    
      if(mytext.length()>19) { 
        tft.setTextSize(FP);
        if(mytext.length()>38) { 
          tft.drawString(mytext.substring(0,38), 5, 24);
          tft.drawString(mytext.substring(38,mytext.length()), 5, 32);
        } 
        else {
          tft.drawString(mytext, 5, 24);
        }
      } else {
        tft.drawString(mytext, 5, 24);
      }
      

      tft.setTextColor(TFT_WHITE, BGCOLOR);    
      tft.setTextSize(FM);


      for(i=0;i<4;i++) {
        for(j=0;j<12;j++) {
          /* If selected, change font color and draw Rectangle*/
          if(x==j && y==i) { tft.setTextColor(BGCOLOR, TFT_WHITE); tft.fillRect(j*11+15,i*9+34,10,10,TFT_WHITE);}
          
          /* Print the letters */
          if(!caps) tft.drawChar(keys[i][j][0], (j*11+18), (i*9+36));
          else tft.drawChar(keys[i][j][1], (j*11+18), (i*9+36));

          /* Return colors to normal to print the other letters */
          if(x==j && y==i) { tft.setTextColor(TFT_WHITE, BGCOLOR); }
        }
      }
      redraw = false;
    }


    /* When Select a key in keyboard */

    if(checkSelPress())  { 
      int z=0;
      if(caps) z=1;
      else z=0;
      if(x==0 && y==-1) break;
      else if(x==1 && y==-1) caps=!caps;
      else if(x==2 && y==-1 && mytext.length() > 0) mytext.remove(mytext.length()-1);
      else if(x>2 && y==-1 && mytext.length()<maxSize) mytext += " ";
      else if(y>-1 && mytext.length()<maxSize) mytext += keys[y][x][z];
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
  tft.fillRect(0,0,tft.width(),tft.height(),BGCOLOR);
  resetTftDisplay();

  return mytext;
}


#endif //If not STICK_C
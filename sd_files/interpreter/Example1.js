var screenWidth = width();
var screenHeight = height();
var battery = getBattery();
var board=getBoard();
var redraw=true;

setTextSize(2);
function updateBtn() {
    while (true) {
        if(getEscPress()) {
            drawFillRect(0, 0, screenWidth, screenHeight, color(255, 0, 0));
            setTextColor(color(0,0,0));
            drawString("Exiting", screenWidth / 2 - 60, screenHeight / 2 - 10);
            delay(3000);
            break; // Exits the game when a prev button is pressed.;
        }
        if(getPrevPress()) {
            setTextColor(color(255,0,0));
            drawFillRect(0, 0, screenWidth, screenHeight, color(0, 0, 0));
            drawString("Prev Btn Pressed ", 10, screenHeight/2-8);
            redraw=true;
        }
        if(getSelPress()) {
            setTextColor(color(0,255,0));
            drawFillRect(0, 0, screenWidth, screenHeight, color(0, 0, 0));
            drawString("Selec Btn Pressed", 10, screenHeight/2-8);
            redraw=true;
        }
        if(getNextPress()) {
            setTextColor(color(0,0,255));
            drawFillRect(0, 0, screenWidth, screenHeight, color(0, 0, 0));
            drawString("Next Btn Pressed ", 10, screenHeight/2-8);
            redraw=true;
        }
        if(redraw==true) {
            battery = getBattery();
            drawString("Battery level: " + battery, 10, 10);
            drawString("Board: " + board, 10, 27);
            drawString("Sel + Next to Exit", 10, screenHeight-22);
            drawRect(5,5,screenWidth-10, screenHeight-10, color(150,20,210));
            delay(200);
            redraw=false;
        }
    }
}

updateBtn();


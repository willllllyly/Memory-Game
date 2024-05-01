#include "mbed.h"
#include "Joystick.h"
#include "N5110.h"
#include <cstdlib>
#include <ctime>

// Functions
void init();
void LoadingScreen();
void HowToPlay();
void MenuScreen();
void ModeEasy();
void ModeMedium();
void ModeHard();
void Button_isr();
void TurnCardDiamonds();
void TurnCardHearts();
void TurnCardSpades();
void TurnCardClubs();
void MenuSelect();
int Randomiser();
volatile int ButtonFlag = 0; //Flag for turning it on and off
int State = 0;

// Objects
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);
DigitalIn JoystickSwitch(PC_12); // Input for joystick switch
Joystick Joystick(PC_1, PC_0);  // Joystick control
InterruptIn Button(PC_10);
DigitalOut GreenLED(PC_3);
DigitalOut RedLED1(PA_13);
DigitalOut RedLED2(PA_14);
DigitalOut RedLED3(PA_15);
PwmOut buzzer(PC_8);

// Sprites
const int Diamonds[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,1,1,1,1,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,1,1,1,1,1,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
const int Hearts[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,0,1,1,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,1,1,1,1,1,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
const int Spades[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,1,1,1,1,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,1,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
const int Clubs[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,1,1,1,1,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,0,1,0,1,0,0,1},
    {1,0,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,0,1},
    {1,0,0,1,0,1,0,1,0,0,1},
    {1,0,0,0,1,1,1,0,0,0,1},
    {1,0,0,1,1,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};

// Select Sprite
const int CardSelect[16][13]={
    {1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1},
};

// Different sprites to animate the turning of the card
const int Back1[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
const int Back2[14][11]={
    {1,1,1,1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,1,1,1,1,1,1},
    {0,0,0,1,1,1,1,1,1,1,1},
};
const int Back3[14][11]={
    {1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,1,1,1,1,1,1},
    {0,0,0,1,1,1,1,1,1,1,1},
    {0,0,0,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,1,1,1,1,1,1},
};
const int Back4[14][11]={
    {1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,1,1,1,1,1,1},
    {0,0,0,1,1,1,1,1,1,1,1},
    {0,0,0,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,1,1,1,1,1},
};
const int Front1[14][11]={
    {1,1,1,1,1,1,1,1,0,0,0},
    {1,0,0,0,0,0,0,0,1,0,0},
    {1,0,0,0,0,0,0,0,0,1,0},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,0,0,0,0,1},
    {0,0,1,0,0,0,0,0,0,0,1},
    {0,0,0,1,1,1,1,1,1,1,1},
};
const int Front2[14][11]={
    {1,1,1,1,1,1,0,0,0,0,0},
    {1,0,0,0,0,1,1,0,0,0,0},
    {1,0,0,0,0,0,1,1,0,0,0},
    {1,0,0,0,0,0,0,1,1,0,0},
    {1,0,0,0,0,0,0,0,1,1,0},
    {1,0,0,0,0,0,0,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,1,1},
    {0,1,1,0,0,0,0,0,0,1,1},
    {0,0,1,1,0,0,0,0,0,1,1},
    {0,0,0,1,1,0,0,0,0,1,1},
    {0,0,0,0,1,1,0,0,0,1,1},
    {0,0,0,0,0,1,1,1,1,1,1},
};
const int Front3[14][11]={
    {1,1,1,1,0,0,0,0,0,0,0},
    {1,0,1,1,1,0,0,0,0,0,0},
    {1,0,0,0,1,1,0,0,0,0,0},
    {1,0,0,0,0,1,1,0,0,0,0},
    {1,0,0,0,0,0,0,1,1,0,0},
    {1,1,0,0,0,0,0,0,1,1,0},
    {0,1,1,0,0,0,0,0,0,1,1},
    {0,1,1,0,0,0,0,0,0,1,1},
    {0,0,1,1,0,0,0,0,0,1,1},
    {0,0,1,1,0,0,0,0,0,1,1},
    {0,0,0,1,1,0,0,0,0,1,1},
    {0,0,0,0,1,1,0,0,0,1,1},
    {0,0,0,0,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,1,1,1,1,1},
};
const int Front4[14][11]={
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,0},
    {0,0,0,0,0,0,0,0,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0},
};

int main() {
    
    Button.rise(&Button_isr);
    GreenLED = State;

    while(1) {
        if (ButtonFlag) {
            ThisThread::sleep_for(250ms);
            ButtonFlag = 0;
            State = !State;
            GreenLED = State;
            init(); // Initialise the device and certain objects
            ModeEasy();
           // LoadingScreen();
           // HowToPlay();
           // MenuSelect();
           // TurnCardDiamonds();           
            //ModeHard();
        }

        sleep();    
    }
}

int Randomiser() {
    srand((int)time(0));
    int r = rand() % 40 + -20;
    return r;
    
}
void Button_isr() {
    ButtonFlag = 1; // Set the flag in isr
}
void init() {
    Joystick.init();
    lcd.init(LPH7366_1);
    lcd.setContrast(0.5);
    lcd.setBrightness(0.5);
    lcd.clear();
}
void LoadingScreen() {
    lcd.drawRect(6,24,72,10,FILL_TRANSPARENT);
    lcd.printString(" GAME LOADING ",0,1);
    lcd.refresh();
    ThisThread::sleep_for(500ms);
    int i;
    int x;
    for (i = 7; i <= 70; i+=9) {
        lcd.drawRect(7,25,i,8,FILL_BLACK);
        lcd.refresh();
        int x = i*20;
        ThisThread::sleep_for(std::chrono::milliseconds(x));
    }
    ThisThread::sleep_for(500ms);

}
void HowToPlay() {
    int i = 5;
    lcd.clear();
    lcd.printString(" HOW TO PLAY",0,i-5);
    lcd.printString("==============",0,i-4);
    lcd.printString("Using the joy ",0,i-3);
    lcd.printString(" stick button:",0,i-2);
    lcd.printString("  select the",0,i-1);
    lcd.printString("matching cards",0,i);
    lcd.refresh();
    ThisThread::sleep_for(1000ms);
    for (i=4; i>=-4; i--) {
        lcd.printString(" HOW TO PLAY",0,i-5);
        lcd.printString("==============",0,i-4);
        lcd.printString("Using the joy ",0,i-3);
        lcd.printString(" stick button:",0,i-2);
        lcd.printString("  select the  ",0,i-1);
        lcd.printString("matching cards",0,i);
        lcd.printString(" of cards to  ",0,i+1);
        lcd.refresh();
        ThisThread::sleep_for(200ms);
        lcd.printString(" win the game.",0,i+2);
        lcd.refresh();
        ThisThread::sleep_for(200ms);
        lcd.printString("You have three",0,i+3);
        lcd.refresh();
        ThisThread::sleep_for(200ms);
        lcd.printString("    lives     ",0,i+4);
        lcd.refresh();
        ThisThread::sleep_for(200ms);
        lcd.printString("              ",0,i+5);
        lcd.refresh();
    }
}
void MenuScreen() {
    lcd.clear();
    lcd.printString("  SELECT YOUR",0,0);
    lcd.printString("   GAME MODE",0,1);
    lcd.printString("==============",0,2);
    lcd.printString(" ----EASY----",0,3);
    lcd.printString(" ---MEDIUM---",0,4);
    lcd.printString(" ----HARD----",0,5);
    lcd.refresh();
}
void ModeEasy() {
    lcd.clear();
    int i;
    int j;
    srand((int)time(0));
    for (j = 9; j <= 65; j+=28) {   
        for (i = 5; i <= 29; i+=24) {
            int RanNum = rand() % 40 + -20; // Generates a new number each time the loop iterates
            if (RanNum > 0) {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Hearts);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Spades);
                }
            } else {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Diamonds);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Clubs);
                }
            }
        }
        lcd.refresh();
        ThisThread::sleep_for(400ms);
    }
    /*lcd.drawSprite(9,5,14,11,(int *)Hearts);
    lcd.drawSprite(9,29,14,11,(int *)Diamonds);
    lcd.drawSprite(37,5,14,11,(int *)Clubs);
    lcd.drawSprite(37,29,14,11,(int *)Hearts);
    lcd.drawSprite(65,5,14,11,(int *)Diamonds);
    lcd.drawSprite(65,29,14,11,(int *)Clubs);
    lcd.refresh();*/
}
void ModeMedium() {
    lcd.clear();
    lcd.drawSprite(5,5,14,11,(int *)Hearts);
    lcd.drawSprite(5,29,14,11,(int *)Diamonds);
    lcd.drawSprite(26,5,14,11,(int *)Clubs);
    lcd.drawSprite(26,29,14,11,(int *)Hearts);
    lcd.drawSprite(47,5,14,11,(int *)Diamonds);
    lcd.drawSprite(47,29,14,11,(int *)Clubs);
    lcd.drawSprite(68,5,14,11,(int *)Diamonds);
    lcd.drawSprite(68,29,14,11,(int *)Clubs);
    lcd.refresh();
}
void ModeHard() {
    lcd.clear();
    lcd.drawSprite(4,5,14,11,(int *)Hearts);
    lcd.drawSprite(4,29,14,11,(int *)Diamonds);
    lcd.drawSprite(20,5,14,11,(int *)Clubs);
    lcd.drawSprite(20,29,14,11,(int *)Hearts);
    lcd.drawSprite(36,5,14,11,(int *)Diamonds);
    lcd.drawSprite(36,29,14,11,(int *)Clubs);
    lcd.drawSprite(52,5,14,11,(int *)Diamonds);
    lcd.drawSprite(52,29,14,11,(int *)Clubs);
    lcd.drawSprite(68,5,14,11,(int *)Diamonds);
    lcd.drawSprite(68,29,14,11,(int *)Clubs);
    lcd.refresh();
}
void TurnCardDiamonds() {
    lcd.drawSprite(13,16,14,11,(int *)Diamonds);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front1);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back1);
    lcd.refresh();    
}
void TurnCardHearts() {
    lcd.drawSprite(13,16,14,11,(int *)Hearts);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front1);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back1);
    lcd.refresh();    
}
void TurnCardSpades() {
    lcd.drawSprite(13,16,14,11,(int *)Spades);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front1);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back1);
    lcd.refresh();    
}
void TurnCardClubs() {
    lcd.drawSprite(13,16,14,11,(int *)Clubs);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front1);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Front4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back4);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back3);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back2);
    lcd.refresh();
    ThisThread::sleep_for(150ms);
    lcd.drawSprite(13,16,14,11,(int *)Back1);
    lcd.refresh();    
}
void MenuSelect() {
    int bank = 3;
    MenuScreen();
    lcd.printChar('>',6,bank);
    lcd.refresh();
    Direction Direc = Joystick.get_direction();
    
    switch (Direc){
        default:{break;} 
        case(N):{
            switch(bank){
                case(3):{bank = 5; break;}
                default:{bank--; break;}
            }
        }
        case(S):{
            switch(bank){
                case(5):{bank = 3; break;}
                default:{bank++; break;}
            }
        }
    }
    lcd.clear();
    MenuScreen();
    lcd.printChar('>',6,bank);    
    lcd.refresh();
    ThisThread::sleep_for(150ms);
}

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
void JoyButton_isr();
void TurnCardDiamonds();
void TurnCardHearts();
void TurnCardSpades();
void TurnCardClubs();
void MenuSelect();
void MenuInput();
void EasyInput();
void MediumInput();
void HardInput();
void EasySelect();
void MediumSelect();
void HardSelect();
volatile int ButtonFlag = 0; // Flag for turning it on and off
volatile int JoystickButtonFlag = 0; // Flag for exiting MenuSelect into game mode 
int State = 0;

// Objects
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);
Joystick Joystick(PC_1, PC_0);  // Joystick control
InterruptIn Button(PC_10);
InterruptIn Joystick_Button(PC_12);
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

// Blink ANimation Sprite
const int CardBlink[18][15]={
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  
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
    Joystick_Button.fall(&JoyButton_isr);
    Joystick_Button.mode(PullUp);

    while(1) {
        if (ButtonFlag) {
            ThisThread::sleep_for(250ms);
            ButtonFlag = 0;
            State = !State;
            GreenLED = State;
            init(); // Initialise the device and certain objects
           // ModeHard();
           // LoadingScreen();
            //HowToPlay();
           // MenuSelect();
           // TurnCardDiamonds();           
            ModeMedium();
        }

        sleep();    
    }
}

void Button_isr() {
    ButtonFlag = 1; // Set the flag in isr
}
void JoyButton_isr() {
    JoystickButtonFlag = 1;
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
    lcd.printString("  ---EASY----",0,3);
    lcd.printString("  --MEDIUM---",0,4);
    lcd.printString("  ---HARD----",0,5);
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
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Hearts);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Spades);
                }
            } else {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Diamonds);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Clubs);
                }
            }
        }
        lcd.refresh();
        ThisThread::sleep_for(250ms);
    }
    ThisThread::sleep_for(2000ms);
    for (j = 9; j <= 65; j+=28) {   
        for (i = 5; i <= 29; i+=24) {    
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Front2);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Front3);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Front4);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Back4);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Back3);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Back2);
            lcd.refresh();
            ThisThread::sleep_for(100ms);
            lcd.drawSprite(j,i,14,11,(int *)Back1);
            lcd.refresh();
        }
    }
    EasySelect();
}
void ModeMedium() {
    lcd.clear();
    int i;
    int j;
    srand((int)time(0));
    for (j = 5; j <= 68; j+=21) {   
        for (i = 5; i <= 29; i+=24) {
            int RanNum = rand() % 40 + -20; // Generates a new number each time the loop iterates
            if (RanNum > 0) {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Hearts);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Spades);
                }
            } else {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Diamonds);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Clubs);
                }
            }
        }
        lcd.refresh();
        ThisThread::sleep_for(350ms);
    }
    ThisThread::sleep_for(2000ms);
    for (j = 5; j <= 68; j+=21) {   
        for (i = 5; i <= 29; i+=24) {    
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Front2);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Front3);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Front4);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Back4);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Back3);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Back2);
            lcd.refresh();
            ThisThread::sleep_for(75ms);
            lcd.drawSprite(j,i,14,11,(int *)Back1);
            lcd.refresh();
        }
    }
    MediumSelect();
}
void ModeHard() {
    lcd.clear();
    int i;
    int j;
    srand((int)time(0));
    for (j = 4; j <= 68; j+=16) {   
        for (i = 5; i <= 29; i+=24) {
            int RanNum = rand() % 40 + -20; // Generates a new number each time the loop iterates
            if (RanNum > 0) {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Hearts);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Spades);
                }
            } else {
                if (RanNum % 2 == 0) {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Diamonds);
                } else {
                    lcd.drawSprite(j,i,14,11,(int *)Front1);
                    lcd.refresh();
                    ThisThread::sleep_for(150ms);
                    lcd.drawSprite(j,i,14,11,(int *)Clubs);
                }
            }
        }
        lcd.refresh();
        ThisThread::sleep_for(500ms);
    }
    ThisThread::sleep_for(2000ms);
    for (j = 4; j <= 68; j+=16) {   
        for (i = 5; i <= 29; i+=24) {    
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Front2);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Front3);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Front4);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Back4);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Back3);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Back2);
            lcd.refresh();
            ThisThread::sleep_for(50ms);
            lcd.drawSprite(j,i,14,11,(int *)Back1);
            lcd.refresh();
        }
    }
    HardSelect();
}
void MenuInput(int* bank) {
    Direction Direc = Joystick.get_direction();

    switch (Direc){ 
        case(N):{
            switch(*bank){
                case(3):{*bank = 5; break;}
                default:{(*bank)--; break;}
            }
            break;
        }
        case(S):{
            switch(*bank){
                case(5):{*bank = 3; break;}
                default:{(*bank)++; break;}
            }
            break;
        }
        default: {
            break;
        }
    }
}
void MenuSelect() {
    MenuScreen();
    lcd.printChar('>', 6, 3);
    lcd.refresh();

    int bank = 3;

    while(1) {
        if(JoystickButtonFlag) {
            ThisThread::sleep_for(250ms);
            JoystickButtonFlag = 0;
            break;
        }
        MenuInput(&bank);
        lcd.clear();
        MenuScreen();
        lcd.printChar('>', 6, bank);
        lcd.refresh();
        ThisThread::sleep_for(150ms);
    }
    switch (bank) {
        case(3):{ModeEasy();break;}
        break;
        case(4):{ModeMedium();break;}
        break;
        case(5):{ModeHard();break;}
        break;
    }
}
void EasyInput(int* E_Sel_x, int*E_Sel_y) {
    Direction Direc = Joystick.get_direction();

    switch (Direc){ 
        case(N):{
            switch(*E_Sel_y){
                case(3):{*E_Sel_y = 27; break;}
                default:{*E_Sel_y -= 24; break;}
            }
            break;
        }
        case(S):{
            switch(*E_Sel_y){
                case(27):{*E_Sel_y = 3; break;}
                default:{*E_Sel_y += 24; break;}
            }
            break;
        }
        case(E):{
            switch(*E_Sel_x){
                case(63):{*E_Sel_x = 7; break;}
                default:{*E_Sel_x += 28; break;}
            }
            break;
        }
        case(W):{
            switch(*E_Sel_x){
                case(7):{*E_Sel_x = 63; break;}
                default:{*E_Sel_x -= 28; break;}
            }
            break;
        }
        default: {
            break;
        }
    }
}
void EasySelect() {
    lcd.drawRect(7,3,15,18,FILL_TRANSPARENT);
    lcd.refresh();
    ThisThread::sleep_for(450ms);
    lcd.drawSprite(7,3,18,15,(int *)CardBlink);
    lcd.drawSprite(9,5,14,11,(int *)Back1);
    lcd.refresh();
    ThisThread::sleep_for(250ms);
    lcd.drawRect(7,3,15,18,FILL_TRANSPARENT);
    lcd.refresh();

    int E_Sel_x = 7;
    int E_Sel_y = 3;

    while(1) {
        if(JoystickButtonFlag) {
            ThisThread::sleep_for(250ms);
            JoystickButtonFlag = 0;
            break;
        }
        int Old_E_Sel_x = E_Sel_x;
        int Old_E_Sel_y = E_Sel_y;
        int Card_x = E_Sel_x + 2;
        int Card_y = E_Sel_y + 2;
        lcd.drawSprite(Old_E_Sel_x,Old_E_Sel_y,18,15,(int *)CardBlink);
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Back1);
        EasyInput(&E_Sel_x,&E_Sel_y);
        lcd.drawRect(E_Sel_x,E_Sel_y,15,18,FILL_TRANSPARENT);
        lcd.refresh();
        ThisThread::sleep_for(200ms);
    }
}
void MediumInput(int* E_Sel_x, int*E_Sel_y) {
    Direction Direc = Joystick.get_direction();

    switch (Direc){ 
        case(N):{
            switch(*E_Sel_y){
                case(3):{*E_Sel_y = 27; break;}
                default:{*E_Sel_y -= 24; break;}
            }
            break;
        }
        case(S):{
            switch(*E_Sel_y){
                case(27):{*E_Sel_y = 3; break;}
                default:{*E_Sel_y += 24; break;}
            }
            break;
        }
        case(E):{
            switch(*E_Sel_x){
                case(66):{*E_Sel_x = 3; break;}
                default:{*E_Sel_x += 21; break;}
            }
            break;
        }
        case(W):{
            switch(*E_Sel_x){
                case(3):{*E_Sel_x = 66; break;}
                default:{*E_Sel_x -= 21; break;}
            }
            break;
        }
        default: {
            break;
        }
    }
}
void MediumSelect() {
    lcd.drawRect(3,3,15,18,FILL_TRANSPARENT);
    lcd.refresh();
    ThisThread::sleep_for(450ms);
    lcd.drawSprite(3,3,18,15,(int *)CardBlink);
    lcd.drawSprite(5,5,14,11,(int *)Back1);
    lcd.refresh();
    ThisThread::sleep_for(250ms);
    lcd.drawRect(3,3,15,18,FILL_TRANSPARENT);
    lcd.refresh();

    int E_Sel_x = 3;
    int E_Sel_y = 3;

    while(1) {
        if(JoystickButtonFlag) {
            ThisThread::sleep_for(250ms);
            JoystickButtonFlag = 0;
            break;
        }
        int Old_E_Sel_x = E_Sel_x;
        int Old_E_Sel_y = E_Sel_y;
        int Card_x = E_Sel_x + 2;
        int Card_y = E_Sel_y + 2;
        lcd.drawSprite(Old_E_Sel_x,Old_E_Sel_y,18,15,(int *)CardBlink);
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Back1);
        EasyInput(&E_Sel_x,&E_Sel_y);
        lcd.drawRect(E_Sel_x,E_Sel_y,15,18,FILL_TRANSPARENT);
        lcd.refresh();
        ThisThread::sleep_for(200ms);
    }
}
void HardInput(int* E_Sel_x, int*E_Sel_y) {
    Direction Direc = Joystick.get_direction();

    switch (Direc){ 
        case(N):{
            switch(*E_Sel_y){
                case(3):{*E_Sel_y = 27; break;}
                default:{*E_Sel_y -= 24; break;}
            }
            break;
        }
        case(S):{
            switch(*E_Sel_y){
                case(27):{*E_Sel_y = 3; break;}
                default:{*E_Sel_y += 24; break;}
            }
            break;
        }
        case(E):{
            switch(*E_Sel_x){
                case(63):{*E_Sel_x = 7; break;}
                default:{*E_Sel_x += 28; break;}
            }
            break;
        }
        case(W):{
            switch(*E_Sel_x){
                case(7):{*E_Sel_x = 63; break;}
                default:{*E_Sel_x -= 28; break;}
            }
            break;
        }
        default: {
            break;
        }
    }    
    
}
void HardSelect() {
    lcd.drawRect(2,3,15,18,FILL_TRANSPARENT);
    lcd.refresh();
    ThisThread::sleep_for(450ms);
    lcd.drawSprite(2,3,18,15,(int *)CardBlink);
    lcd.drawSprite(4,5,14,11,(int *)Back1);
    lcd.refresh();
    ThisThread::sleep_for(250ms);
    lcd.drawRect(2,3,15,18,FILL_TRANSPARENT);
    lcd.refresh();
}

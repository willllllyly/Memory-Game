#include "mbed.h"
#include "Joystick.h"
#include "N5110.h" 

// Functions
void init();
void LoadingScreen();
void HowToPlay();
void ModeSelect();
void ModeEasy();
void ModeMedium();
void ModeHard();
void Button_isr();
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
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};

const int Hearts[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
const int Spades[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
const int Clubs[14][11]={
    {1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1},
};
int main() {
    
    Button.rise(&Button_isr);
    GreenLED = State;

    while(1) {
        if (ButtonFlag) {
            ButtonFlag = 0;
            State = !State;
            GreenLED = State;
            init(); // Initialise the device and certain objects
            LoadingScreen();
            HowToPlay();
            //ModeSelect();
        }

        sleep();    
    }
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
    lcd.printString("matching pairs",0,i);
    lcd.refresh();
    ThisThread::sleep_for(1000ms);
    for (i=4; i>=-4; i--) {
        lcd.printString(" HOW TO PLAY",0,i-5);
        lcd.printString("==============",0,i-4);
        lcd.printString("Using the joy ",0,i-3);
        lcd.printString(" stick button:",0,i-2);
        lcd.printString("  select the  ",0,i-1);
        lcd.printString("matching pairs",0,i);
        lcd.printString(" of cards to  ",0,i+1);
        lcd.refresh();
        ThisThread::sleep_for(150ms);
        lcd.printString(" win the game.",0,i+2);
        lcd.refresh();
        ThisThread::sleep_for(150ms);
        lcd.printString("You have three",0,i+3);
        lcd.refresh();
        ThisThread::sleep_for(150ms);
        lcd.printString("    lives     ",0,i+4);
        lcd.refresh();
        ThisThread::sleep_for(150ms);
        lcd.printString("              ",0,i+5);
        lcd.refresh();
    }
}

void ModeSelect() {
    lcd.printString("  SELECT YOUR",0,0);
    lcd.printString("   GAME MODE",0,1);
    lcd.printString("==============",0,2);
    lcd.printString(" ----EASY----",0,3);
    lcd.printString(" ---MEDIUM---",0,4);
    lcd.printString(" ----HARD----",0,5);
}



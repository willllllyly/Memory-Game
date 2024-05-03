#include "mbed.h"
#include "Joystick.h"
#include "N5110.h"
#include <cstdlib>
#include <ctime>
#include "Card.h"

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
void TurnCard();
void MenuSelect();
void MenuInput();
void EasyInput();
void MediumInput();
void HardInput();
void EasySelect(Card E_CardArray[6]);
void MediumSelect(Card M_CardArray[8]);
void HardSelect(Card H_CardArray[10]);
void WinScreen();
void LossScreen();
volatile int ButtonFlag = 0; // Flag for turning it on and off
volatile int JoystickButtonFlag = 0; // Flag for exiting MenuSelect into game mode 
int StateG = 0;
int StateR1 = 0;
int StateR2 = 0;
int StateR3 = 0;
volatile int ButtonCount = 0;

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
const int Diamond[14][11]={
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
const int Heart[14][11]={
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
const int Spade[14][11]={
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
const int Club[14][11]={
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
const int Empty[14][11]={
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
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
    GreenLED = StateG;
    RedLED3 = !StateR3;
    Joystick_Button.fall(&JoyButton_isr);
    Joystick_Button.mode(PullUp);

    srand((int)time(0));

    while(1) {
        if (ButtonFlag) {
            ThisThread::sleep_for(250ms);
            ButtonFlag = 0;
            ButtonCount++;
            if (ButtonCount % 2 ==1) {
                StateG = 1;
                StateR1 = 0;
                StateR2 = 0;
                StateR3 = 0;
                RedLED1 = StateR1;
                RedLED2 = StateR2;
                RedLED3 = StateR3;
                GreenLED = StateG;
                init(); // Initialise the device and certain objects
            // ModeHard();
            // LoadingScreen();
                //HowToPlay();
            // MenuSelect();
            // TurnCardDiamonds();           
                ModeHard();
            }
            else {
                StateG = 0;
                GreenLED = StateG;
                StateR2 = 0;
                StateR3 = 0;
                RedLED2 = StateR2;
                RedLED1 = StateR3;
                lcd.clear();
                lcd.refresh();
                sleep(); 
            }
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
void WinScreen() {
    lcd.clear();
    lcd.printString("  WELL DONE",0,1);
    lcd.printString("==============",0,2);
    lcd.printString("   YOU WON",0,3);
    lcd.printString("  THE GAME!!",0,4);
    lcd.refresh();
}
void LossScreen() {
    lcd.clear();
    lcd.printString("  INCORRECT",0,1);
    lcd.printString("  GAME OVER",0,2);
    lcd.printString("==============",0,3);
    lcd.printString("   UNLUCKY",0,4);
    lcd.refresh();    
}
void ModeEasy() {
    lcd.clear();
    Card Card1, Card2, Card3, Card4, Card5, Card6; // Card instances

    Card1.generate_suit();
    Card2.generate_suit();
    Card3.generate_suit();
    int RanNum2 = rand() % 40 + -20;

        if (RanNum2 > 0) {
            if (RanNum2 % 2 == 0) {
                Card6.set_suit(Card1.get_suit());
                Card5.set_suit(Card2.get_suit());
                Card4.set_suit(Card3.get_suit());
            }
            else {
                Card4.set_suit(Card1.get_suit());
                Card6.set_suit(Card2.get_suit());
                Card5.set_suit(Card3.get_suit());
            }
        } 
        else {
            if (RanNum2 % 2 == 0) {
                Card4.set_suit(Card1.get_suit());
                Card5.set_suit(Card2.get_suit());
                Card6.set_suit(Card3.get_suit());
            } 
            else {
                Card5.set_suit(Card1.get_suit());
                Card4.set_suit(Card2.get_suit());
                Card6.set_suit(Card3.get_suit());
            }
        }
    
    Card E_CardArray[6] = {Card1, Card2, Card3, Card4, Card5, Card6}; 
    int i = 5;
    int j = 9;

    for(int k = 0; k < 6; k++){
        Card *temp = &E_CardArray[k];
        //printf("Suit %d\n", temp->get_suit());
        if (temp->get_suit() == Hearts) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Heart);
        }
        else if (temp->get_suit() == Spades) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Spade);
        }
        else if (temp->get_suit() == Diamonds) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Diamond);
        }
        else{
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Club);
        }

        lcd.refresh();

        i += 24;

        if (i > 30) {
            i = 5; j+= 28;
        }
        if (j > 66){
            break;
        }
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
    for (int i = 0; i < 6; ++i) {
        printf("Card %d: %d\n", i, E_CardArray[i].get_suit());
    }
    EasySelect(E_CardArray);
}
void ModeMedium() {
    lcd.clear();
    Card Card1, Card2, Card3, Card4, Card5, Card6, Card7, Card8; // Card instances

    Card1.generate_suit();
    Card2.generate_suit();
    Card3.generate_suit();
    Card4.generate_suit();

    int RanNum2 = rand() % 40 + -20;

        if (RanNum2 > 0) {
            if (RanNum2 % 2 == 0) {
                Card8.set_suit(Card1.get_suit());
                Card6.set_suit(Card4.get_suit());
                Card7.set_suit(Card2.get_suit());
                Card5.set_suit(Card3.get_suit());
            }
            else {
                Card7.set_suit(Card1.get_suit());
                Card8.set_suit(Card4.get_suit());
                Card6.set_suit(Card2.get_suit());
                Card5.set_suit(Card3.get_suit());
            }
        } 
        else {
            if (RanNum2 % 2 == 0) {
                Card5.set_suit(Card1.get_suit());
                Card6.set_suit(Card4.get_suit());
                Card8.set_suit(Card2.get_suit());
                Card7.set_suit(Card3.get_suit());
            } 
            else {
                Card6.set_suit(Card1.get_suit());
                Card7.set_suit(Card4.get_suit());
                Card8.set_suit(Card2.get_suit());
                Card6.set_suit(Card3.get_suit());
            }
        }
    
    Card M_CardArray[8] = {Card1, Card2, Card3, Card4, Card5, Card6, Card7, Card8}; 
    int i = 5;
    int j = 5;

    for(int k = 0; k < 8; k++){
        Card *temp = &M_CardArray[k];
        //printf("Suit %d\n", temp->get_suit());
        if (temp->get_suit() == Hearts) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Heart);
        }
        else if (temp->get_suit() == Spades) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Spade);
        }
        else if (temp->get_suit() == Diamonds) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Diamond);
        }
        else{
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Club);
        }

        lcd.refresh();

        i += 24;

        if (i > 30) {
            i = 5; j+= 21;
        }
        if (j > 69){
            break;
        }
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
    MediumSelect(M_CardArray);
}
void ModeHard() {
    lcd.clear();
    Card Card1, Card2, Card3, Card4, Card5, Card6, Card7, Card8, Card9, Card10; // Card instances

    Card1.generate_suit();
    Card2.generate_suit();
    Card3.generate_suit();
    Card4.generate_suit();
    Card5.generate_suit();

    int RanNum2 = rand() % 40 + -20;

        if (RanNum2 > 0) {
            if (RanNum2 % 2 == 0) {
                Card8.set_suit(Card5.get_suit());
                Card6.set_suit(Card1.get_suit());
                Card7.set_suit(Card2.get_suit());
                Card10.set_suit(Card3.get_suit());
                Card9.set_suit(Card4.get_suit());
            }
            else {
                Card7.set_suit(Card5.get_suit());
                Card8.set_suit(Card1.get_suit());
                Card6.set_suit(Card2.get_suit());
                Card9.set_suit(Card4.get_suit());
                Card10.set_suit(Card3.get_suit());
            }
        } 
        else {
            if (RanNum2 % 2 == 0) {
                Card10.set_suit(Card5.get_suit());
                Card6.set_suit(Card1.get_suit());
                Card8.set_suit(Card2.get_suit());
                Card7.set_suit(Card4.get_suit());
                Card9.set_suit(Card3.get_suit());
            } 
            else {
                Card7.set_suit(Card5.get_suit());
                Card10.set_suit(Card1.get_suit());
                Card8.set_suit(Card2.get_suit());
                Card6.set_suit(Card4.get_suit());
                Card9.set_suit(Card3.get_suit());
            }
        }
    
    Card H_CardArray[10] = {Card1, Card2, Card3, Card4, Card5, Card6, Card7, Card8}; 
    int i = 5;
    int j = 4;

    for(int k = 0; k < 10; k++){
        Card *temp = &H_CardArray[k];
        //printf("Suit %d\n", temp->get_suit());
        if (temp->get_suit() == Hearts) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Heart);
        }
        else if (temp->get_suit() == Spades) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Spade);
        }
        else if (temp->get_suit() == Diamonds) {
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Diamond);
        }
        else{
            lcd.drawSprite(j,i,14,11,(int *)Front1);
            lcd.refresh();
            ThisThread::sleep_for(150ms);
            lcd.drawSprite(j,i,14,11,(int *)Club);
        }

        lcd.refresh();

        i += 24;

        if (i > 30) {
            i = 5; j+= 16;
        }
        if (j > 69){
            break;
        }
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
    HardSelect(H_CardArray);
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
void TurnCard(int Card_x, int Card_y, Suit suit) {
    
    if (suit == Hearts) {
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Front1);
        lcd.refresh();
        ThisThread::sleep_for(150ms);
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Heart);
        lcd.refresh();
    }
    else if (suit == Spades) {
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Front1);
        lcd.refresh();
        ThisThread::sleep_for(150ms);        
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Spade);
        lcd.refresh();
    }
    else if (suit == Diamonds) {
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Front1);
        lcd.refresh();
        ThisThread::sleep_for(150ms);        
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Diamond);
        lcd.refresh();
    }
    else {
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Front1);
        lcd.refresh();
        ThisThread::sleep_for(150ms);        
        lcd.drawSprite(Card_x,Card_y,14,11,(int *)Club);
        lcd.refresh();
    }
}
void EasyInput(int* E_Sel_x, int*E_Sel_y) {
    Direction E_Direc = Joystick.get_direction();

    switch (E_Direc){ 
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
void EasySelect(Card E_CardArray[5]) {
   lcd.drawLine(7,3,21,3,1);
   lcd.refresh();
   ThisThread::sleep_for(450ms);
   lcd.drawLine(7,3,21,3,0);
   lcd.refresh();
   ThisThread::sleep_for(250ms);
   lcd.drawLine(7,3,21,3,1);
   lcd.refresh();

    int E_Sel_x = 7;
    int E_Sel_y = 3;
    int Card_x = 0;
    int Card_y = 0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    int Counter1 = 0;
    int Counter2 = 0;
    int Counter3 = 0;
    int MatchedPairs = 0;
    int Lives = 0;
    bool AllPairsMatched = false;

    Suit Card_one = UNASSIGNED;
    Suit Card_two = UNASSIGNED;

    while(!AllPairsMatched) {
        while(MatchedPairs < 3) {
            while(Counter1 == 0) {    
                int Old_E_Sel_x = E_Sel_x;
                int Old_E_Sel_y = E_Sel_y;
                Card_x = E_Sel_x + 2;
                Card_y = E_Sel_y + 2;
                float SelCard_x = (Card_x / 28.0f) + (19.0/28.0);
                float SelCard_y = Card_y / 29.0f;
                if(JoystickButtonFlag) {
                    ThisThread::sleep_for(250ms);
                    JoystickButtonFlag = 0;
                    Counter1++;
                    x1 = Card_x;
                    y1 = Card_y;
                    if (SelCard_y == 1) {
                        if (SelCard_x == 1) { // Card 2
                            TurnCard(Card_x, Card_y, E_CardArray[1].get_suit());
                            Card_one = E_CardArray[1].get_suit(); 
                            ThisThread::sleep_for(100ms);                    
                        }
                        else if (SelCard_x == 2) { // Card 4
                            TurnCard(Card_x, Card_y, E_CardArray[3].get_suit());
                            Card_one = E_CardArray[3].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else { // Card 6
                            TurnCard(Card_x, Card_y, E_CardArray[5].get_suit());
                            Card_one = E_CardArray[5].get_suit();                           
                            ThisThread::sleep_for(100ms); 
                        }
                    }   
                    else {
                        if (SelCard_x == 1) { // Card 1                        
                            TurnCard(Card_x, Card_y, E_CardArray[0].get_suit());
                            Card_one = E_CardArray[0].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 2) { // Card 3
                            TurnCard(Card_x, Card_y, E_CardArray[2].get_suit());
                            Card_one = E_CardArray[2].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else { // Card 5
                            TurnCard(Card_x, Card_y, E_CardArray[4].get_suit());
                            Card_one = E_CardArray[4].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }        
                 break;
                }
            
                lcd.drawLine(Old_E_Sel_x,Old_E_Sel_y,Old_E_Sel_x + 14,Old_E_Sel_y,0);
                EasyInput(&E_Sel_x,&E_Sel_y);
                lcd.drawLine(E_Sel_x,E_Sel_y,E_Sel_x + 14,E_Sel_y,1);
                lcd.refresh();
                ThisThread::sleep_for(200ms);
            }
            while(Counter1 == 1) {
                int Old_E_Sel_x = E_Sel_x;
                int Old_E_Sel_y = E_Sel_y;
                Card_x = E_Sel_x + 2;
                Card_y = E_Sel_y + 2;
                float SelCard_x = (Card_x / 28.0f) + (19.0/28.0);
                float SelCard_y = Card_y / 29.0f;
                if(JoystickButtonFlag) {
                    ThisThread::sleep_for(250ms);
                    JoystickButtonFlag = 0;
                    x2 = Card_x;
                    y2 = Card_y;
                    if (SelCard_y == 1) {
                        if (SelCard_x == 1) { // Card 2
                            TurnCard(Card_x, Card_y, E_CardArray[1].get_suit());
                            Card_two = E_CardArray[1].get_suit(); 
                            ThisThread::sleep_for(100ms);                    
                        }
                        else if (SelCard_x == 2) { // Card 4
                            TurnCard(Card_x, Card_y, E_CardArray[3].get_suit());
                            Card_two = E_CardArray[3].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else { // Card 6
                            TurnCard(Card_x, Card_y, E_CardArray[5].get_suit());
                            Card_two = E_CardArray[5].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }   
                    else {
                        if (SelCard_x == 1) { // Card 1
                            TurnCard(Card_x, Card_y, E_CardArray[0].get_suit());
                            Card_two = E_CardArray[0].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 2) { // Card 3
                            TurnCard(Card_x, Card_y, E_CardArray[2].get_suit());
                            Card_two = E_CardArray[2].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else { // Card 5
                            TurnCard(Card_x, Card_y, E_CardArray[4].get_suit());
                            Card_two = E_CardArray[4].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }
                    break;
                }
                
                lcd.drawLine(Old_E_Sel_x,Old_E_Sel_y,Old_E_Sel_x + 14,Old_E_Sel_y,0);
                EasyInput(&E_Sel_x,&E_Sel_y);
                lcd.drawLine(E_Sel_x,E_Sel_y,E_Sel_x + 14,E_Sel_y,1);
                lcd.refresh();
                ThisThread::sleep_for(200ms);
 
            }
            
            // Flipped Second card over and stored suit still in while oop as counter 2 isnt 2    
           // printf("Suit for card %d: %d\n", 1, Card_one);
            //printf("Suit for card %d: %d\n", 2, Card_two);
                
            if (Card_two == Card_one) {
                lcd.drawSprite(x1,y1,14,11,(int *)Empty);
                lcd.drawSprite(x2,y2,14,11,(int *)Empty);
                lcd.refresh();
                Counter1 = 0;
                MatchedPairs++;
            }
            else  {
                Lives++;
                RedLED1 = !StateR1;
                RedLED2 = !StateR2;
                RedLED3 = !StateR1;
                MatchedPairs = 3;
            }
        }
        if (Lives == 1) {
            LossScreen();
            AllPairsMatched = true;
        }
        else {
            WinScreen();
            AllPairsMatched = true;    
        }   
    }    
}
void MediumInput(int* M_Sel_x, int*M_Sel_y) {
    Direction M_Direc = Joystick.get_direction();

    switch (M_Direc){ 
        case(N):{
            switch(*M_Sel_y){
                case(3):{*M_Sel_y = 27; break;}
                default:{*M_Sel_y -= 24; break;}
            }
            break;
        }
        case(S):{
            switch(*M_Sel_y){
                case(27):{*M_Sel_y = 3; break;}
                default:{*M_Sel_y += 24; break;}
            }
            break;
        }
        case(E):{
            switch(*M_Sel_x){
                case(66):{*M_Sel_x = 3; break;}
                default:{*M_Sel_x += 21; break;}
            }
            break;
        }
        case(W):{
            switch(*M_Sel_x){
                case(3):{*M_Sel_x = 66; break;}
                default:{*M_Sel_x -= 21; break;}
            }
            break;
        }
        default: {
            break;
        }
    }
}
void MediumSelect(Card M_CardArray[7]) {
    lcd.drawLine(3,3,17,3,1);
    lcd.refresh();
    ThisThread::sleep_for(450ms);
    lcd.drawLine(3,3,17,3,0);
    lcd.refresh();
    ThisThread::sleep_for(250ms);
    lcd.drawLine(3,3,17,3,1);
    lcd.refresh();

    int M_Sel_x = 3;
    int M_Sel_y = 3;
    int Card_x = 0;
    int Card_y = 0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    int Counter1 = 0;
    int Counter2 = 0;
    int Counter3 = 0;
    int MatchedPairs = 0;
    int Lives = 0;
    bool AllPairsMatched = false;

    Suit Card_one = UNASSIGNED;
    Suit Card_two = UNASSIGNED;
    
    while(!AllPairsMatched) {
        while(MatchedPairs < 4) {
            while(Counter1 == 0) {    
                int Old_M_Sel_x = M_Sel_x;
                int Old_M_Sel_y = M_Sel_y;
                Card_x = M_Sel_x + 2;
                Card_y = M_Sel_y + 2;
                float SelCard_x = (Card_x - 5.0) / 21 + 1.0;
                float SelCard_y = Card_y / 29.0f;
                if(JoystickButtonFlag) {
                    ThisThread::sleep_for(250ms);
                    JoystickButtonFlag = 0;
                    Counter1++;
                    x1 = Card_x;
                    y1 = Card_y;
                    if (SelCard_y == 1) {
                        if (SelCard_x == 1) { // Card 2
                            TurnCard(Card_x, Card_y, M_CardArray[1].get_suit());
                            Card_one = M_CardArray[1].get_suit(); 
                            ThisThread::sleep_for(100ms);                    
                        }
                        else if (SelCard_x == 2) { // Card 4
                            TurnCard(Card_x, Card_y, M_CardArray[3].get_suit());
                            Card_one = M_CardArray[3].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 6
                            TurnCard(Card_x, Card_y, M_CardArray[5].get_suit());
                            Card_one = M_CardArray[5].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else { // Card 8
                            TurnCard(Card_x, Card_y, M_CardArray[7].get_suit());
                            Card_one = M_CardArray[7].get_suit();                           
                            ThisThread::sleep_for(100ms); 
                        }
                    }   
                    else {
                        if (SelCard_x == 1) { // Card 1                        
                            TurnCard(Card_x, Card_y, M_CardArray[0].get_suit());
                            Card_one = M_CardArray[0].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 2) { // Card 3
                            TurnCard(Card_x, Card_y, M_CardArray[2].get_suit());
                            Card_one = M_CardArray[2].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 5
                            TurnCard(Card_x, Card_y, M_CardArray[4].get_suit());
                            Card_one = M_CardArray[4].get_suit();
                            ThisThread::sleep_for(100ms);
                        }
                        else { // Card 7
                            TurnCard(Card_x, Card_y, M_CardArray[6].get_suit());
                            Card_one = M_CardArray[6].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }        
                 break;
                }
            
                lcd.drawLine(Old_M_Sel_x,Old_M_Sel_y,Old_M_Sel_x + 14,Old_M_Sel_y,0);
                MediumInput(&M_Sel_x,&M_Sel_y);
                lcd.drawLine(M_Sel_x,M_Sel_y,M_Sel_x + 14,M_Sel_y,1);
                lcd.refresh();
                ThisThread::sleep_for(200ms);
            }
            while(Counter1 == 1) {
                int Old_M_Sel_x = M_Sel_x;
                int Old_M_Sel_y = M_Sel_y;
                Card_x = M_Sel_x + 2;
                Card_y = M_Sel_y + 2;
                float SelCard_x = (Card_x - 5.0) / 21 + 1.0;
                float SelCard_y = Card_y / 29.0f;
                if(JoystickButtonFlag) {
                    ThisThread::sleep_for(250ms);
                    JoystickButtonFlag = 0;
                    x2 = Card_x;
                    y2 = Card_y;
                    if (SelCard_y == 1) {
                        if (SelCard_x == 1) { // Card 2
                            TurnCard(Card_x, Card_y, M_CardArray[1].get_suit());
                            Card_two = M_CardArray[1].get_suit(); 
                            ThisThread::sleep_for(100ms);                    
                        }
                        else if (SelCard_x == 2) { // Card 4
                            TurnCard(Card_x, Card_y, M_CardArray[3].get_suit());
                            Card_two = M_CardArray[3].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 6
                            TurnCard(Card_x, Card_y, M_CardArray[5].get_suit());
                            Card_two = M_CardArray[5].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else { // Card 8
                            TurnCard(Card_x, Card_y, M_CardArray[7].get_suit());
                            Card_two = M_CardArray[7].get_suit();                           
                            ThisThread::sleep_for(100ms); 
                        }
                    }   
                    else {
                        if (SelCard_x == 1) { // Card 1                        
                            TurnCard(Card_x, Card_y, M_CardArray[0].get_suit());
                            Card_two = M_CardArray[0].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 2) { // Card 3
                            TurnCard(Card_x, Card_y, M_CardArray[2].get_suit());
                            Card_two = M_CardArray[2].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 5
                            TurnCard(Card_x, Card_y, M_CardArray[4].get_suit());
                            Card_two = M_CardArray[4].get_suit();
                            ThisThread::sleep_for(100ms);
                        }
                        else { // Card 7
                            TurnCard(Card_x, Card_y, M_CardArray[6].get_suit());
                            Card_two = M_CardArray[6].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }
                    break;
                }
                
                lcd.drawLine(Old_M_Sel_x,Old_M_Sel_y,Old_M_Sel_x + 14,Old_M_Sel_y,0);
                MediumInput(&M_Sel_x,&M_Sel_y);
                lcd.drawLine(M_Sel_x,M_Sel_y,M_Sel_x + 14,M_Sel_y,1);
                lcd.refresh();
                ThisThread::sleep_for(200ms);
 
            }
            
            // Flipped Second card over and stored suit still in while oop as counter 2 isnt 2    
           // printf("Suit for card %d: %d\n", 1, Card_one);
            //printf("Suit for card %d: %d\n", 2, Card_two);
                
            if (Card_two == Card_one) {
                lcd.drawSprite(x1,y1,14,11,(int *)Empty);
                lcd.drawSprite(x2,y2,14,11,(int *)Empty);
                lcd.refresh();
                Counter1 = 0;
                MatchedPairs++;
            }
            else  {
                Lives++;
                RedLED1 = !StateR1;
                RedLED2 = !StateR2;
                RedLED3 = !StateR1;
                MatchedPairs = 4;
            }
        }
        if (Lives == 1) {
            LossScreen();
            AllPairsMatched = true;
        }
        else {
            WinScreen();
            AllPairsMatched = true;    
        }   
    }
}
void HardInput(int* H_Sel_x, int*H_Sel_y) {
    Direction Direc = Joystick.get_direction();

    switch (Direc){ 
        case(N):{
            switch(*H_Sel_y){
                case(3):{*H_Sel_y = 27; break;}
                default:{*H_Sel_y -= 24; break;}
            }
            break;
        }
        case(S):{
            switch(*H_Sel_y){
                case(27):{*H_Sel_y = 3; break;}
                default:{*H_Sel_y += 24; break;}
            }
            break;
        }
        case(E):{
            switch(*H_Sel_x){
                case(66):{*H_Sel_x = 2; break;}
                default:{*H_Sel_x += 16; break;}
            }
            break;
        }
        case(W):{
            switch(*H_Sel_x){
                case(2):{*H_Sel_x = 66; break;}
                default:{*H_Sel_x -= 16; break;}
            }
            break;
        }
        default: {
            break;
        }
    }    
    
}
void HardSelect(Card H_CardArray[9]) {
    lcd.drawLine(2,3,16,3,1);
    lcd.refresh();
    ThisThread::sleep_for(450ms);
    lcd.drawLine(2,3,16,3,0);
    lcd.refresh();
    ThisThread::sleep_for(250ms);
    lcd.drawLine(2,3,16,3,1);
    lcd.refresh();

    int H_Sel_x = 2;
    int H_Sel_y = 3;
    int Card_x = 0;
    int Card_y = 0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    int Counter1 = 0;
    int Counter2 = 0;
    int Counter3 = 0;
    int MatchedPairs = 0;
    int Lives = 0;
    bool AllPairsMatched = false;

    Suit Card_one = UNASSIGNED;
    Suit Card_two = UNASSIGNED;

    while(!AllPairsMatched) {
        while(MatchedPairs < 5) {
            while(Counter1 == 0) {    
                int Old_H_Sel_x = H_Sel_x;
                int Old_H_Sel_y = H_Sel_y;
                Card_x = H_Sel_x + 2;
                Card_y = H_Sel_y + 2;
                float SelCard_x = (Card_x - 4.0) / 16 + 1.0;
                float SelCard_y = Card_y / 29.0f;
                if(JoystickButtonFlag) {
                    ThisThread::sleep_for(250ms);
                    JoystickButtonFlag = 0;
                    Counter1++;
                    x1 = Card_x;
                    y1 = Card_y;
                    if (SelCard_y == 1) {
                        if (SelCard_x == 1) { // Card 2
                            TurnCard(Card_x, Card_y, H_CardArray[1].get_suit());
                            Card_one = H_CardArray[1].get_suit(); 
                            ThisThread::sleep_for(100ms);                    
                        }
                        else if (SelCard_x == 2) { // Card 4
                            TurnCard(Card_x, Card_y, H_CardArray[3].get_suit());
                            Card_one = H_CardArray[3].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 6
                            TurnCard(Card_x, Card_y, H_CardArray[5].get_suit());
                            Card_one = H_CardArray[5].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 4) { // Card 8
                            TurnCard(Card_x, Card_y, H_CardArray[7].get_suit());
                            Card_one = H_CardArray[7].get_suit();
                            ThisThread::sleep_for(100ms);
                        }                        
                        else { // Card 10
                            TurnCard(Card_x, Card_y, H_CardArray[9].get_suit());
                            Card_one = H_CardArray[9].get_suit();                           
                            ThisThread::sleep_for(100ms); 
                        }
                    }   
                    else {
                        if (SelCard_x == 1) { // Card 1                        
                            TurnCard(Card_x, Card_y, H_CardArray[0].get_suit());
                            Card_one = H_CardArray[0].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 2) { // Card 3
                            TurnCard(Card_x, Card_y, H_CardArray[2].get_suit());
                            Card_one = H_CardArray[2].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 5
                            TurnCard(Card_x, Card_y, H_CardArray[4].get_suit());
                            Card_one = H_CardArray[4].get_suit();
                            ThisThread::sleep_for(100ms);
                        }
                        else if (SelCard_x == 4) { // Card 7
                            TurnCard(Card_x, Card_y, H_CardArray[6].get_suit());
                            Card_one = H_CardArray[6].get_suit();
                            ThisThread::sleep_for(100ms);
                        }                        
                        else { // Card 9
                            TurnCard(Card_x, Card_y, H_CardArray[8].get_suit());
                            Card_one = H_CardArray[8].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }        
                 break;
                }
            
                lcd.drawLine(Old_H_Sel_x,Old_H_Sel_y,Old_H_Sel_x + 14,Old_H_Sel_y,0);
                HardInput(&H_Sel_x,&H_Sel_y);
                lcd.drawLine(H_Sel_x,H_Sel_y,H_Sel_x + 14,H_Sel_y,1);
                lcd.refresh();
                ThisThread::sleep_for(200ms);
            }
            while(Counter1 == 1) {
                int Old_H_Sel_x = H_Sel_x;
                int Old_H_Sel_y = H_Sel_y;
                Card_x = H_Sel_x + 2;
                Card_y = H_Sel_y + 2;
                float SelCard_x = (Card_x - 4.0) / 16 + 1.0;
                float SelCard_y = Card_y / 29.0f;
                if(JoystickButtonFlag) {
                    ThisThread::sleep_for(250ms);
                    JoystickButtonFlag = 0;
                    x2 = Card_x;
                    y2 = Card_y;
                    if (SelCard_y == 1) {
                        if (SelCard_x == 1) { // Card 2
                            TurnCard(Card_x, Card_y, H_CardArray[1].get_suit());
                            Card_two = H_CardArray[1].get_suit(); 
                            ThisThread::sleep_for(100ms);                    
                        }
                        else if (SelCard_x == 2) { // Card 4
                            TurnCard(Card_x, Card_y, H_CardArray[3].get_suit());
                            Card_two = H_CardArray[3].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 6
                            TurnCard(Card_x, Card_y, H_CardArray[5].get_suit());
                            Card_two = H_CardArray[5].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 4) { // Card 8
                            TurnCard(Card_x, Card_y, H_CardArray[7].get_suit());
                            Card_two = H_CardArray[7].get_suit();
                            ThisThread::sleep_for(100ms);
                        }                        
                        else { // Card 10
                            TurnCard(Card_x, Card_y, H_CardArray[9].get_suit());
                            Card_two = H_CardArray[9].get_suit();                           
                            ThisThread::sleep_for(100ms); 
                        }
                    }   
                    else {
                        if (SelCard_x == 1) { // Card 1                        
                            TurnCard(Card_x, Card_y, H_CardArray[0].get_suit());
                            Card_two = H_CardArray[0].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 2) { // Card 3
                            TurnCard(Card_x, Card_y, H_CardArray[2].get_suit());
                            Card_two = H_CardArray[2].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                        else if (SelCard_x == 3) { // Card 5
                            TurnCard(Card_x, Card_y, H_CardArray[4].get_suit());
                            Card_two = H_CardArray[4].get_suit();
                            ThisThread::sleep_for(100ms);
                        }
                        else if (SelCard_x == 4) { // Card 7
                            TurnCard(Card_x, Card_y, H_CardArray[6].get_suit());
                            Card_two = H_CardArray[6].get_suit();
                            ThisThread::sleep_for(100ms);
                        }                        
                        else { // Card 9
                            TurnCard(Card_x, Card_y, H_CardArray[8].get_suit());
                            Card_two = H_CardArray[8].get_suit();
                            ThisThread::sleep_for(100ms); 
                        }
                    }
                    break;
                }
                
                lcd.drawLine(Old_H_Sel_x,Old_H_Sel_y,Old_H_Sel_x + 14,Old_H_Sel_y,0);
                HardInput(&H_Sel_x,&H_Sel_y);
                lcd.drawLine(H_Sel_x,H_Sel_y,H_Sel_x + 14,H_Sel_y,1);
                lcd.refresh();
                ThisThread::sleep_for(200ms);
 
            }
            
            // Flipped Second card over and stored suit still in while oop as counter 2 isnt 2    
           // printf("Suit for card %d: %d\n", 1, Card_one);
            //printf("Suit for card %d: %d\n", 2, Card_two);
                
            if (Card_two == Card_one) {
                lcd.drawSprite(x1,y1,14,11,(int *)Empty);
                lcd.drawSprite(x2,y2,14,11,(int *)Empty);
                lcd.refresh();
                Counter1 = 0;
                MatchedPairs++;
            }
            else  {
                Lives++;
                RedLED1 = !StateR1;
                RedLED2 = !StateR2;
                RedLED3 = !StateR1;
                MatchedPairs = 5;
            }
        }
        if (Lives == 1) {
            LossScreen();
            AllPairsMatched = true;
        }
        else {
            WinScreen();
            AllPairsMatched = true;    
        }   
    }
}

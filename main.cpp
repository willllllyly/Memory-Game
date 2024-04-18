#include "mbed.h"
#include "Joystick.h"
#include "N5110.h" 

// Functions
void init();
void OpenGraphic();
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

int main() {
    
    init(); // Initialise the device and certain objects
    OpenGraphic();
    //HowToPlay();
    //ModeSelect();

}

void init() {
    Joystick.init();
    lcd.init(LPH7366_1);
    lcd.setContrast(0.5);
    lcd.setBrightness(0.5);
    lcd.clear();
}

void OpenGraphic() {

}



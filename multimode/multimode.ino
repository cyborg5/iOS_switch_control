//iOS switch control example
//Multiple mode version
#define MY_DEBUG 1
#include "BluefruitRoutines.h"

//Pin numbers for switches
#define PREVIOUS_SWITCH A0
#define SELECT_SWITCH A1
#define NEXT_SWITCH A2

//Actions
#define DO_PREVIOUS 1
#define DO_SELECT   2
#define DO_NEXT     4

//Flag to tell if you are using switch control or other functions
uint8_t Mode;
#define SWITCH_MODE 1
#define OTHER_MODE 2
#define EXIT_LIMIT 10000ul  //Time limit for switching modes

uint8_t readSwitches(void) {
  return (~(digitalRead(PREVIOUS_SWITCH)*DO_PREVIOUS
      + digitalRead(SELECT_SWITCH)*DO_SELECT
      + digitalRead (NEXT_SWITCH)*DO_NEXT)
     ) & (DO_PREVIOUS+ DO_SELECT+ DO_NEXT);
}

//Translate character to keyboard keycode and transmit
void pressKeyCode (uint8_t c) {
  uint32_t Start=millis();
  ble.print(F("AT+BLEKEYBOARDCODE=00-00-"));
  uint8_t Code=c-'a'+4;
  if (Code<0x10)ble.print("0");
  ble.print(Code,HEX);
  ble.println(F("-00-00-00-00"));
  MESSAGE(F("Pressed."));
  delay(100);//de-bounce
  while (readSwitches()) { //wait for button to be released
    /*do nothing*/
  };
  if( (millis()-Start) > EXIT_LIMIT) {
    Mode= OTHER_MODE;
  }
  ble.println(F("AT+BLEKEYBOARDCODE=00-00"));
  MESSAGE(F("Released"));
}

// This reads the switches and decides what keypresses
// to send to the BLE device.
void doSwitchMode (void) {
  uint8_t i=readSwitches();
  switch (i) {
    case DO_PREVIOUS: pressKeyCode('p'); break;
    case DO_SELECT:   pressKeyCode('s'); break;
    case DO_NEXT:     pressKeyCode('n'); break;
  }
}

// This routine can perform some alternate function other than
// iOS switch control. It will be engaged if you hold a button
// for a very very long time.
void doOtherMode (void) {
  /*Insert your code here*/
  MESSAGE(F("Doing other mode."));
  //For demonstration purposes we will just blink the LED pin 13
  //a few times and then go back into switch control mode.
  pinMode(13, OUTPUT);
  for(uint8_t i=0;i<5;i++ ) {
    digitalWrite(13, HIGH); delay(1000);
    digitalWrite(13, LOW);  delay(1000);
  }
  Mode=SWITCH_MODE;
  MESSAGE(F("Returning to switch mode"));
}

void setup() {
#if(MY_DEBUG)
  while (! Serial) {}; delay (500);
  Serial.begin(9600); Serial.println("Debug output");
#endif
  pinMode(SELECT_SWITCH, INPUT_PULLUP);
  pinMode(NEXT_SWITCH, INPUT_PULLUP);
  pinMode(PREVIOUS_SWITCH, INPUT_PULLUP);
  initializeBluefruit();
  Mode=SWITCH_MODE;
}

void loop() {
  switch(Mode) {
    case SWITCH_MODE: doSwitchMode(); break;
    case OTHER_MODE:  doOtherMode(); break;
  }
}


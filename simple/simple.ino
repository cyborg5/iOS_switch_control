//iOS switch control example
//Simple version
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

uint8_t readSwitches(void) {
  return (~(digitalRead(PREVIOUS_SWITCH)*DO_PREVIOUS
      + digitalRead(SELECT_SWITCH)*DO_SELECT
      + digitalRead (NEXT_SWITCH)*DO_NEXT)
     ) & (DO_PREVIOUS+ DO_SELECT+ DO_NEXT);
}

//Translate character to keyboard keycode and transmit
void pressKeyCode (uint8_t c) {
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
  ble.println(F("AT+BLEKEYBOARDCODE=00-00"));
  MESSAGE(F("Released"));
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
}

void loop() {
  uint8_t i=readSwitches();
  switch (i) {
    case DO_PREVIOUS: pressKeyCode('p'); break;
    case DO_SELECT:   pressKeyCode('s'); break;
    case DO_NEXT:     pressKeyCode('n'); break;
  }
}


//iOS switch control example
//Chord version
#define MY_DEBUG 1
#include "BluefruitRoutines.h"

//Pin numbers for switches
#define PREVIOUS_SWITCH A0
#define SELECT_SWITCH A1
#define NEXT_SWITCH A2

//Actions
#define DO_PREVIOUS 1
#define DO_SELECT 2
#define DO_NEXT   4
#define DO_SPLIT  (DO_PREVIOUS+DO_NEXT)
#define DO_DOWN   (DO_SELECT+DO_NEXT)
#define DO_UP     (DO_PREVIOUS+DO_SELECT)
#define DO_ALL    (DO_PREVIOUS+DO_SELECT+DO_NEXT)
#define DO_LONG   8
#define DO_END    (DO_PREVIOUS+DO_LONG)
#define DO_LONG_SEL (DO_SELECT+DO_LONG)
#define DO_HOME   (DO_NEXT+DO_LONG)
#define DO_RIP    (DO_SPLIT+DO_LONG)
#define DO_PGUP   (DO_UP+DO_LONG)
#define DO_PGDN   (DO_DOWN+DO_LONG)
#define DO_EXIT   (DO_ALL+DO_LONG)

//Flag to tell if you are using switch control or other functions
uint8_t Mode;
uint8_t Prev, Buttons;
#define SWITCH_MODE 1
#define OTHER_MODE 2

uint8_t readSwitches(void) {
  return (~(digitalRead(PREVIOUS_SWITCH)*DO_PREVIOUS
      + digitalRead(SELECT_SWITCH)*DO_SELECT
      + digitalRead (NEXT_SWITCH)*DO_NEXT)
     ) & (DO_PREVIOUS+ DO_SELECT+ DO_NEXT);
}

uint8_t getChord() {
  uint32_t Delta, Start;
  uint8_t Sample;
  Prev=Buttons; //Save previous button state
  Buttons=readSwitches();//Get current button state
  if (Buttons) {
    if(Buttons!=Prev) {//If it's different, restart timer
      Delta=0; Start=millis();
    }
    do {
      delay(50);
      //Take another sample and logical bitwise "or" together
      Buttons |= (Sample = readSwitches());
      Delta=millis()-Start;//Compute difference
    } while (Sample);
    if (Delta > 1000)  Buttons |= 8;//Greater than one second is a long
  } else {
    Prev=0;
  }
  return  Buttons;
};

// This reads the switches and decides what keypresses
// to send to the BLE device.
void doSwitchMode (void) {
  uint8_t i=getChord();
  if (i) {
    ble.print(F("AT+BleKeyboard="));
    switch (i) {
      case DO_PREVIOUS: ble.println("p"); MESSAGE(F("PREV")); break; 
      case DO_SELECT:   ble.println("s"); MESSAGE(F("SELECT")); break; 
      case DO_NEXT:     ble.println("n"); MESSAGE(F("NEXT")); break;
      case DO_SPLIT:    ble.println("v"); MESSAGE(F("SPLIT")); break;  
      case DO_DOWN:     ble.println("d"); MESSAGE(F("DOWN")); break;
      case DO_UP:       ble.println("u"); MESSAGE(F("UP")); break;
      case DO_PGUP:     ble.println("t"); MESSAGE(F("PAGE UP")); break;  
      case DO_LONG_SEL: ble.println("l"); MESSAGE(F("LONG SEL")); break; 
      case DO_PGDN: ble.println("b"); MESSAGE(F("PAGE DOWN")); break;
      case DO_RIP:  ble.println("i"); MESSAGE(F("RIP")); break;
      case DO_END:  ble.println("e"); MESSAGE(F("END")); break;
      case DO_HOME: ble.println("h"); MESSAGE(F("HOME")); break;
      case DO_ALL:    
      case DO_EXIT: ble.println("a"); MESSAGE(F("ALL3")); 
          Mode=OTHER_MODE; delay(500); break;
    }
  }
}

// This routine can perform some alternate function other than
// iOS switch control. It will be engaged if you hold a button
// for a very very long time.
void doOtherMode (void) {
  /*Insert your code here*/
  MESSAGE(F("Doing other mode."));
  //For demonstration purposes we will just blink the LED pin 13
  //until we press the exit code
  digitalWrite(13, HIGH); delay(1000);
  digitalWrite(13, LOW);  delay(1000);
  if(getChord()== DO_ALL) {
    Mode=SWITCH_MODE;
    MESSAGE(F("Returning to switch mode"));  
  }
}

void setup() {
#if(MY_DEBUG)
  while (! Serial) {}; delay (500);
  Serial.begin(9600); Serial.println("Debug output");
#endif
  pinMode(SELECT_SWITCH, INPUT_PULLUP);
  pinMode(NEXT_SWITCH, INPUT_PULLUP);
  pinMode(PREVIOUS_SWITCH, INPUT_PULLUP);
  pinMode(13, OUTPUT);//Blink LED for other mode
  initializeBluefruit();
  Mode=SWITCH_MODE;
  Prev,  Buttons=0;
}

void loop() {
  switch(Mode) {
    case SWITCH_MODE: doSwitchMode(); break;
    case OTHER_MODE:  doOtherMode(); break;
  }
}


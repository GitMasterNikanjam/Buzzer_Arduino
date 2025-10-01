#include "Buzzer.h"

// #################################################################
// Global variables and objects:

BuzzerAlarm buzzer;

// ##################################################################
// Setup:

void setup() {

  Serial.begin(115200);
  
  buzzer.parameters.ACTIVE_MODE = 0;
  buzzer.parameters.PIN_NUM = 10;

  if(!buzzer.init())
  {
    Serial.println(buzzer.errorMessage);
    while(1);
  }

  buzzer.soundInit();
}

// ####################################################################
// Loop:

void loop() {
  // put your main code here, to run repeatedly:
  
}

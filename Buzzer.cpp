
#include "Buzzer.h"

  void BUZZER::StartRecord(void)
  {
    digitalWrite(pin_num,1);
    delay(1000);
    digitalWrite(pin_num,0);
    delay(100);
  }

  // +++++++++++++++++++++++++++++++++++++++++++

  void BUZZER::StopRecord(void)
  {
    for(uint8_t i=1;i<=2;i++)
    {
    digitalWrite(pin_num, 1);
    delay(100);
    digitalWrite(pin_num, 0);
    delay(100);
    }
  }

  // +++++++++++++++++++++++++++++++++++++++++++

  void BUZZER::NoSD(void)
  {
    for(uint8_t i=1;i<=10;i++)
    {
      digitalWrite(pin_num, 1);
      delay(25);
      digitalWrite(pin_num, 0);
      delay(25);  
    }
  }

  // +++++++++++++++++++++++++++++++++++++++++++

  void BUZZER::Warning(void)
  {
    for(int j=1; j<=10; j++)
    {
      for(uint8_t i=1;i<=20;i++)
      {
        digitalWrite(pin_num, 1);
        delay(5);
        digitalWrite(pin_num, 0);
        delay(5);  
      }
      delay(200);
    }
  }
  
  // +++++++++++++++++++++++++++++++++++++++++++
  
  void BUZZER::FinishSetup(void)
  {
    digitalWrite(pin_num, 1);
    delay(100);
    digitalWrite(pin_num, 0);
    delay(100);
  }

  BUZZER::BUZZER(int pin)
  {
    pin_num = pin;
    pinMode(pin_num,OUTPUT);
    digitalWrite(pin_num,0);
  }



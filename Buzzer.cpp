
// ################################################################################
// Include libraries:

#include "Buzzer.h"

// ################################################################################
// BuzzerAlarm class:

Buzzer::Buzzer()
{
  parameters.ACTIVE_MODE = 0;
  parameters.PIN_NUM = -1;
}

Buzzer::~Buzzer()
{
  // Free the GPIO.
  pinMode(parameters.PIN_NUM, INPUT);
}

bool Buzzer::init(void)
{
  if(_checkParameters() == false)
  {
    return false;
  }

  pinMode(parameters.PIN_NUM, OUTPUT);

  off();
}

bool Buzzer::_checkParameters(void)
{
  bool state = (parameters.ACTIVE_MODE <= 1) && (parameters.PIN_NUM >= 0);

  if(state == false)
  {
    errorMessage = "One or some parameters are not correct.";
    return false;
  }

  return true;
}

void Buzzer::on(void)
{
  digitalWrite(parameters.PIN_NUM, parameters.ACTIVE_MODE);
}

void Buzzer::off(void)
{
  digitalWrite(parameters.PIN_NUM, ~parameters.ACTIVE_MODE);
}

void Buzzer::soundInit(void)
{
  on();
  delay(1000);
  off();
  delay(100);
}

void Buzzer::soundStop(void)
{
  for(uint8_t i=1;i<=2;i++)
  {
  on();
  delay(100);
  off();
  delay(100);
  }
}

void Buzzer::soundWarning_1(void)
{
  for(int j=1; j<=10; j++)
  {
    for(uint8_t i=1;i<=20;i++)
    {
      on();
      delay(5);
      off();
      delay(5);  
    }
    delay(200);
  }
}
    
void Buzzer::soundWarning_2(void)
{
  on();
  delay(100);
  off();
  delay(100);
}

void Buzzer::soundWarning_3(void)
{
  for(uint8_t i=1;i<=10;i++)
  {
    on();
    delay(25);
    off();
    delay(25);  
  }
}



#include "BuzzerAlarm.h"

// ################################################################################


BuzzerAlarm::BuzzerAlarm()
{
  parameters.ACTIVE_MODE = 0;
  parameters.PIN_NUM = -1;
}

BuzzerAlarm::~BuzzerAlarm()
{
  // Free the GPIO.
  pinMode(parameters.PIN_NUM, INPUT);
}

bool BuzzerAlarm::init(void)
{
  if(_checkParameters() == false)
  {
    return false;
  }

  pinMode(parameters.PIN_NUM, OUTPUT);

  off();
}

bool BuzzerAlarm::_checkParameters(void)
{
  bool state = (parameters.ACTIVE_MODE <= 1) && (parameters.PIN_NUM >= 0);

  if(state == false)
  {
    errorMessage = "Error BuzzerAlarm: One or some parameters are not correct.";
    return false;
  }

  return true;
}

void BuzzerAlarm::on(void)
{
  digitalWrite(parameters.PIN_NUM, parameters.ACTIVE_MODE);
}

void BuzzerAlarm::off(void)
{
  digitalWrite(parameters.PIN_NUM, ~parameters.ACTIVE_MODE);
}

void BuzzerAlarm::soundInit(void)
{
  on();
  delay(1000);
  off();
  delay(100);
}

void BuzzerAlarm::soundStop(void)
{
  for(uint8_t i=1;i<=2;i++)
  {
  on();
  delay(100);
  off();
  delay(100);
  }
}

void BuzzerAlarm::soundWarning_1(void)
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
    
void BuzzerAlarm::soundWarning_2(void)
{
  on();
  delay(100);
  off();
  delay(100);
}

void BuzzerAlarm::soundWarning_3(void)
{
  for(uint8_t i=1;i<=10;i++)
  {
    on();
    delay(25);
    off();
    delay(25);  
  }
}



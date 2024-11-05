#include "Buzzer.h"

// ################################################################################


Buzzer::Buzzer()
{
  parameters.ACTIVE_MODE = 0;
  parameters.PIN_NUM = -1;
}

Buzzer::~Buzzer()
{
  pinMode(parameters.PIN_NUM, INPUT);
}

bool Buzzer::init(void)
{
  if(_checkParameters() == false)
  {
    return false;
  }

  pinMode(parameters.PIN_NUM, OUTPUT);

  _off();

}

bool Buzzer::_checkParameters(void)
{
  bool state = (parameters.ACTIVE_MODE <= 1) && (parameters.PIN_NUM >= 0);

  if(state == false)
  {
    errorMessage = "Error Buzzer: One or some parameters are not correct.";
    return false;
  }

  return true;
}

void Buzzer::_on(void)
{
  digitalWrite(parameters.PIN_NUM, parameters.ACTIVE_MODE);
}

void Buzzer::_off(void)
{
  digitalWrite(parameters.PIN_NUM, ~parameters.ACTIVE_MODE);
}

void Buzzer::soundInit(void)
{
  _on();
  delay(1000);
  digitalWrite(parameters.PIN_NUM,0);
  delay(100);
}

void Buzzer::soundStop(void)
{
  for(uint8_t i=1;i<=2;i++)
  {
  digitalWrite(parameters.PIN_NUM, 1);
  delay(100);
  digitalWrite(parameters.PIN_NUM, 0);
  delay(100);
  }
}

void Buzzer::soundWarning_1(void)
{
  for(int j=1; j<=10; j++)
  {
    for(uint8_t i=1;i<=20;i++)
    {
      digitalWrite(parameters.PIN_NUM, 1);
      delay(5);
      digitalWrite(parameters.PIN_NUM, 0);
      delay(5);  
    }
    delay(200);
  }
}
    
void Buzzer::soundWarning_2(void)
{
  digitalWrite(parameters.PIN_NUM, 1);
  delay(100);
  digitalWrite(parameters.PIN_NUM, 0);
  delay(100);
}

void Buzzer::soundWarning_3(void)
{
  for(uint8_t i=1;i<=10;i++)
  {
    digitalWrite(parameters.PIN_NUM, 1);
    delay(25);
    digitalWrite(parameters.PIN_NUM, 0);
    delay(25);  
  }
}



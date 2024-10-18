#ifndef _BUZZER_H
#define _BUZZER_H

#include <Arduino.h>

class BUZZER
{
  private:
    int pin_num = 4;
  public:
    BUZZER(int);
    void StartRecord(void);
    void StopRecord(void);
    void NoSD(void);
    void Warning(void);
    void FinishSetup(void);
};

#endif
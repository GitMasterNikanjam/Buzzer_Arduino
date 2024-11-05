#ifndef BUZZER_H
#define BUZZER_H

// ##############################################################
// Include libraries:

#include <Arduino.h>

// ##############################################################
// Buzzer class:

/*
 Buzzer class for manage alaram sounds.
*/
class Buzzer
{
  public:

    String errorMessage;

    struct ParametersStruct
    {
      /// Digital gpio pin number for control buzzer sounds.
      int PIN_NUM;    

      /// Buzzer active mode control. 1: High active, 0: Low active (Default)
      uint8_t ACTIVE_MODE;

    }parameters;

    /*
     Default constructor.
    */
    Buzzer();

    // Default destructor.
    ~Buzzer();

    /*
     Initial object. start digital gpio ouput for buzzer control. check parameters.
     @return true if successed.
    */
    bool init(void);

    // ------------------------------------------------------------------
    // Special sounds for buzzer:

    // Sound for finished initial and configurations at setup section.
    void soundInit(void);

    // Sound for stop something action for operations. 
    void soundStop(void);
    
    // Sound type_1 for warning happening.
    void soundWarning_1(void);

    // Sound type_2 for warning happening.
    void soundWarning_2(void);

    // Sound type_3 for warning happening.
    void soundWarning_3(void);
    
    // -----------------------------------------------------------------

  private:

    // Check parameters validation. Return true if successed.
    bool _checkParameters(void);

    // Buzzer turn on sound.
    void _on(void);

    // Buzzer turn off sound.
    void _off(void);

};

#endif
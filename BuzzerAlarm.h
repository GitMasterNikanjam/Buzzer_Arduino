#ifndef BUZZER_ALRAM_H
#define BUZZER_ALARM_H

// ##############################################################
// Include libraries:

#include <Arduino.h>

// ##############################################################
// Buzzer class:

/**
 @class 
 @brief Buzzer class for manage alaram sounds.
*/
class BuzzerAlarm
{
  public:

    /// @brief Last error accured for object.
    String errorMessage;

    /**
      @struct ParametersStruct
      @brief Structure variable for parameters.
    */
    struct ParametersStruct
    {
      /// @brief Digital GPIO pin number for control buzzer sounds.
      int PIN_NUM;    

      /// @brief Buzzer active mode control. 1: High active, 0: Low active (Default)
      uint8_t ACTIVE_MODE;
    }parameters;

    /**
     @brief Default constructor.
    */
    BuzzerAlarm();

    /// @brief Default destructor.
    ~BuzzerAlarm();

    /**
     @brief Initial object. start digital GPIO ouput mode. Check parameters.
     @return true if successed.
    */
    bool init(void);

    // ------------------------------------------------------------------
    // Special sounds for buzzer:

    /// @brief Alarm for finished initial and configurations something.
    void soundInit(void);

    /// @brief Alarm for stop something action for operations. 
    void soundStop(void);
    
    /// @brief Alarm type_1 for warning happening.
    void soundWarning_1(void);

    /// @brief Alarm type_2 for warning happening.
    void soundWarning_2(void);

    /// @brief Alarm type_3 for warning happening.
    void soundWarning_3(void);
    
    // -----------------------------------------------------------------
    // ON/OFF buzzer sound:

    /// @brief turn on sound.
    void on(void);

    /// @brief turn off sound.
    void off(void);

  private:

    /// @brief Check parameters validation. Return true if successed.
    bool _checkParameters(void);

};

#endif
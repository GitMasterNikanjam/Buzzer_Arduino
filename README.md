# <BuzzerAlarm.h> library for Arduino

- This library can be used to control active buzzer sounds.       
- There are some predefined functions to create sounds with a buzzer.      
- Warning: All alarm sounds are generated in time block mode.     

## Public Member Functions   

 
- ```bool init(void);```--------------------------// Initial object. start digital GPIO ouput mode. Check parameters.   
- ```void soundInit(void);```-------------------// Alarm for finished initial and configurations something.   
- ```void soundStop(void);```-------------------// Alarm for stop something action for operations.     
- ```void soundWarning_1(void);```------------// Alarm type_1 for warning happening.    
- ```void soundWarning_2(void);```------------// Alarm type_2 for warning happening.    
- ```void soundWarning_3(void);```------------// Alarm type_3 for warning happening.   
- ```void on(void);```----------------------------// turn on sound.    
- ```void off(void);```---------------------------// turn off sound.     

## Public Member Variables

- ```String errorMessage;```----------------------------// Last error accured for object.    
- ```struct ParametersStruct parameters;```-------// Structure variable for parameters. parameters include: PIN_NUM, ACTIVE_MODE    
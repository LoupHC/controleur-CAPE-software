
#include "Arduino.h"

#ifndef Defines_h
#define Defines_h


//Debug lines
//#define DEBUG_ROLLUP
//#define DEBUG_ROLLUP1_OVERRIDES
//#define DEBUG_ROLLUP1_TIMING
//#define DEBUG_ROLLUP2_TIMING
//#define DEBUG_ROLLUP1_POSITION
//#define DEBUG_ROLLUP2_POSITION
//#define DEBUG_ROLLUP1_STATE
//#define DEBUG_ROLLUP2_STATE
//#define DEBUG_ROLLUP1_CYCLE
//#define DEBUG_FAN
//#define DEBUG_HEATER
//#define DEBUG_EEPROM
//#define DEBUG_TIMEPOINTS
//#define DEBUG_PROGRAM
//#define DEBUG_SOLARCALC
//#define DEBUG_CLOCK
//#define DEBUG_SETUP

#define TEST_MODE


#define MCP_I2C_OUTPUTS   //Comment if you use Atmegas IOs pins instead of MCP23008

//Don't change these values unless you know what you are doing
#define MAX_ROLLUPS 2
#define MAX_TIMEPOINTS 5
#define MAX_FANS 2
#define MAX_HEATERS 2
#define MAX_STAGES 5
#define MAX_OVERRIDES 5
#define MAX_OVERRIDE_PROGRAMS 9


#define MAX_ALARM_SEQUENCES 3

//EEPROM indexation
#define ROLLUP_INDEX 10
#define FAN_INDEX     101
#define HEATER_INDEX  120
#define TIMEPOINT_INDEX 150
#define OVERRIDES_INDEX 250


//Vocabulary
#define VAR_TEMP 0
#define FIX_TEMP 1
#define MAN_TEMP 2

#define ACT_HIGH 1
#define ACT_LOW 0

#define OFF_VAL 255
#define OFF     255

#define HOUR 2
#define MINUT 1
#define SR 0
#define CLOCK 1
#define SS 2

#define ROUTINE 0

#define FIX 0
#define FLOATING 1
#define BOOLRELATIVE 2
#define FLOATRELATIVE 2

#define ABOVE 0
#define BELOW 1

#define SUN   0
#define CLOUD 1

#ifdef MCP_I2C_OUTPUTS
  #include <Wire.h>
  #include "Adafruit_MCP23008.h"
  extern  Adafruit_MCP23008 mcp;
#endif


#endif
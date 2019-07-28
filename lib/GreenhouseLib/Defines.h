
#include "Arduino.h"

#ifndef Defines_h
#define Defines_h


//#define DEBUG_ROLLUP
//#define DEBUG_ROLLUP1_OVERRIDES
//#define DEBUG_ROLLUP1_TIMING
//#define DEBUG_ROLLUP2_TIMING
//#define DEBUG_ROLLUP3_TIMING
//#define DEBUG_ROLLUP1_POSITION
//#define DEBUG_ROLLUP2_POSITION
//#define DEBUG_ROLLUP1_STATE
//#define DEBUG_ROLLUP2_STATE
//#define DEBUG_ROLLUP1_CYCLE
//#define DEBUG_DEVICE
//#define DEBUG_HEATER
//#define DEBUG_EEPROM
//#define DEBUG_TIMEPOINTS
//#define DEBUG_PROGRAM
//#define DEBUG_SOLARCALC
//#define DEBUG_CLOCK
//#define DEBUG_SETUP
//#define DEBUG_RAMPING

//#define TEST_MODE

//Don't change these values unless you know what you are doing

#ifndef MAX_ROLLUPS
  #define MAX_ROLLUPS 3
#endif

#ifndef MAX_TIMEPOINTS
  #define MAX_TIMEPOINTS 4
#endif

#ifndef MAX_DEVICES
  #define MAX_DEVICES 5
#endif


#ifndef MAX_STAGES
  #define MAX_STAGES 6
#endif

#ifndef MAX_OVERRIDES
  #define MAX_OVERRIDES 6
#endif

#define MAX_CLOCK_OV    3
//humidity
#define MAX_FLOAT_OV    2
//rain
//wind
//outsideTemp
#define MAX_BOOL_OV     2

#define MAX_ALARM_SEQUENCES 3

//EEPROM indexation
#define ROLLUP_INDEX      10
#define DEVICE_INDEX         200
#define TIMEPOINT_INDEX   500


#define GREENHOUSE_INDEX        2000
#define CLOCKOV_INDEX           2100
#define FLOATOV_INDEX           2500
#define BOOLOV_INDEX            3000


#define ROLLUP_INDEX_SIZE       55
#define DEVICE_INDEX_SIZE       12
#define TIMEPOINT_INDEX_SIZE    24

#define CLOCKOV_INDEX_SIZE      10
#define FLOATOV_INDEX_SIZE      15
#define BOOLOV_INDEX_SIZE       8

//Vocabulary
#define VAR_TEMP 0
#define FIX_TEMP 1
#define MAN_TEMP 2

#define ACT_HIGH 1
#define ACT_LOW 0
#define IO           1
#define MCP23008     0

#define OFF_VAL 255
#define AUTO_WEATHER 255
#define OFF 255

#define HOUR 2
#define MINUT 1
#define SR 0
#define CLOCK 1
#define SS 2

#define ROUTINE 0

#define FIX 0
#define FLOATING 1
#define FLOATRELATIVE 2
#define BOOLRELATIVE 3

#define ABOVE 1
#define BELOW 2

#define SUN   100
#define CLOUD 1

#define FANTYPE   0
#define HEATERTYPE 1
#define VALVTYPE 2

#define LOCK 50

#define DS18B20_TEMP 0
#define STH1X_TEMP 1
#define OFF_TEMP 2

#define OFF_RAIN 3
#define RG11_BUCKET 2
#define NO_CONTACT 1
#define NC_CONTACT 0


#endif

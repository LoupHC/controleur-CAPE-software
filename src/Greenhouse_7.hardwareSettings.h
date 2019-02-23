
//************************************************************
//****************HARDWARE ID*********************
//************************************************************
#define DS18B20             1000//Temperature only
#define DHT11               1001//Temperature and humidity
#define DHT12               1002//Temperature and humidity
#define DHT22               1003//Temperature and humidity
#define SHT1X               1004//Temperature and humidity
#define RTC_DS3231          2000//RTC
#define TEST_CLOCKF         2001//RTC
#define TEST_CLOCKFF        2002//RTC
#define HYDREON_RG11        3000//Rain sensor
//************************************************************
//****************INTERFACE ID*********************
//************************************************************
#define MEMORY_SETTINGS     0
#define DEFAULT_SETTINGS    1
#define EXCEL_SETTINGS      2
#define EXCEL_PROGRAMMER_ID 4444
#define EXCEL_DATALOGGER_ID 5555
//************************************************************
//****************HARDWARE SETTINGS*********************
//************************************************************
//*************SENSORS*************M*******
#define GH_TEMPERATURE      SHT1X
#define GH_HUMIDITY         SHT1X
//#define OUT_TEMPERATURE     DS18B20
#define RAIN_SENSOR         HYDREON_RG11
//#define WIND_SPEED
//#define LIGHT
#define RTC                 RTC_DS3231
//*************SENSORS PINOUT*********************
#define SHT1X_DATA          4 //connect this pin to the SHT1X data line
#define SHT1X_CLOCK         5 //connect this pin to the SHT1X clock line
#define DS18B20_DATA        7 //connect this pin to the DS18B20 data line
#define RAIN_SWITCH         3
//*************INTERFACE*********************
#define I2CADDR_LCD         0x27
#define I2CADDR_KEY         0x26
#define KEYPAD_DISPLAY          //Comment to desactivate keypad interface (only main menu)
//#define COMPUTER_INTERFACE    //Uncomment to desactivate interface programming, only parameters from this sheet are kept in memory

//*************DATA IMPORT/EXPORT*********************

//#define UNIT_TEST         //Uncomment to run unit-tests

#define EXCEL_PROGRAMMER

#define EXCEL_DATALOGGER
#define EXPORT_DELAY        10
#define MAX_LINES_PER_FILE  10000
//*************VERSION*********************
#define FIRST_BOOT          109


//************************************************************
//*******************CONTROL PARAMETERS***********************
//************************************************************

//********************GREENHOUSE**************************
#define TIMEPOINTS            4          //# of timepoints
#define ROLLUPS               2          //# of rollups
#define STAGES                4          //# of cool stages (for rollups)
#define FANS                  1          //# of fans
#define HEATERS               1          //# of heaters
//********************PINOUT**************************
#define ROLLUP1_OPENING_PIN   0    //connect this pin to the opening relay (west motor)
#define ROLLUP1_CLOSING_PIN   1    //connect this pin to the closing relay (west motor)
#define ROLLUP2_OPENING_PIN   2    //connect this pin to the opening relay (east motor)
#define ROLLUP2_CLOSING_PIN   3    //connect this pin to the closing relay (east motor)
#define DEVICE1_PIN           4    //connect this pin to the heater relay
#define DEVICE2_PIN           5    //connect this pin to the heater relay
#define ALARM_PIN             7    //Connect this pin to the safety buzzer

//*************COORD GÉOGRAPHIQUES*********************
#define TIMEZONE              -5
#define LATITUDE              46.03
#define LONGITUDE             -73.56

//TIME AND DATE ARE SET AT FIRST UPLOAD ONLY
//For subsequent RTC programming :
//1. Uncomment (by erasing the "//") the line #define RTC_TIME_SET and #define RTC_DATE_SET and upload to set time
//2. Put back the comment markers("//") and upload again
//ALWAYS SET TO WINTER TIME

//#define RTC_TIME_SET
#define HOUR_SET            11
#define MINUT_SET           19
#define SECOND_SET          0

//#define RTC_DATE_SET
#define DAY_SET             26
#define MONTH_SET           10
#define YEAR_SET            2018

#if HARDWARE_VERSION == 2018
  //*************SENSORS*********************
  //Temperature sensor
  #define TEMP_DS18B20
  //Real time clock
  #define CLOCK_DS3231
  //*************SENSORS PINOUT*********************
  #define DS18B20_PIN          7 //connect this pin to the DS18B20 data line
  //*************INTERFACE*********************
  #define I2CADDR_LCD         0x27
  #define I2CADDR_KEY         0x26
  //*************MEMORY MANAGEMENT*********************
  #define FIRST_BOOT 102

#endif

#if HARDWARE_VERSION == CUSTOM
  //*************SENSORS*********************
  //Temperature sensor
  //#define TEMP_DS18B20
  #define TEMP_SHT1X
  #define HUM_SHT1X
  //Real time clock
  #define CLOCK_DS3231
  //*************SENSORS PINOUT*********************
  //#define DS18B20_PIN          7 //connect this pin to the DS18B20 data line
  #define SHT1X_DATA  6
  #define SHT1X_CLOCK 7
  //*************INTERFACE*********************
  #define I2CADDR_LCD         0x27
  #define I2CADDR_KEY         0x26
  //*************MEMORY MANAGEMENT*********************
  #define FIRST_BOOT 103

#endif

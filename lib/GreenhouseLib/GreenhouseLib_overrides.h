
#include "Arduino.h"
#include "EEPROM.h"
#include "elapsedMillis.h"
#include "Parameters.h"
#include "Defines.h"
#include "GreenhouseLib_FF.h"
#include "GreenhouseLib_rollups.h"


#ifndef GreenhouseLib_overrides_h
#define GreenhouseLib_overrides_h





class OverrideR{
  public:
    boolean isActive();
    void activate();
    void desactivate();

    byteParameter ID;
    byteParameter priority;
    boolParameter enabled;
    byteParameter target;
  protected:
    bool _active;
    //bool initialized();
};

class ClockRelativeOverride : public OverrideR {
  public:
    ClockRelativeOverride();
    void checkIfActive(byte hr, byte mn);

    byteParameter hrStart;
    byteParameter mnStart;
    byteParameter hrStop;
    byteParameter mnStop;

  private:

    static unsigned short _EEPROMindex;
    unsigned short _localIndex;

};


class FloatRelativeOverride : public OverrideR {
  public:
    FloatRelativeOverride();
    void checkIfActive(float variable);

    byteParameter comparator;
    floatParameter floatVar;
    floatParameter hyst;

  private:

    static unsigned short _EEPROMindex;
    unsigned short _localIndex;
};

class BoolRelativeOverride : public OverrideR {
  public:
    BoolRelativeOverride();
    void checkIfActive(bool variable);

  private:
    static unsigned short _EEPROMindex;
    unsigned short _localIndex;
};


class OverrideManager{
  public:
    bool isActive(byte ID);
    bool isEnabled(byte ID);
    bool isInitialized(byte ID);
    void enable(byte ID);
    void disable(byte ID);
    void clearOverride(byte ID);
    void clearOverrides();
    //Clock overrides calls
    void initOverride(byte ID, byte priority, byte hrStart, byte mnStart, byte hrStop, byte mnStop, byte target);
    void checkOverride(byte ID, Rollup &rollup, byte hr, byte mn);
    void checkOverride(byte ID, Fan &fan, byte hr, byte mn);
    void checkOverride(byte ID, Heater &heater, byte hr, byte mn);
    //Float override calls
    void initOverride(byte ID, byte priority, byte comparator, float value, float hyst, byte target);
    void checkOverride(byte ID, Rollup &rollup, float variable);
    void checkOverride(byte ID, Fan &fan,float variable);
    void checkOverride(byte ID, Heater &heater,float variable);
    //Bool overrides calls
    void initOverride(byte ID, byte priority, byte target);
    void checkOverride(byte ID, Rollup &rollup, bool variable);
    void checkOverride(byte ID, Fan &fan, bool variable);
    void checkOverride(byte ID, Heater &heater, bool variable);

    byte indexPosition(byte ID);


    ClockRelativeOverride clockOv[MAX_CLOCK_OV];
    FloatRelativeOverride floatOv[MAX_FLOAT_OV];
    BoolRelativeOverride boolOv[MAX_BOOL_OV];

  private:
    bool idIsUnique(byte ID);
    static byte _clockOvCount;
    static byte _floatOvCount;
    static byte _boolOvCount;


};

#endif

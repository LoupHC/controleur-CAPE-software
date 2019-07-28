
#include "Arduino.h"
#include "EEPROM.h"
#include "elapsedMillis.h"
#include "Parameters.h"
#include "Defines.h"

#ifndef GreenhouseLib_overrides_h
#define GreenhouseLib_overrides_h



class Timer{
  public:
    void reset();
    void start(int on, int off);
    void start(int on);
    void loop(int on, int off);
    boolean isOn();

  private:
    byteParameter activeTimer;
    byteParameter inactiveTimer;
    elapsedMillis timer;
    bool lockedAndWaiting;
    int overrideWaitingTime;
};


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
};

class ClockRelativeOverride : public OverrideR {
  public:
    ClockRelativeOverride();
    void checkIfActive(byte hr, byte mn);
    void checkIfActive(byte hr, byte mn, byte condition);

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
    //Clock overrides calls
    void initOverride(byte ID, byte priority, byte hrStart, byte mnStart, byte hrStop, byte mnStop, byte target);
    void checkOverride(byte ID, byte hr, byte mn);

    void checkOverride(ClockRelativeOverride& ov, byte hr, byte mn);
    void checkOverride(ClockRelativeOverride& ov, byte hr, byte mn, byte isAboveTarget);

    byte hourStart(byte ID);
    byte minStart(byte ID);
    byte hourStop(byte ID);
    byte minStop(byte ID);

    //Float override calls
    void initOverride(byte ID, byte priority, byte comparator, float value, float hyst, byte target);
    void checkOverride(byte ID, float variable);
    float floatVar(byte ID);

    //Bool overrides calls
    void initOverride(byte ID, byte priority, byte target);
    void checkOverride(byte ID, bool variable);

    //override calls
    bool isActive(byte ID);
    bool isEnabled(byte ID);
    bool isInitialized(byte ID);
    void enable(byte ID);
    void disable(byte ID);
    void clearOverride(byte ID);
    byte indexPosition(byte ID);
    byte overrideTarget(byte ID);
    void OverridesEEPROMGet();
    void clearOverridesInEEPROM();

    //priority dispatcher
    byte activeOverride();
    byte overrideTarget();

    void setOverride(byte priority, byte target);
    void disableOverride(byte priority);
    void disableOverrides();
    void printOverrides();

    boolean isLock();

    int getFreeRam();

    ClockRelativeOverride& id(byte ID);

    ClockRelativeOverride clockOv[MAX_CLOCK_OV];
    FloatRelativeOverride floatOv[MAX_FLOAT_OV];
    BoolRelativeOverride boolOv[MAX_BOOL_OV];

  protected:

  private:
    bool idIsUnique(byte ID);
    byte _clockOvCount = 0;
    byte _floatOvCount = 0;
    byte _boolOvCount = 0;
};

#endif

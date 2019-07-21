/*
  GreenhouseLib_FF.h
  Copyright (C)2017 Loup Hébert-Chartrand. All right reserved

  You can find the latest version of this code at :
  https://github.com/LoupHC/GreenhouseLib

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "Arduino.h"
#include "elapsedMillis.h"
#include "Parameters.h"
#include "Defines.h"


#ifndef GreenhouseLib_sensors_h
#define GreenhouseLib_sensors_h


class Sensor
{
  public:
    Sensor();
    ~Sensor();
    float value();
    float average24h();
    float min24h();
    float max24h();
    void registerValue(float value);
    void clearRecords();
    boolean allDataAcquired();


  protected:
    float _value;
    float _average24h[24];
    float _average1h[6];
    int _recordCount;
    elapsedMillis timer;
    void recordNewValueInAverage1h();
    void recordNewValueInAverage24h();
    void  updateLastHourAverage();
};

class Temperature : public Sensor{};

class Humidity : public Sensor
{
  public:
    float absolute(float temp);
    float deficit(float temp);
  private:
    float relativeToAbsoluteConversion(float humidity, float temp);
};

class Wind
{  public:
    Wind();
    ~Wind();
    float value();
    void registerValue(float value);
    boolean allDataAcquired();


  protected:
    float _average1min[6];
    elapsedMillis timer;
    void recordNewValueInAverage5min();

};

class Rain
{
  public:
    Rain();
    ~Rain();
    float totalRainfall();
    float rainfall24h();
    boolean isRaining();
    void registerValue(float value);
    boolean allDataAcquired();


  protected:
    float _totalRainfall;
    float _rainfall24h[24];
    elapsedMillis timer;

};


class Current
{
  public:
    Current();
    ~Current();
    float average();
    void registerValue(float value);


  private:
    void getAverage();
    float _currentRead[5];
    float _value;
    byte _recordCount;
    elapsedMillis _timer;
    elapsedMillis _averageTimer;


};

class Lux : public Sensor
{
  public:
    unsigned long wattPerMeterSquare();
    unsigned long Joules24h();
    byte weatherRatio(byte hour, byte min);
  private:
    friend class Timepoint;
};


#endif

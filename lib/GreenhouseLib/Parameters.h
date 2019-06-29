/*
  Parameters.h
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
#include "EEPROM.h"
#include "Defines.h"

#ifndef Parameters_h
#define Parameters_h
//Each greenhouse parameter have 4 associated values, a minimumimum value, a
//maximum value, an actual value and the last value recorded

class Parameter
{
  public:
    void setAddress(int adress);
    int address();

  protected :
    int _address;
};

class floatParameter : public Parameter
{
  public:
    floatParameter();
    ~floatParameter();
    void setValue(float value);
    void setLimits(float minimum, float maximum);
    float minimum();
    float maximum();
    float value();
    boolean isOffLimit();

  private:
    float _minimum;
    float _maximum;
};

class uShortParameter : public Parameter
{
  public:
    uShortParameter();
    ~uShortParameter();
    void setValue(unsigned short value);
    void setLimits(unsigned short minimum, unsigned short maximum);
    unsigned short minimum();
    unsigned short maximum();
    unsigned short value();
    boolean isOffLimit();

  private:
    unsigned short _minimum;
    unsigned short _maximum;
};

class shortParameter : public Parameter
{
  public:
    shortParameter();
    ~shortParameter();
    void setValue(short  value);
    void setLastValue(short value);
    void updateLastValue();
    void setLimits(short minimum, short maximum);
    short minimum();
    short maximum();
    short value();
    boolean isOffLimit();

  private:
    short _minimum;
    short _maximum;
};

class byteParameter : public Parameter
{
  public:
    byteParameter();
    ~byteParameter();
    void setValue(byte value);
    void setLimits(byte minimum, byte maximum);
    byte minimum();
    byte maximum();
    boolean isOffLimit();
    byte value();

  private:
    byte _minimum;
    byte _maximum;
};

class boolParameter : public Parameter
{
  public:
    boolParameter();
    ~boolParameter();
    void setValue(bool value);
    boolean value();

};

class timeParameter
{
  public:
    timeParameter();
    timeParameter(short hour, short minut);
    ~timeParameter();
    void setTime(short hour, short minut);
    void addTime(short hour, short minut);
    void subTime(short hour, short minut);
    unsigned short hour();
    unsigned short minut();
    int address();

  private:
    void adjustTime();
    short _hour;
    short _minut;
};

boolean isBetween(byte startHour, byte startMin, byte actualHour, byte actualMin, byte stopHour, byte stopMin);


#endif

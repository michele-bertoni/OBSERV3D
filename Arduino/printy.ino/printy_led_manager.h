/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_LED_MANAGER_H
#define PRINTY_LED_MANAGER_H

#include "EEPROM.h"
#include "FastLED.h"

#define _LED_FIRST_EEPROM_ADDR 0
#define _LED_LAST_EEPROM_ADDR 12

#define _LED_DEBUG 0

class LedManager {
  public:
    LedManager();
    void setChamberLedsOn(bool ledsOn), setExtruderLedsOn(bool ledsOn);
    void setEditExtruderHSV(bool editExtruderHSV);
    void setHue(uint8_t value), setSaturation(uint8_t value), setBrightness(uint8_t value);
    void setEffectMode(uint8_t mode), setFadingMode(uint8_t mode), restoreEffectMode();
    void setEffectDuration(uint8_t duration), setFadingDuration(uint8_t duration);
    bool getChamberLedsOn(), getExtruderLedsOn(), getEditExtruderHSV();
    uint8_t getHue(), getSaturation(), getBrightness();
    uint8_t getExtHue(), getExtSaturation(), getExtBrightness();
    uint8_t getEffectMode(), getFadingMode();
    uint8_t getEffectDuration(), getFadingDuration();
    void storeSettings(), loadSettings(), resetSettings(uint8_t startingPosition=0);
    
  private:
    bool chamberLedsOn, extruderLedsOn, isEditExtruderHSV;
    uint8_t hue, saturation, brightness;
    uint8_t extHue, extSaturation, extBrightness;
    uint8_t effectMode, fadingMode, oldEffectMode;
    uint8_t effectDuration, fadingDuration;
};

#endif

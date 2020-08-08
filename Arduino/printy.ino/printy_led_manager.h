#ifndef PRINTY_LED_SETTINGS_H
#define PRINTY_LED_SETTINGS_H

#include "EEPROM.h"

#define _STARTING_EEPROM_ADDR 0

class LedManager {
  private:
    bool chamberLedsOn, extruderLedsOn;
    uint8_t hue, saturation, brightness;
    uint8_t effectMode, fadingMode, extruderMode;
    uint8_t effectDuration, fadingDuration;
    bool isRandomHue, isRandomEffectMode, isRandomFadingMode; 
    bool isRandomEffectDuration, isRandomFadingDuration;
  public:
    LedManager();
    void setChamberLedsOn(bool ledsOn), setExtruderLedsOn(bool ledsOn);
    void setHue(uint8_t value), setSaturation(uint8_t value), setBrightness(uint8_t value);
    void setEffectMode(uint8_t mode), setFadingMode(uint8_t mode), setExtruderMode(uint8_t value);
    void setEffectDuration(uint8_t duration), setFadingDuration(uint8_t duration);
    void setRandomHue(bool state), setRandomEffectMode(bool state), setRandomFadingMode(bool state); 
    void setRandomEffectDuration(bool state), setRandomFadingDuration(bool state);
    bool getChamberLedsOn(), getExtruderLedsOn();
    uint8_t getHue(), getSaturation(), getBrightness();
    uint8_t getEffectMode(), getFadingMode();
    uint8_t getEffectDuration(), getFadingDuration();
    bool getExtruderLedsRandom(), getExtruderLedsWhite(), getExtruderLedsAsChamber(), getExtruderLedsComplementary();
    void storeSettings(), loadSettings(), resetSettings();
};

#endif

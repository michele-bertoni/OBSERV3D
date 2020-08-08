#include "printy_led_manager.h"

LedManager::LedManager() {
  loadSettings();
}

void LedManager::setChamberLedsOn(bool ledsOn) {
  chamberLedsOn = ledsOn;
}

void LedManager::setExtruderLedsOn(bool ledsOn) {
  extruderLedsOn = ledsOn;
}

void LedManager::setHue(uint8_t value) {
  hue = value;
}

void LedManager::setSaturation(uint8_t value) { 
  saturation = value;
}

void LedManager::setBrightness(uint8_t value) {
  brightness = value;
  FastLED.setBrightness(brightness);
}

void LedManager::setEffectMode(uint8_t mode) {
  effectMode = mode;
}

void LedManager::setFadingMode(uint8_t mode) {
  fadingMode = mode;
}

void LedManager::setExtruderMode(uint8_t mode) {
  extruderMode = mode;
}

void LedManager::setEffectDuration(uint8_t duration) {
  effectDuration = duration;
}

void LedManager::setFadingDuration(uint8_t duration) {
  fadingDuration = duration;
}

void LedManager::setRandomHue(bool state) {
  isRandomHue = state;
}

void LedManager::setRandomEffectMode(bool state) {
  isRandomEffectMode = state;
}

void LedManager::setRandomFadingMode(bool state) {
  isRandomFadingMode = state;
}

void LedManager::setRandomEffectDuration(bool state) {
  isRandomEffectDuration = state;
}

void LedManager::setRandomFadingDuration(bool state) {
  isRandomFadingDuration = state;
}

bool LedManager::getChamberLedsOn() {
  return chamberLedsOn;
}

bool LedManager::getExtruderLedsOn() {
  return extruderLedsOn;
}

uint8_t LedManager::getHue() {
  if(isRandomHue)
    return random8();
  return hue;
}

uint8_t LedManager::getSaturation() {
  return saturation;
}

uint8_t LedManager::getBrightness() {
  return brightness;
}

uint8_t LedManager::getEffectMode() {
  if(isRandomEffectMode)
    return random8(28);
  return effectMode;
}

uint8_t LedManager::getFadingMode() {
  if(isRandomFadingMode)
    return random8(32);
  return fadingMode;
}

uint8_t LedManager::getEffectDuration() {
  if(isRandomEffectDuration)
    return random8(1, 41);
  return effectDuration;
}

uint8_t LedManager::getFadingDuration() {
  if(isRandomFadingDuration)
    return random8(8);
  return fadingDuration;
}

void LedManager::storeSettings() {
  return;
  EEPROM.update(_STARTING_EEPROM_ADDR, 1);
  EEPROM.update(_STARTING_EEPROM_ADDR+1, chamberLedsOn);
  EEPROM.update(_STARTING_EEPROM_ADDR+2, extruderLedsOn);
  EEPROM.update(_STARTING_EEPROM_ADDR+3, hue);
  EEPROM.update(_STARTING_EEPROM_ADDR+4, saturation);
  EEPROM.update(_STARTING_EEPROM_ADDR+5, brightness);
  EEPROM.update(_STARTING_EEPROM_ADDR+6, effectMode);
  EEPROM.update(_STARTING_EEPROM_ADDR+7, fadingMode);
  EEPROM.update(_STARTING_EEPROM_ADDR+8, effectDuration);
  EEPROM.update(_STARTING_EEPROM_ADDR+9, fadingDuration);
  EEPROM.update(_STARTING_EEPROM_ADDR+10, isRandomHue);
  EEPROM.update(_STARTING_EEPROM_ADDR+11, isRandomEffectMode);
  EEPROM.update(_STARTING_EEPROM_ADDR+12, isRandomFadingMode);
  EEPROM.update(_STARTING_EEPROM_ADDR+13, isRandomEffectDuration);
  EEPROM.update(_STARTING_EEPROM_ADDR+14, isRandomFadingDuration);
  EEPROM.update(_STARTING_EEPROM_ADDR+15, extruderMode);
}

void LedManager::loadSettings() {
  if(EEPROM.read(_STARTING_EEPROM_ADDR) == 1) {
    setChamberLedsOn(EEPROM.read(_STARTING_EEPROM_ADDR+1));
    setExtruderLedsOn(EEPROM.read(_STARTING_EEPROM_ADDR+2));
    setHue(EEPROM.read(_STARTING_EEPROM_ADDR+3));
    setSaturation(EEPROM.read(_STARTING_EEPROM_ADDR+4));
    setBrightness(EEPROM.read(_STARTING_EEPROM_ADDR+5));
    setEffectMode(EEPROM.read(_STARTING_EEPROM_ADDR+6)); 
    setFadingMode(EEPROM.read(_STARTING_EEPROM_ADDR+7));
    setEffectDuration(EEPROM.read(_STARTING_EEPROM_ADDR+8)); 
    setFadingDuration(EEPROM.read(_STARTING_EEPROM_ADDR+9));
    setRandomHue(EEPROM.read(_STARTING_EEPROM_ADDR+10)); 
    setRandomEffectMode(EEPROM.read(_STARTING_EEPROM_ADDR+11)); 
    setRandomFadingMode(EEPROM.read(_STARTING_EEPROM_ADDR+12)); 
    setRandomEffectDuration(EEPROM.read(_STARTING_EEPROM_ADDR+13)); 
    setRandomFadingDuration(EEPROM.read(_STARTING_EEPROM_ADDR+14));
    setExtruderMode(EEPROM.read(_STARTING_EEPROM_ADDR+15));
  }
  else {
    resetSettings();
    storeSettings();
  }
}

void LedManager::resetSettings() {
  setChamberLedsOn(true);
  setExtruderLedsOn(true);
  setHue(0);
  setSaturation(0);
  setBrightness(64);
  setEffectMode(0); 
  setFadingMode(0);
  setEffectDuration(20); 
  setFadingDuration(4);
  setRandomHue(true); 
  setRandomEffectMode(true); 
  setRandomFadingMode(true); 
  setRandomEffectDuration(false); 
  setRandomFadingDuration(false);
  setExtruderMode(0);
}

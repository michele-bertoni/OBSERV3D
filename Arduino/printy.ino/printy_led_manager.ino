/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

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

void LedManager::setEditExtruderHSV(bool editExtruderHSV) {
  isEditExtruderHSV = editExtruderHSV;
}

void LedManager::setHue(uint8_t value) {
  isEditExtruderHSV ? extHue=value : hue=value;
}

void LedManager::setSaturation(uint8_t value) { 
  isEditExtruderHSV ? extSaturation=value : saturation=value;
}

void LedManager::setBrightness(uint8_t value) {
  isEditExtruderHSV ? extBrightness=value : brightness=value;
}

void LedManager::setEffectMode(uint8_t mode) {
  effectMode = mode;
}

void LedManager::setFadingMode(uint8_t mode) {
  fadingMode = mode;
}

void LedManager::setEffectDuration(uint8_t duration) {
  effectDuration = duration;
}

void LedManager::setFadingDuration(uint8_t duration) {
  fadingDuration = duration;
}

bool LedManager::getChamberLedsOn() {
  return chamberLedsOn;
}

bool LedManager::getExtruderLedsOn() {
  return extruderLedsOn;
}

bool LedManager::getEditExtruderHSV() {
  return isEditExtruderHSV;
}

uint8_t LedManager::getHue() {
  return hue;
}

uint8_t LedManager::getSaturation() {
  return saturation;
}

uint8_t LedManager::getBrightness() {
  return brightness;
}

uint8_t LedManager::getExtHue() {
  return extHue;
}

uint8_t LedManager::getExtSaturation() {
  return extSaturation;
}

uint8_t LedManager::getExtBrightness() {
  return extBrightness;
}

uint8_t LedManager::getEffectMode() {
  return effectMode;
}

uint8_t LedManager::getFadingMode() {
  return fadingMode;
}

uint8_t LedManager::getEffectDuration() {
  return effectDuration;
}

uint8_t LedManager::getFadingDuration() {
  return fadingDuration;
}

void LedManager::storeSettings() {
  EEPROM.update(_LED_FIRST_EEPROM_ADDR, _LED_LAST_EEPROM_ADDR-_LED_FIRST_EEPROM_ADDR);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+1, chamberLedsOn);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+2, extruderLedsOn);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+3, hue);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+4, saturation);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+5, brightness);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+6, effectMode);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+7, fadingMode);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+8, effectDuration);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+9, fadingDuration);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+10, extHue);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+11, extSaturation);
  EEPROM.update(_LED_FIRST_EEPROM_ADDR+12, extBrightness);
}

void LedManager::loadSettings() {
  uint8_t storedBytes = EEPROM.read(_LED_FIRST_EEPROM_ADDR);
  isEditExtruderHSV = false;
  switch(storedBytes) {
    case 12: extBrightness = EEPROM.read(_LED_FIRST_EEPROM_ADDR+12); 
    case 11: extSaturation = EEPROM.read(_LED_FIRST_EEPROM_ADDR+11); 
    case 10: extHue = EEPROM.read(_LED_FIRST_EEPROM_ADDR+10);
    case 9:  setFadingDuration(EEPROM.read(_LED_FIRST_EEPROM_ADDR+9));
    case 8:  setEffectDuration(EEPROM.read(_LED_FIRST_EEPROM_ADDR+8)); 
    case 7:  setFadingMode(EEPROM.read(_LED_FIRST_EEPROM_ADDR+7));
    case 6:  setEffectMode(EEPROM.read(_LED_FIRST_EEPROM_ADDR+6)); 
    case 5:  brightness = EEPROM.read(_LED_FIRST_EEPROM_ADDR+5);
    case 4:  saturation = EEPROM.read(_LED_FIRST_EEPROM_ADDR+4);
    case 3:  hue = EEPROM.read(_LED_FIRST_EEPROM_ADDR+3);
    case 2:  setExtruderLedsOn(EEPROM.read(_LED_FIRST_EEPROM_ADDR+2)>0);
    case 1:  setChamberLedsOn(EEPROM.read(_LED_FIRST_EEPROM_ADDR+1)>0);
             resetSettings(storedBytes);
             break;
    default: resetSettings(); 
  }
  if(storedBytes != _LED_LAST_EEPROM_ADDR-_LED_FIRST_EEPROM_ADDR) {
    storeSettings();
  }
}

void LedManager::resetSettings(uint8_t startingPosition=0) {
  switch(startingPosition) {
    case 0:  setChamberLedsOn(true);
    case 1:  setExtruderLedsOn(true);
    case 2:  hue = 0;
    case 3:  saturation = 0;
    case 4:  brightness = 255;
    case 5:  setEffectMode(0); 
    case 6:  setFadingMode(0);
    case 7:  setEffectDuration(20); 
    case 8:  setFadingDuration(4);
    case 9:  extHue = 0; 
    case 10: extSaturation = 0;
    case 11: extBrightness = 64;
  }
}

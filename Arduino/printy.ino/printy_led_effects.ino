/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy_led_effects.h"

LedEffects::LedEffects() {
  /*
  setters[0] =&LedManager::setChamberLedsOn;
  setters[1] =&LedManager::setExtruderLedsOn;
  setters[2] =&LedManager::setHue;
  setters[3] =&LedManager::setSaturation;
  setters[4] =&LedManager::setBrightness;
  setters[5] =&LedManager::setEffectMode; 
  setters[6] =&LedManager::setFadingMode;
  setters[7] =&LedManager::setEffectDuration; 
  setters[8] =&LedManager::setFadingDuration;
  setters[9] =&LedManager::setRandomHue;
  setters[10]=&LedManager::setRandomEffectMode; 
  setters[11]=&LedManager::setRandomFadingMode;
  setters[12]=&LedManager::setRandomEffectDuration; 
  setters[13]=&LedManager::setRandomFadingDuration;
  setters[14]=&LedManager::setExtruderMode;
  */
}

void LedEffects::setup() {
  FastLED.addLeds<NEOPIXEL,PIN_LEDS_C1>(chLeds, Z2E, NUM_C1LEDS);
  FastLED.addLeds<NEOPIXEL,PIN_LEDS_C2>(chLeds, Y2C, NUM_C2LEDS);
  FastLED.addLeds<NEOPIXEL,PIN_LEDS_C3>(chLeds, Y0A, NUM_C3LEDS);
  FastLED.addLeds<NEOPIXEL,PIN_LEDS_C4>(chLeds, Z0E, NUM_C4LEDS);
  FastLED.addLeds<NEOPIXEL,PIN_LEDS_E>(extLeds, NUM_EXTLEDS);
}

void LedEffects::handleLeds() {
  //TODO
  uint8_t time = millis()>>_LEDS_TIME_SHIFT;
  if(time-lastUpdateTime >= _LEDS_UPDATE_TIME) {
    lastUpdateTime = time;
    fill_solid(chLeds, NUM_CHLEDS, CHSV(ledManager.getHue(), ledManager.getSaturation(), ledManager.getBrightness()));
    fill_solid(extLeds, NUM_EXTLEDS, CHSV(ledManager.getExtHue(), ledManager.getExtSaturation(), ledManager.getExtBrightness()));
    FastLED.show();
  }
}

void LedEffects::callStatusEffectByIndex(uint8_t index, uint8_t time){
    (this->*statusEffects[index])(time);
}

void LedEffects::callPrintingEffectByIndex(uint8_t index, uint8_t time){
    (this->*printingEffects[index])(time);
}

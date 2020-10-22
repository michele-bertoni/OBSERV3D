/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_LED_EFFECTS_H
#define PRINTY_LED_EFFECTS_H

#include "FastLED.h"

#define _LED_STATUS_EFFECTS 19
#define _LED_PRINTING_EFFECTS 28

#define NUM_CHLEDS 243
#define NUM_C1LEDS 67
#define NUM_C2LEDS 47
#define NUM_C3LEDS 51
#define NUM_C4LEDS 78
#define NUM_EXTLEDS 6

#define PIN_LEDS_C1 10
#define PIN_LEDS_C2 9
#define PIN_LEDS_C3 6
#define PIN_LEDS_C4 5
#define PIN_LEDS_E  11

#define _LEDS_TIME_SHIFT 2
#define _LEDS_UPDATE_FREQ 60  //possible frequencies: 2, 3, 4, 5, 6, 8, 12, 15, 16, 20, 24, 30, 48, 60, 80, 120
#define _LEDS_UPDATE_TIME ((1000/_LEDS_UPDATE_FREQ)>>_LEDS_TIME_SHIFT)

class LedEffects {
  public:
    LedEffects();
    void setup();
    void handleLeds();
    
  private:
    CRGBArray<NUM_CHLEDS> chLeds;
    CRGBArray<NUM_EXTLEDS> extLeds;
    typedef void (LedEffects::*Effect)(uint8_t);
    Effect statusEffects[_LED_STATUS_EFFECTS];
    Effect printingEffects[_LED_PRINTING_EFFECTS];
    void callStatusEffectByIndex(uint8_t index, uint8_t time);
    void callPrintingEffectByIndex(uint8_t index, uint8_t time);
    uint8_t lastUpdateTime;
};

#endif

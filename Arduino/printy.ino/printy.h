/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_H
#define PRINTY_H

#include "FastLED.h"
#include "printy_led_manager.h"
#include "printy_printer_status.h"
#include "printy_communication.h"

#define PIN_POWERBUTTON 7

#define PIN_LEDS_C1 5
#define PIN_LEDS_C2 6
#define PIN_LEDS_C3 9
#define PIN_LEDS_C4 10
#define PIN_LEDS_E  11

#if _COMM_DEBUG || _STATUS_DEBUG
  #define BAUDRATE 115200
#else
  #define BAUDRATE 2400
#endif

#endif

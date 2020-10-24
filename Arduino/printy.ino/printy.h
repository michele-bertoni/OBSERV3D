/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_H
#define PRINTY_H

#include "printy_led_manager.h"
#include "printy_led_effects.h"
#include "printy_printer_status.h"
#include "printy_communication.h"

#define PIN_POWER_BUTTON 7

#define PIN_5V A0
#define PIN_12V A2
#define PIN_24V A1
#define PIN_POWER_SWITCH 8

#define PIN_RASPI_SWITCH 12
#define PIN_RASPI_STATUS 4

#if _COMM_DEBUG || _STATUS_DEBUG
  #define BAUDRATE 115200
  #pragma message "Printy.ino v0.5 - DEBUG MODE"
  #define DEBUGMODE 1 
#else
  #define BAUDRATE 2400
  #pragma message "Printy.ino v0.5"
#endif

#endif

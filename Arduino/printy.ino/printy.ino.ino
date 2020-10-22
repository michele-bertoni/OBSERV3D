/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy.h"

LedManager ledManager;
LedEffects ledEffects;
PrinterStatus printerStatus(PIN_POWER_BUTTON, PIN_5V, PIN_12V, PIN_24V, PIN_POWER_SWITCH, PIN_RASPI_STATUS, PIN_RASPI_SWITCH);
Communication communication;

void setup() {
  // put your setup code here, to run once:
  communication.setup(BAUDRATE);
  ledEffects.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  communication.read();
  printerStatus.checkPowerStatus();
  ledEffects.handleLeds();
}

/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy.h"

LedManager ledManager;
PrinterStatus printerStatus(PIN_POWERBUTTON);
Communication communication;

void setup() {
  // put your setup code here, to run once:
  communication.setup(BAUDRATE);
}

void loop() {
  // put your main code here, to run repeatedly:
  communication.read();
  printerStatus.checkPowerStatus();
}

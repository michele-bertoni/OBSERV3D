/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy_printer_status.h"

PrinterStatus::PrinterStatus(uint8_t powerButtonPin)
  : powerButtonPin(powerButtonPin)
{
  pinMode(powerButtonPin, INPUT_PULLUP);
}

void PrinterStatus::checkPowerStatus() {
  uint8_t time = millis()>>_STATUS_TIME_SHIFT;
  if(checkPowerButton(time) || ((scheduledPowerOff || scheduledReboot) && isSafeToSwitch(newPrinterStatus) && isSafeToSwitch(printerStatus))) {
    if(printerStatus != STATUS_O) {
      setNewPrinterStatus(STATUS_D);
      scheduledPowerOff = false;
      communication.write(OUT_SHUTDOWN);
      switchPhase = 0;
      lastSwitchPhaseTime = time;
    }
    else if(!scheduledPowerOff){
      setNewPrinterStatus(STATUS_U);
      scheduledReboot = false;
      switchPhase = 0;
      lastSwitchPhaseTime = time;
    }
  }
  if(newPrinterStatus == STATUS_D) {
    handleShutDown(time);
  }
  else if(newPrinterStatus == STATUS_U) {
    handleBootUp(time);
  }
}

bool PrinterStatus::checkPowerButton(uint8_t time) {
  if(digitalRead(powerButtonPin)) {   //button not pressed
    lastPBUnpressedTime = time;
    wasButtonPressed = false;
  }
  else if(!wasButtonPressed) {        //button pressed and not already pressed
    if((time-lastPBUnpressedTime>=_TIME_LONGPRESS && isSafeToForceSwitch(newPrinterStatus) && isSafeToForceSwitch(printerStatus)) || 
       (time-lastPBUnpressedTime>=_TIME_SHORTPRESS && isSafeToSwitch(newPrinterStatus) && isSafeToSwitch(printerStatus))) {
      wasButtonPressed = true;
      return true;
    }
  }
  return false;
}

void PrinterStatus::setNewPrinterStatus(uint8_t status) {
  lastSwitchPhaseTime = millis();
  if(hasNewStatus()) {
    switchToNewPrinterStatus();
  }
  newPrinterStatus = status;
}

uint8_t PrinterStatus::getPrinterStatus() {
  return printerStatus;
}

uint8_t PrinterStatus::getNewPrinterStatus() {
  return newPrinterStatus;
}

bool PrinterStatus::isSafeToSwitch(uint8_t status) {
  return status==STATUS_O || status==STATUS_I || status==STATUS_E || status==STATUS_H;
}

bool PrinterStatus::isSafeToForceSwitch(uint8_t status) {
  return status!=STATUS_F;
}

void PrinterStatus::switchToNewPrinterStatus() {
  printerStatus = newPrinterStatus;
}

bool PrinterStatus::hasNewStatus() {
  return newPrinterStatus != printerStatus;
}

void PrinterStatus::schedulePowerOff(bool isScheduled) {
  scheduledPowerOff = isScheduled;
}

void PrinterStatus::scheduleReboot(bool isScheduled) {
  scheduledReboot = isScheduled;
}

void PrinterStatus::handleShutDown(uint8_t time) {
  switch(switchPhase) {
    case 0: break;
  }
}

void PrinterStatus::handleBootUp(uint8_t time) {
  switch(switchPhase) {
    case 0: break;
  }
}

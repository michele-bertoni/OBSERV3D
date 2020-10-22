/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy_printer_status.h"

PrinterStatus::PrinterStatus(uint8_t powerButtonPin, uint8_t v5Pin, uint8_t v12Pin, uint8_t v24Pin, uint8_t powerSwitchPin, uint8_t raspiStatusPin, uint8_t raspiSwitchPin)
  : powerButtonPin(powerButtonPin),
    powerStatus(v5Pin, v12Pin, v24Pin, powerSwitchPin),
    raspiStatus(raspiStatusPin, raspiSwitchPin)
{
  loadSettings();
  pinMode(powerButtonPin, INPUT_PULLUP);
}

void PrinterStatus::checkPowerStatus() {
  uint8_t time = millis()>>_STATUS_TIME_SHIFT;
  
  if(checkPowerButton(time) || ((scheduledPowerOff || scheduledReboot) && isSafeToSwitch(newPrinterStatus) && isSafeToSwitch(printerStatus))) {
    /* if the printer is on and it might switch off */
    if(printerStatus != STATUS_O) {
      setNewPrinterStatus(STATUS_D);
      scheduledPowerOff = false;
      powerStatus.setScheduledOn(false);
      raspiStatus.setScheduledOn(keepRaspberryOn);
    }
    /* if the printer is off and it might switch on */
    else if(!scheduledPowerOff){
      setNewPrinterStatus(STATUS_U);
      scheduledReboot = false;
      powerStatus.setScheduledOn(true);
      raspiStatus.setScheduledOn(true);
    }
  }
  
  powerStatus.handlePowerStatus();
  raspiStatus.handleRaspiStatus();

  if(printerStatus != STATUS_O && powerStatus.isOff()) {
    raspiStatus.setScheduledOn(keepRaspberryOn);
    if(keepRaspberryOn || raspiStatus.isOff()) {
      setNewPrinterStatus(STATUS_O);
    }
  }
  else if(printerStatus == STATUS_O && !powerStatus.isOff()) {
    setNewPrinterStatus(STATUS_U);
    raspiStatus.setScheduledOn(true);
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
  if(hasNewStatus()) {
    switchToNewPrinterStatus();
  }
  newPrinterStatus = status;

  //TODO: move the following line into effect manager
  switchToNewPrinterStatus();
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

void PrinterStatus::setKeepRaspberryOn(bool keepOn) {
  keepRaspberryOn = keepOn;
  if(printerStatus == STATUS_O) {
    raspiStatus.setScheduledOn(keepRaspberryOn);
  }
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

void PrinterStatus::storeSettings() {
  EEPROM.update(_STATUS_FIRST_EEPROM_ADDR, _STATUS_LAST_EEPROM_ADDR-_STATUS_FIRST_EEPROM_ADDR);
  EEPROM.update(_STATUS_FIRST_EEPROM_ADDR+1, keepRaspberryOn);
}

void PrinterStatus::loadSettings() {
uint8_t storedBytes = EEPROM.read(_STATUS_FIRST_EEPROM_ADDR);
  switch(storedBytes) {
    case 1: setKeepRaspberryOn(EEPROM.read(_STATUS_FIRST_EEPROM_ADDR+1));
    default: resetSettings(); 
  }
  if(storedBytes != _STATUS_LAST_EEPROM_ADDR-_STATUS_FIRST_EEPROM_ADDR) {
    storeSettings();
  }
}

void PrinterStatus::resetSettings(uint8_t startingPosition=0) {
  switch(startingPosition) {
    case 0: setKeepRaspberryOn(false);
  }
}

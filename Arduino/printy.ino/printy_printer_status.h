/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_PRINTER_STATUS_H
#define PRINTY_PRINTER_STATUS_H

#include "EEPROM.h"
#include "printy_raspi_status.h"
#include "printy_power_status.h"

#define STATUS_O  0     //Off
#define STATUS_U  1     //Booting up
#define STATUS_D  2     //Shutting down
#define STATUS_I  3     //Idle
#define STATUS_P  4     //Printing
#define STATUS_S  5     //Paused
#define STATUS_C  6     //Changing filament
#define STATUS_E  7     //Error
#define STATUS_W  8     //Warning, print is running, but temperatures are not as expected
#define STATUS_H  9     //Halted (emergency stop)
#define STATUS_Q  10    //Heated
#define STATUS_F  11    //Firmware update
#define STATUS_B  12    //Performing macro (busy)

#define _STATUS_DEBUG 0

#define _STATUS_TIME_SHIFT 6
#define _TIME_LONGPRESS (5120>>_STATUS_TIME_SHIFT)
#define _TIME_SHORTPRESS (128>>_STATUS_TIME_SHIFT)

#define _STATUS_FIRST_EEPROM_ADDR 100
#define _STATUS_LAST_EEPROM_ADDR 101

class PrinterStatus {
  private: 
    PowerStatus powerStatus; 
    RaspiStatus raspiStatus;
    uint8_t printerStatus, newPrinterStatus;
    bool scheduledPowerOff, scheduledReboot;
    uint8_t lastPBUnpressedTime;
    bool wasButtonPressed;
    bool keepRaspberryOn;
    const uint8_t powerButtonPin;
    bool isSafeToSwitch(uint8_t status), isSafeToForceSwitch(uint8_t status); 
    bool checkPowerButton(uint8_t time);

  public: 
    PrinterStatus(uint8_t powerButtonPin, uint8_t v5Pin, uint8_t v12Pin, uint8_t v24Pin, uint8_t powerSwitchPin, uint8_t raspiStatusPin, uint8_t raspiSwitchPin);
    void checkPowerStatus(); 
    void setNewPrinterStatus(uint8_t status);
    void switchToNewPrinterStatus();
    void setKeepRaspberryOn(bool keepOn);
    bool hasNewStatus();
    uint8_t getPrinterStatus(), getNewPrinterStatus();
    void schedulePowerOff(bool isScheduled), scheduleReboot(bool isScheduled);
    void storeSettings(), loadSettings(), resetSettings(uint8_t startingPosition=0);
    uint8_t x, z, y, te, tc, tb, dte, dtc, dtb; 
};

#endif

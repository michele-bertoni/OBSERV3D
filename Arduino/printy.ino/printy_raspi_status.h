/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_RASPI_STATUS_H
#define PRINTY_RASPI_STATUS_H

/**
 * Time constants
 */
#define _RASPI_TIME_SHIFT 8                             //discard the first n less significant bits
#define _RASPI_TIME_SHORTPRESS (512>>_RASPI_TIME_SHIFT) //raspiSwitchPin will go to ground for 512ms
#define _RASPI_TIME_LONGPRESS (2304>>_RASPI_TIME_SHIFT) //raspiSwitchPin will go to ground for 2304ms
#define _RASPI_TIME_WAIT (29952>>_RASPI_TIME_SHIFT)     //check if Raspberry is on every 29.952s

/**
 * Possible status of Raspberry Pi
 */
#define _RASPI_STATUS_OFF 0
#define _RASPI_STATUS_SHORTPRESS 1  //between button pressure and release; right after this phase RPi should start booting up
#define _RASPI_STATUS_ON 2
#define _RASPI_STATUS_LONGPRESS 3   //between button pressure and release; right after this phase RPi should start switching down

/**
 * Class for asynchronously managing the status of the Raspberry; 
 * it can be used as a black box, so we know that by setting the
 * desired status of the RPi with setScheduledOn(bool isOn) and
 * by calling handleRaspiStatus() every loop iteration, the RPi
 * will reach the given status as fast as possible. If not, this
 * class will automatically repeat the procedure and send an error.
 */
class RaspiStatus {
  private: 
    uint8_t raspiStatus, lastSwitchStatusTime;            //current status of the Raspberry and time since that status was last changed
    const uint8_t raspiStatusPin, raspiSwitchPin;         //pins for reading if the Raspberry is on or off and for writing to switch it on or off
    bool isScheduledOn;                                   //boolean telling if the Raspberry is scheduled to be on or off
    void switchRaspiStatus(uint8_t status, uint8_t time);
    void simulateButtonPress(), simulateButtonRelease();
    bool isRaspiOn();
    void setLastSwitchStatusTime(uint8_t time);

  public: 
    RaspiStatus(uint8_t raspiStatusPin, uint8_t raspiSwitchPin);
    void handleRaspiStatus(); 
    void setScheduledOn(bool isOn);
    bool isOff();
};

#endif

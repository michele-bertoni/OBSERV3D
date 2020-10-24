/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy_raspi_status.h"

/**
 * RaspiStatus constructor, initializes pins and raspiStatus
 */
RaspiStatus::RaspiStatus(uint8_t raspiStatusPin, uint8_t raspiSwitchPin)
  : raspiStatusPin(raspiStatusPin), raspiSwitchPin(raspiSwitchPin)
{
  pinMode(raspiStatusPin, INPUT);
  raspiStatus = _RASPI_STATUS_ON;
  lastSwitchStatusTime = millis()>>_RASPI_TIME_SHIFT;
  isScheduledOn = true;
}

/**
 * Function to be called every loop iteration
 * Handles asynchronously Raspberry powering on and off
 */
void RaspiStatus::handleRaspiStatus() {
  uint8_t time = millis()>>_RASPI_TIME_SHIFT;
  switch(raspiStatus) {
    case _RASPI_STATUS_OFF:         //Raspberry is off
      if(isScheduledOn) {                                       //if it was scheduled to switch Raspberry on
        switchRaspiStatus(_RASPI_STATUS_SHORTPRESS, time);
      }
      else if(time-lastSwitchStatusTime >= _RASPI_TIME_WAIT) {  //or if Raspberry should be off but it's on
        isRaspiOn() ? switchRaspiStatus(_RASPI_STATUS_LONGPRESS, time) : setLastSwitchStatusTime(time);
        //TODO: debug error
      }
      break;
    case _RASPI_STATUS_SHORTPRESS:  //Raspberry is off and the powering simulated button is being pressed
      if(time-lastSwitchStatusTime>=_RASPI_TIME_SHORTPRESS) {   //if enough time passed
        switchRaspiStatus(_RASPI_STATUS_ON, time);
      }
      break;
    case _RASPI_STATUS_ON:          //Raspberry is on
      if(!isScheduledOn) {                                      //if it was scheduled to switch Raspberry off
        switchRaspiStatus(_RASPI_STATUS_LONGPRESS, time);
      }
      else if(time-lastSwitchStatusTime >= _RASPI_TIME_WAIT) {  //or if Raspberry should be on but it's off
        !isRaspiOn() ? switchRaspiStatus(_RASPI_STATUS_SHORTPRESS, time) : setLastSwitchStatusTime(time);
        //TODO: debug error
      }
      break;
    case _RASPI_STATUS_LONGPRESS:   //Raspberry is on and the powering simulated button is being pressed
      if(time-lastSwitchStatusTime>=_RASPI_TIME_LONGPRESS) {    //if enough time passed
        switchRaspiStatus(_RASPI_STATUS_OFF, time);
      }
      break;
  }
}

/**
 * Sets scheduled Raspi status
 */
void RaspiStatus::setScheduledOn(bool isOn) {
  isScheduledOn = isOn;
  #if _STATUS_DEBUG
    Serial.print("R: -> ");
    Serial.println(isOn);
  #endif
}

/**
 * Returns true if Raspberry is off, false otherwise
 */
bool RaspiStatus::isOff() {
  return raspiStatus == _RASPI_STATUS_OFF;
}

/**
 * Switch to the a new status, passed as argument, updating the time of the latest status switch and simulating button pressure and release
 */
void RaspiStatus::switchRaspiStatus(uint8_t status, uint8_t time) {
  if(status == _RASPI_STATUS_OFF || status == _RASPI_STATUS_ON) {
    simulateButtonRelease();
  }
  else if(status == _RASPI_STATUS_SHORTPRESS || status == _RASPI_STATUS_LONGPRESS) {
    simulateButtonPress();
  }
  raspiStatus = status;
  lastSwitchStatusTime = time;
  #if _STATUS_DEBUG
    Serial.print("R: status ");
    Serial.println(status);
  #endif
}

/**
 * Simulates the pressure of the button to switch on and off the Raspberry Pi
 */
void RaspiStatus::simulateButtonPress() {
  pinMode(raspiSwitchPin, OUTPUT);
  digitalWrite(raspiSwitchPin, LOW);
  #if _STATUS_DEBUG
    Serial.println("R: bDown");
  #endif
}

/**
 * Simulates the release of the button to switch on and off the Raspberry Pi
 */
void RaspiStatus::simulateButtonRelease() {
  pinMode(raspiSwitchPin, INPUT);
  #if _STATUS_DEBUG
    Serial.println("R: bUp");
  #endif
} 

/**
 * Reads the status of the Raspberry through the corresponding digital pin
 */
bool RaspiStatus::isRaspiOn() {
  bool isRaspiOn = digitalRead(raspiStatusPin)==HIGH;
  #if _STATUS_DEBUG
    Serial.println(isRaspiOn ? "R: on" : "R: off"); 
  #endif
  return isRaspiOn;
}

/**
 * Sets lastSwitchStatus to the given time
 */
void RaspiStatus::setLastSwitchStatusTime(uint8_t time) {
  lastSwitchStatusTime = time;
}

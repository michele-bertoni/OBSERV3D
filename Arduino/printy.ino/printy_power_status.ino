/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy_power_status.h"

/**
 * PowerStatus constructor, initializes pins and powerStatus
 */
PowerStatus::PowerStatus(uint8_t v5Pin, uint8_t v12Pin, uint8_t v24Pin, uint8_t powerSwitchPin)
  : v5Pin(v5Pin), v12Pin(v12Pin), v24Pin(v24Pin), powerSwitchPin(powerSwitchPin)
{
  pinMode(v5Pin, INPUT);
  pinMode(v12Pin, INPUT);
  pinMode(v24Pin, INPUT);
  powerStatus = _POWER_STATUS_CHARGE;
  lastSwitchStatusTime = millis()>>_POWER_TIME_SHIFT;
  dischargeStartTime = lastSwitchStatusTime;
  isScheduledOn = true;
}

/**
 * Function to be called every loop iteration
 * Handles asynchronously power strip switching on and off
 */
void PowerStatus::handlePowerStatus() {
  uint8_t time = millis()>>_POWER_TIME_SHIFT;
  switch(powerStatus) {
    case _POWER_STATUS_OFF:         //Power strip is off
      if(isScheduledOn) {                                       //if it was scheduled to switch power strip on
        !isPowerOn() ? switchPowerStatus(_POWER_STATUS_PRESSON, time) : switchPowerStatus(_POWER_STATUS_ON, time);
      }
      else if((uint8_t)(time-lastSwitchStatusTime) >= _POWER_TIME_WAIT) {  //or if power strip should be off, but it has been switched on in other ways
        isPowerOn() ? switchPowerStatus(_POWER_STATUS_ON, time) : setLastSwitchStatusTime(time);
      }
      break;
    case _POWER_STATUS_PRESSON:     //Power strip is off and the powering simulated button is being pressed
      if((uint8_t)(time-lastSwitchStatusTime)>=_POWER_TIME_PRESS) {   //if enough time passed
        switchPowerStatus(_POWER_STATUS_ON, time);
      }
      break;
    case _POWER_STATUS_ON:          //Power strip is on
      if(!isScheduledOn) {                                      //if it was scheduled to switch power strip off
        isPowerOn() ? switchPowerStatus(_POWER_STATUS_PRESSOFF, time) : switchPowerStatus(_POWER_STATUS_OFF, time);
      }
      else if((uint8_t)(time-lastSwitchStatusTime) >= _POWER_TIME_WAIT) {  //or if power strip should be on, but it has been switched off in other ways
        !isPowerOn() ? switchPowerStatus(_POWER_STATUS_OFF, time) : setLastSwitchStatusTime(time);
      }
      break;
    case _POWER_STATUS_PRESSOFF:    //Power strip is on and the powering simulated button is being pressed
      if((uint8_t)(time-lastSwitchStatusTime)>=_POWER_TIME_PRESS) {    //if enough time passed
        switchPowerStatus(_POWER_STATUS_DISCHARGE, time);
      }
      break;
    case _POWER_STATUS_DISCHARGE: //Power strip is off, but PSUs voltage values might still be high for at maximum _POWER_TIME_DISCHARGE
      if((uint8_t)(time-lastSwitchStatusTime) >= _POWER_TIME_WAIT) {     //if discharging completed
        !isPowerOn() ? switchPowerStatus(_POWER_STATUS_OFF, time) : setLastSwitchStatusTime(time);
      }
      if((uint8_t)(time-dischargeStartTime) >= _POWER_TIME_DISCHARGE) {  //if discharging is taking too much time
        !isPowerOn() ? switchPowerStatus(_POWER_STATUS_OFF, time) : switchPowerStatus(_POWER_STATUS_ON, time);
      }
      break;
    case _POWER_STATUS_CHARGE: //Power strip is on, but PSUs voltage values might still be low for at maximum _POWER_TIME_CHARGE
      if((uint8_t)(time-lastSwitchStatusTime) >= _POWER_TIME_WAIT) {     //if charging completed
        isPowerOn() ? switchPowerStatus(_POWER_STATUS_ON, time) : setLastSwitchStatusTime(time);
      }
      if((uint8_t)(time-dischargeStartTime) >= _POWER_TIME_CHARGE) {     //if charging is taking too much time
        isPowerOn() ? switchPowerStatus(_POWER_STATUS_ON, time) : switchPowerStatus(_POWER_STATUS_OFF, time);
      }
      break;
  }
}

/**
 * Sets scheduled power strip status
 */
void PowerStatus::setScheduledOn(bool isOn) {
  isScheduledOn = isOn;

  #if _STATUS_DEBUG
    Serial.println(isOn ? "P: -> on" : "P: -> off");
  #endif
}

/**
 * Returns true if power strip is off, false otherwise
 */
bool PowerStatus::isOff() {
  return powerStatus == _POWER_STATUS_OFF;
}

/**
 * Switch to the a new status, passed as argument, updating the time of the latest status switch and simulating button pressure and release
 */
void PowerStatus::switchPowerStatus(uint8_t status, uint8_t time) {
  switch(status) {
    case _POWER_STATUS_OFF:
      setScheduledOn(false);
      simulateButtonRelease();
      break;
    case _POWER_STATUS_DISCHARGE:
    case _POWER_STATUS_CHARGE:
      dischargeStartTime = time;
      simulateButtonRelease();
      break;
    case _POWER_STATUS_ON:
      setScheduledOn(true);
      simulateButtonRelease();
      break;
    case _POWER_STATUS_PRESSON: 
    case _POWER_STATUS_PRESSOFF: 
      simulateButtonPress();
      break;
  }
  powerStatus = status;
  lastSwitchStatusTime = time;

  #if _STATUS_DEBUG
    Serial.print("P: newStatus ");
    Serial.println(status);
  #endif
}

/**
 * Simulates the pressure of the button to switch on and off the power strip
 */
void PowerStatus::simulateButtonPress() {
  pinMode(powerSwitchPin, OUTPUT);
  digitalWrite(powerSwitchPin, LOW);
  #if _STATUS_DEBUG
    Serial.println("P: bDown");
  #endif
}

/**
 * Simulates the release of the button to switch on and off the power strip
 */
void PowerStatus::simulateButtonRelease() {
  pinMode(powerSwitchPin, INPUT);
  #if _STATUS_DEBUG
    Serial.println("P: bUp");
  #endif
} 

/**
 * Reads the status of the PSUs with the corresponding analog pins
 */
bool PowerStatus::isPowerOn() {
  voltage5 = analogRead(v5Pin)*_TO_5V+0.5;
  voltage12 = analogRead(v12Pin)*_TO_12V+0.5;
  voltage24 = analogRead(v24Pin)*_TO_24V+0.5;
  
  #if _STATUS_DEBUG
    Serial.print(voltage5/8.0);
    Serial.print("V, ");
    Serial.print(voltage12/8.0);
    Serial.print("V, ");
    Serial.print(voltage24/8.0);
    Serial.println("V");
  #endif
  
  return  (voltage5>=_5V-_5V_TOLERANCE) || (voltage12>=_12V-_12V_TOLERANCE) || (voltage24>=_24V-_24V_TOLERANCE);
}

/**
 * Sets lastSwitchStatus to the given time
 */
void PowerStatus::setLastSwitchStatusTime(uint8_t time) {
  lastSwitchStatusTime = time;
}

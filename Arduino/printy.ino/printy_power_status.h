/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#ifndef PRINTY_POWER_STATUS_H
#define PRINTY_POWER_STATUS_H

/**
 * Time constants
 */
#define _POWER_TIME_SHIFT (uint8_t)(8)                             //discard the first n less significant bits
#define _POWER_TIME_PRESS (uint8_t)(512>>_POWER_TIME_SHIFT)        //powerSwitchPin will go to ground for 512ms
#define _POWER_TIME_WAIT (uint8_t)(3072>>_POWER_TIME_SHIFT)        //check if PSUs are on every 3.072s
#define _POWER_TIME_DISCHARGE (uint8_t)(59904>>_POWER_TIME_SHIFT)  //max dicharge time is 59.904s
#define _POWER_TIME_CHARGE (uint8_t)(6144>>_POWER_TIME_SHIFT)      //max charge time is 6.144s

/**
 * Possible status of power strip
 */
#define _POWER_STATUS_OFF (uint8_t)(0)
#define _POWER_STATUS_PRESSON (uint8_t)(1)    //between button pressure and release; right after this phase the power strip will switch on
#define _POWER_STATUS_CHARGE (uint8_t)(2)     //power strip is on, but the PSUs voltage values might be still low for some seconds
#define _POWER_STATUS_ON (uint8_t)(3)
#define _POWER_STATUS_PRESSOFF (uint8_t)(4)   //between button pressure and release; right after this phase the power strip will switch off
#define _POWER_STATUS_DISCHARGE (uint8_t)(5)  //power strip is off, but the PSUs voltage values might be still high for some seconds

/**
 * Voltage calculations constants
 */
#define _V_MULT 8   //multiplicator in order to keep integer values
#define _V_OP 5     //operating voltage is 5V
#define _V_MAX 1023 //max readable value from analogRead, corresponding to a voltage equal to _V_OP
#define _V_CONV ((float)(_V_MULT*_V_OP)/_V_MAX)

/**
 * Voltage values
 */
#define _5V ((uint8_t)(5*_V_MULT))
#define _12V ((uint8_t)(12*_V_MULT))
#define _24V ((uint8_t)(25*_V_MULT))

/*
 * Voltage tolerances
 */
#define _5V_TOLERANCE ((uint8_t)(0.5*_V_MULT))   
#define _12V_TOLERANCE ((uint8_t)(1*_V_MULT))
#define _24V_TOLERANCE ((uint8_t)(2*_V_MULT))

/*
 * Voltage divider values
 */
#define _5V_DIVIDER (12.14f/9.93f)
#define _12V_DIVIDER (14.65f/4.665f)
#define _24V_DIVIDER (26.37f/4.7f)

/**
 * Multiplier to obtain uint8_t 8*actual_voltage from the 10-bit value read by analogRead()
 */
#define _TO_5V (_5V_DIVIDER*_V_CONV)
#define _TO_12V (_12V_DIVIDER*_V_CONV)
#define _TO_24V (_24V_DIVIDER*_V_CONV)

/**
 * Class for asynchronously managing the status of the power strip; 
 * it can be used as a black box, so we know that by setting the
 * desired status of the power strip with setScheduledOn(bool isOn) and
 * by calling handlePowerStatus() every loop iteration, the power strip
 * will reach the given status as fast as possible. If not, this
 * class will automatically repeat the procedure and send an error.
 */
class PowerStatus {
  private: 
    uint8_t powerStatus, lastSwitchStatusTime;            //current status of the main power and time since that status was last changed
    uint8_t dischargeStartTime;                           //time when latest charge/discharge began
    uint8_t voltage5, voltage12, voltage24;               //voltage values of the 5V, 12V and 24V PSUs; value stored is 8 times the real value
    const uint8_t v5Pin, v12Pin, v24Pin;                  //pins for reading actual voltage values of the 5V, 12V and 24V PSUs
    const uint8_t powerSwitchPin;                         //pin for writing to switch power strip on or off
    bool isScheduledOn;                                   //boolean telling if the power strip is scheduled to be on or off
    void switchPowerStatus(uint8_t status, uint8_t time);
    void simulateButtonPress(), simulateButtonRelease();
    bool isPowerOn();
    void setLastSwitchStatusTime(uint8_t time);

  public: 
    PowerStatus(uint8_t v5Pin, uint8_t v12Pin, uint8_t v24Pin, uint8_t powerSwitchPin);
    void handlePowerStatus(); 
    void setScheduledOn(bool isOn);
    bool isOff();
};

#endif

#ifndef PRINTY_PRINTER_STATUS_H
#define PRINTY_PRINTER_STATUS_H

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

#define _STATUS_DEBUG 1

#define _STATUS_TIME_SHIFT 8
#define _TIME_LONGPRESS (5120>>_STATUS_TIME_SHIFT)
#define _TIME_SHORTPRESS (256>>_STATUS_TIME_SHIFT)

class PrinterStatus {
  private: 
    uint8_t printerStatus, newPrinterStatus, switchPhase;
    bool scheduledPowerOff, scheduledReboot;
    uint8_t lastPBUnpressedTime, lastSwitchPhaseTime;
    bool wasButtonPressed;
    const uint8_t powerButtonPin;
    bool isSafeToSwitch(uint8_t status), isSafeToForceSwitch(uint8_t status); 
    bool checkPowerButton(uint8_t time);
    void handleShutDown(uint8_t time), handleBootUp(uint8_t time);

  public: 
    PrinterStatus(uint8_t powerButtonPin);
    void checkPowerStatus(); 
    void setNewPrinterStatus(uint8_t status);
    void switchToNewPrinterStatus();
    bool hasNewStatus();
    uint8_t getPrinterStatus(), getNewPrinterStatus();
    void schedulePowerOff(bool isScheduled), scheduleReboot(bool isScheduled);
    uint8_t x, z, y, te, tc, tb, dte, dtc, dtb; 
};

#endif

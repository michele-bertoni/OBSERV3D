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

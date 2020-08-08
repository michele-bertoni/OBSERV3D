#ifndef PRINTY_COMMUNICATION_H
#define PRINTY_COMMUNICATION_H

#define _COMM_DEBUG 0

#define OUT_SHUTDOWN 'D'

class Communication {
  private: 
    uint8_t awaitingData;
    uint8_t awaitingStatus;
    uint8_t handleMessage(byte message);
    void handleExtraData();

  public:
    Communication();
    void setup(long baud);
    void read();
    void write(byte data);
};

#endif

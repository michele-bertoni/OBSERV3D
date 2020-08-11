/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

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

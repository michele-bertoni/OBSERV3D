###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import os.path as path
import socket
import time
import requests
import serial
from simple_line_protocol import SimpleLineProtocol
from stored_values import StoredValues
from message import DuetMessage

UPDATING_PATH = "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating"

conf_path = "/home/pi/Printy-McPrintface/Raspberry/.config/"
duet_ip_conf_path = conf_path + "duet_ip.conf"

DUET_HOST = "192.168.0.3"
try:
    with open(duet_ip_conf_path, 'r') as f:
        DUET_HOST = f.readline().rstrip('\n\r')
except Exception as e:
    print(e)

REQUEST_URL = "http://{}/rr_status?type=".format(DUET_HOST)

SERIAL_PORT = "/dev/ttyUSB0"
BAUDRATE = 2400

serMessFromStatus = {'C':-1,
                     'I':128,
                     'B':-1,
                     'P':.1,
                     'D':-1,
                     'S':-1,
                     'R':-1,
                     'H':-1,
                     'F':136,
                     'T':-1
                     }

if __name__ == "__main__":
    storedValues = StoredValues(".storedValues.json")
    duet_message = DuetMessage(stored_values=storedValues)
    ser = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, timeout=.1)
    statusType = 2

    x=0
    y=0
    z=0
    te=0
    tc=0
    tb=0
    dte=0
    dtc=0
    dtb=0

    isTelnetConnected = False

    #TODO: change to non-blocking
    while not isTelnetConnected:
        try:
            print("Connecting to {}...".format(DUET_HOST))
            time.sleep(1)
            sock = socket.create_connection((DUET_HOST, 23), timeout=0.1)
            time.sleep(4.5)  # RepRapFirmware uses a 4-second ignore period after connecting
            conn = SimpleLineProtocol(sock)
            print("Connection established.")
            isTelnetConnected = True
        except Exception as e:
            print(e)

    scheduledTime = time.time()
    while not path.exists(UPDATING_PATH):
        if time.time() >= scheduledTime:
            try:
                telnetMessages = conn.read_lines()
                while len(telnetMessages) > 0:
                    m = telnetMessages.pop(0)
                    arduinoMessages = duet_message.handle_message(m)
                    for am, log in arduinoMessages:
                        if am >= 0:
                            #ser.write(am.to_bytes(1, "big"))
                            pass
                        print(log)
                response = requests.get(REQUEST_URL+str(statusType)).json()
                #TODO: handle status
                status = response['status']
                if status == 'I':
                    ser.write(serMessFromStatus[status].to_bytes(1, "big"))
                    pass
                elif status == 'P':
                    statusType = 3

                if status != 'P':
                    statusType = 1
            except Exception as e:
                print(e)

            scheduledTime += 0.5
        sleepTime = scheduledTime - time.time()
        if sleepTime > 0:
            time.sleep(sleepTime)

    ser.write(serMessFromStatus['F'].to_bytes(1, "big"))
    ser.close()

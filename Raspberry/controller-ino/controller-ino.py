###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import os.path as path
import time
import requests
import serial
from connections import Connection
from stored_values import StoredValues
from message import DuetMessage

UPDATING_PATH = "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating"

conf_path = "/home/pi/Printy-McPrintface/Raspberry/.config/"
duet_ip_conf_path = conf_path + "duet_ip.conf"
socket_port_conf_path = conf_path + "telegram-bot_socket_port.conf"

DUET_HOST = "192.168.0.3"
try:
    with open(duet_ip_conf_path, 'r') as f:
        DUET_HOST = f.readline().rstrip('\n\r')
except Exception as e:
    print(e, flush=True)

socket_port = 6126
try:
    with open(socket_port_conf_path, 'r') as f:
        socket_port = int(f.readline().rstrip('\n\r'))
except Exception as e:
    print(e, flush=True)

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
    connections = [
        Connection(address=DUET_HOST, port=23, host_type='DUET', timeout=0.1, ignore_period=4.5),
        Connection(address='127.0.0.1', port=socket_port, host_type='TELEBOT', timeout=0.1, ignore_period=0.0)
    ]
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

    isDisconnected = False

    scheduledTime = time.time()
    while not path.exists(UPDATING_PATH) and not isDisconnected:
        if time.time() >= scheduledTime:
            try:
                for conn in connections:
                    for m in conn.handle_connection():
                        response = ''
                        arduinoMessages = duet_message.handle_message(m)
                        for am, log in arduinoMessages:
                            if 0 <= am < 256:
                                ser.write(am.to_bytes(1, "big"))
                            elif am < 0 or am == 257:
                                response += log + ', '
                            print(log, flush=True)
                        if len(response) <= 0 and conn.host_type=='TELEBOT':
                            response = 'ok'
                        else:
                            response = response[:len(response)-2]
                        conn.queue_message(response)

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
                print(e, flush=True)
                isDisconnected = True

            scheduledTime += 0.5
        sleepTime = scheduledTime - time.time()
        if sleepTime > 0:
            time.sleep(sleepTime)
        else:
            print('System slowing: it took {} seconds more than expected!'.format(-sleepTime), flush=True)
            scheduledTime = time.time()

    if not isDisconnected:
        ser.write(serMessFromStatus['F'].to_bytes(1, "big"))
    else:
        ser.write((0).to_bytes(1, "big"))
        ser.write((2).to_bytes(1, "big"))
    ser.close()

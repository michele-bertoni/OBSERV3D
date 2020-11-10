import requests
import os.path as path
import serial
import socket
import time

UPDATING_PATH = "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating"

DUET_HOST = "192.168.0.3"
REQUEST_URL = "http://{}/rr_status?type={}"

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


class SimpleLineProtocol:
    def __init__(self, soc):
        self.socket = soc
        self.buffer = b''
        self.NEWLINE = b'\r'

    def write(self, msg):
        msg = msg.strip()
        msg += '\r'
        self.socket.sendall(msg.encode())

    def read_lines(self):
        lines = []
        while self.NEWLINE not in self.buffer:
            try:
                d = self.socket.recv(1024)
            except Exception as sockException:
                if str(sockException) != 'timed out':
                    print(sockException)
                return lines
            if not d:
                return lines
            self.buffer = self.buffer + d

        while self.NEWLINE in self.buffer:
            i = self.buffer.find(self.NEWLINE)
            line = self.buffer[:i]
            self.buffer = self.buffer[i:].lstrip()
            lines.append(line.decode("utf-8"))
        return lines


if __name__ == "__main__":
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
                    #TODO: handle message
                    print(m)
                response = requests.get(REQUEST_URL.format(DUET_HOST, statusType)).json()
                #TODO: handle status
                status = response['status']
                if status == 'I':
                    ser.write(serMessFromStatus[status].to_bytes(1, "big"))
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

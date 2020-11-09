import requests
import os.path as path
import serial
import time

UPDATING_PATH = "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating"

REQUEST_URL = "http://192.168.0.3/rr_status?type={}"

SERIAL_PORT = "/dev/ttyUSB0"
BAUDRATE = 2400

if __name__ == "__main__":
    ser = serial.Serial(port=SERIAL_PORT, baudrate=BAUDRATE, timeout=.1)
    scheduledTime = time.time()
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

    while not path.exists(UPDATING_PATH):
        if time.time()>=scheduledTime:
            try:
                response = requests.get(REQUEST_URL.format(statusType)).json()
                status = response['status']
                if status == 'I':
                    ser.write((128).to_bytes(1, "big"))
                elif status == 'P':
                    statusType = 3

                if status != 'P':
                    statusType = 1
            except Exception:
                pass

            scheduledTime += 0.5
        sleepTime = scheduledTime - time.time()
        if sleepTime > 0:
            time.sleep(sleepTime)

    ser.write((136).to_bytes(1, "big"))
    ser.close()

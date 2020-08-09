#!/bin/bash
#
# Waits for a button press and then switches off Raspberry Pi
#
# Momentary switch is connected between physical pins 5 and 6 on
# the GPIO header
#
# Script lives at:
#
# /home/pi/Printy-McPrintface/Raspberry/switch-off
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/switch-off/wait-for-switch-off-button.sh
#
# All output goes to stdout and log file

# From http://stackoverflow.com/questions/3173131

# Make a copy of last log into log.old
cat > /home/pi/Printy-McPrintface/Raspberry/switch-off/log.old.txt
echo "`cat /home/pi/Printy-McPrintface/Raspberry/switch-off/log.txt`" > /home/pi/Printy-McPrintface/Raspberry/switch-off/log.old.txt

# Redirect stdout to a named pipe running tee replacing log file
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/switch-off/log.txt)

# Redirect stderr to stdout
exec 2>&1

echo Reboot at `date` - waiting... 

# Execute inline python code
sudo python - <<END
import time
import RPi.GPIO as gpio
# Set pin numbering to board numbering
gpio.setmode(gpio.BCM)
# Set up pin 3 as an input
# enable pullups
# physical pin 5 - short with physical pin 6 (gnd) to reboot)
gpio.setup(3, gpio.IN, pull_up_down=gpio.PUD_UP)
print "Python: Waiting for 2 second button press..."
# Code adapted from
# http://raspberrypi.stackexchange.com/questions/13866/
buttonReleased = True
while buttonReleased:
    gpio.wait_for_edge(3, gpio.FALLING)
    # button has been pressed
    buttonReleased = False
    for i in range(20):
        time.sleep(0.1)
        if gpio.input(3):
            buttonReleased = True
            break
gpio.cleanup()
print "Python: Button pressed"
END

echo Done waiting at `date`. Shutting down system...
sudo shutdown 0

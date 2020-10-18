#!/bin/bash

###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

# /home/pi/Printy-McPrintface/Raspberry/webserver
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/ping-lan/ping-lan.sh
#
# Move last log to log.old
mv /home/pi/Printy-McPrintface/Raspberry/ping-lan/log.txt /home/pi/Printy-McPrintface/Raspberry/ping-lan/log.old.txt

# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/ping-lan/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Verbose mode
set -x

# Wait 20 seconds
sleep 20

# Ping 10 times an always on device
ping -c 10 192.168.1.1

# If none of the pings was successfull, reboot
if $? then
  sudo reboot
fi

#!/bin/bash
#
# /home/pi/Printy-McPrintface/Raspberry/webserver
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/webserver/webserver.sh
#
# Move last log to log.old
mv /home/pi/Printy-McPrintface/Raspberry/webserver/log.txt /home/pi/Printy-McPrintface/Raspberry/webserver/log.old.txt

# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/webserver/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Verbose mode
set -x

# Go to Videos directory
cd /home/pi/Videos
sudo python3 -m http.server 80

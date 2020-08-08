#!/bin/bash
#
# /home/pi/Printy-McPrintface/Raspberry/git-pull
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/git-pull/git-pull.sh
#
# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
#!exec > >(tee -a /home/pi/Printy-McPrintface/Raspberry/git-pull/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Go to home directory
cd /home/pi

cloned = 0
# If ~/Printy-McPrintface directory does not exist, clone the repository
while [ ! -d "/home/pi/Printy-McPrintface" ] 
do
    git clone https://github.com/michele-bertoni/Printy-McPrintface
    cloned = 1
done

# Move to repository root directory and pull changes
cd Printy-McPrintface/
pulled = -1
sleepTime = 1
while [ $cloned==0 ] && [ $pulled<0 ]
do
	pull=`git pull`
	if [[ $pull == *"Already up-to-date."* ]]; then
		echo Already up-to-date.
		pulled = 0
	elif [[ $pull == *"file changed,"* ]]; then
		echo Pulled successfully
		pulled = 1
	elif [[ $pull == *"fatal"* ]]; then
		echo Unable to connect to GitHub, retry
		sleep 1
	else; then
		echo Unknown error
		sleep $sleepTime
		sleepTime = $sleepTime*2
	fi
done

if [pulled == 1] || [cloned == 1]; then
	cd /home/pi
	arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old Printy-McPrintface/Arduino/printy.ino
	arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old Printy-McPrintface/Arduino/printy.ino  
fi

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
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/git-pull/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Verbose mode
set -x

# Go to home directory
cd /home/pi

cloned=0
sleepTime=0
# If ~/Printy-McPrintface directory does not exist, clone the repository
while [ ! -d "/home/pi/Printy-McPrintface" ]
do
	sleep $sleepTime
	git clone https://github.com/michele-bertoni/Printy-McPrintface
   	cloned=1
   	sleepTime=$(($sleepTime+1))
done

# Move to repository root directory and pull changes
cd Printy-McPrintface/
sleepTime=1
while [ "$cloned" -eq 0 ]
do
	pull=`git pull`
	if [[ "$?" -eq 0 ]]; then
		if [[ "$pull" =~ "up to date" ]]; then
			echo No changes
			cloned=-1
		else
			echo Updated
			cloned=1
	fi
	else
		sleep $sleepTime
		sleepTime=$(($sleepTime*2))
	fi
done

if [ "$cloned" -eq 1 ] || [ "`cat /home/pi/Printy-McPrintface/Raspberry/git-pull/lastUpload.txt`" -ne "0" ]; then
	cd /home/pi
	arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old Printy-McPrintface/Arduino/printy.ino
	arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old Printy-McPrintface/Arduino/printy.ino
	echo "$?" > /home/pi/Printy-McPrintface/Raspberry/git-pull/lastUpload.txt
fi

#!/bin/bash
#
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.
# You can find a copy of the license inside the LICENSE file you received with this code 
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)
#
# /home/pi/Printy-McPrintface/Raspberry/git-pull
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/git-pull/git-pull.sh
#
# Move last log to log.old
mv /home/pi/Printy-McPrintface/Raspberry/git-pull/log.txt /home/pi/Printy-McPrintface/Raspberry/git-pull/log.old.txt

# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/git-pull/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Verbose mode
set -x

# Reset possibly leftover locks
rm /home/pi/Printy-McPrintface/Raspberry/git-pull/.updating

# If argv -f is present force compile and upload
for i in "$@"
do
	if [ "$i" -eq "-f" ]; then
		rm /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile
		rm /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload
	fi
done

# Go to home directory
cd /home/pi

cloned=0
sleepTime=0
# If ~/Printy-McPrintface directory does not exist, clone the repository
while [ ! -d "/home/pi/Printy-McPrintface/.git" ]
do
	sleep $sleepTime
	git clone https://github.com/michele-bertoni/Printy-McPrintface
   	cloned=1
   	sleepTime=$(($sleepTime+1))
done

# Move to repository root directory and pull changes
cd Printy-McPrintface/
sleepTime=0
while [ "$cloned" -eq 0 ]
do
	pull=`git pull`
	# If pull was successfull
	if [[ "$?" -eq 0 ]]; then
		# If pull did not change anything
		if [[ "$pull" =~ "up to date" ]]; then
			echo No changes
			cloned=-1
		# Otherwise if some files changed
		else
			echo Updated
			cloned=1
	fi
	# Otherwise retry
	else
		sleep 2
		sleepTime=$(($sleepTime+2))
		if [[ "$sleepTime" -gt 60 ]]; then
			echo Repository unreachable
			cloned=-1
		fi
	fi
done

cd /home/pi
compiled=-1

# Compile if a new version is available, if it's the first time the code is compiled or if last compile was unsuccessfull
if [ ! -f "/home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile" ]; then
	touch /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile
	echo "2" > /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile
fi
if [ "$cloned" -eq 1 ] || [ "`cat /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile`" -ne "0" ]; then
	echo "3" > /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile
	/home/pi/bin/arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old Printy-McPrintface/Arduino/printy.ino
	echo "$?" > /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastCompile
	compiled=$?
fi

# Upload if a new version is available, if it's the first time the code is uploaded or if last upload was unsuccessfull
if [ ! -f "/home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload" ]; then
        touch /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload
        echo "2" > /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload
fi
if [ "$compiled" -eq 0 ] || [ "`cat /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload`" -ne "0" ]; then
	echo "3" > /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload
	sleepTime=0
	touch /home/pi/Printy-McPrintface/Raspberry/git-pull/.updating
	while [ -f "/home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen" ] && [ "$sleepTime" -lt 60 ]
	do
		sleep 1
		sleepTime=$(($sleepTime+1))
	done
	/home/pi/bin/arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old Printy-McPrintface/Arduino/printy.ino
	echo "$?" > /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload
	rm /home/pi/Printy-McPrintface/Raspberry/git-pull/.updating
fi

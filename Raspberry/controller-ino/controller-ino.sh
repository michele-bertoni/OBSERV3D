#!/bin/bash

###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

# /home/pi/Printy-McPrintface/Raspberry/controller-ino
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/controller-ino/controller-ino.sh
#
# Move last log to log.old
mv /home/pi/Printy-McPrintface/Raspberry/controller-ino/log.txt /home/pi/Printy-McPrintface/Raspberry/controller-ino/log.old.txt

# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/controller-ino/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Reset possibly leftover locks
rm /home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen

# Main loop
updated=0
while [[ "1" -eq "1" ]]; do
	if [ ! -f "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating" ]; then
		if [ "$updated" -ne 0 ]; then
			if [ "`cat /home/pi/Printy-McPrintface/Raspberry/git-pull/.lastUpload`" -eq "0" ]; then
				echo "Code uploaded successfully."
			else
				echo "Upload failed."
			fi
		fi
		touch /home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen
		echo "Running Python code..."
		# Run Python code
		# TODO
		#
		# The following code simulates the actual code:
		while [ ! -f "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating" ]; do
			sleep 1
		done
		# End of the simulated code
		#
		rm /home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen
		echo "Updating Arduino..."
		updated=1
	fi
	sleep 1
done

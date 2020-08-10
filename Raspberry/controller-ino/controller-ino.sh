#!/bin/bash
#
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

# Verbose mode
set -x

# Reset possibly leftover locks
rm /home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen

# Main loop
while [ "1" -eq "1" ]
do
	if [ ! -f "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating" ]; then
		touch /home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen
		# Run Python code
		# TODO
		#
		# The following code simulates the actual code:
		while [ ! -f "/home/pi/Printy-McPrintface/Raspberry/git-pull/.updating" ]
		do
			echo "I'm using Arduino serial"
			sleep 1
		done
		# End of the simulated code
		#
		rm /home/pi/Printy-McPrintface/Raspberry/controller-ino/.serialOpen
	fi
	sleep 1
done

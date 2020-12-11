#!/bin/bash

###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

# /home/pi/Printy-McPrintface/Raspberry/plot-handler
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/plot-handler/plot-handler.sh
#
# Move last log to log.old
mv /home/pi/Printy-McPrintface/Raspberry/plot-handler/log.txt /home/pi/Printy-McPrintface/Raspberry/plot-handler/log.old.txt

# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/plot-handler/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Reset possibly leftover locks
rm /home/pi/Printy-McPrintface/Raspberry/.downloads/heightmap.csv
rm /home/pi/Printy-McPrintface/Raspberry/.downloads/heightmap.png
rm /home/pi/Printy-McPrintface/Raspberry/.downloads/heightmap.mp4
rm /home/pi/Printy-McPrintface/Raspberry/.downloads/heightmap.json

# Move to source directory
cd /home/pi/Printy-McPrintface/Raspberry/plot-handler/

# Main loop
echo "Wait for heightmap.csv"
while true; do
	if [[ -f "/home/pi/Printy-McPrintface/Raspberry/.downloads/heightmap.csv" ]]; then
		echo "heightmap.csv found: running Python code..."
		# Run Python code
		python3 /home/pi/Printy-McPrintface/Raspberry/plot-handler/plot-handler.py

        echo "Heightmap plot generated, removing heightmap.csv"
		rm /home/pi/Printy-McPrintface/Raspberry/.downloads/heightmap.csv

		echo "Wait for heightmap.csv"
	else
	    sleep 1
	fi
done

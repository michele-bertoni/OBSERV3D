#!/bin/bash

###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

# /home/pi/Printy-McPrintface/Raspberry/telegram-bot
#
# This script is added to crontab (with crontab -e) as follows:
#
# @reboot /home/pi/Printy-McPrintface/Raspberry/telegram-bot/telegram-bot.sh
#
# Move last log to log.old
mv /home/pi/Printy-McPrintface/Raspberry/telegram-bot/log.txt /home/pi/Printy-McPrintface/Raspberry/telegram-bot/log.old.txt

# All output goes to stdout and log file1
#
# Redirect stdout to a named pipe running tee with append
exec > >(tee /home/pi/Printy-McPrintface/Raspberry/telegram-bot/log.txt)

# Redirect stderr to stdout
exec 2>&1

# Verbose mode
set -x

# Print date and time
date +"Date : %d/%m/%Y Time : %H.%M.%S"

# Wait 20 second for net to be established
sleep 20

python3 /home/pi/Printy-McPrintface/Raspberry/telegram-bot/telegram-bot.py

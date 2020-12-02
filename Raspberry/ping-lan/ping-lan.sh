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

# Enable eth0 interface
sudo ifconfig eth0 up

# Print date and time
date +"Date : %d/%m/%Y Time : %H.%M.%S"

# Wait 10 seconds
echo "Wait 20 seconds"
echo
sleep 20

c=0
ok=0
while true; do
  #date +"Date : %d/%m/%Y Time : %H.%M.%S"

  # Ping 3 times extender router
  ping -c 3 "$(cat /home/pi/Printy-McPrintface/Raspberry/.config/router_ext_ip.conf)" > /dev/null

  # If at least 1 ping was successful
  if [[ "$?" -eq "0" ]]; then
    # Ping 3 times the gateway
    ping -c 3 "$(cat /home/pi/Printy-McPrintface/Raspberry/.config/gateway_ip.conf)" > /dev/null

    # If at least 1 ping was successful
    if [[ "$?" -eq "0" ]]; then
      ping -c 2 "$(cat /home/pi/Printy-McPrintface/Raspberry/.config/wan_ip.conf)" > /dev/null

      # If at least 1 ping was successful
      if [[ "$?" -eq "0" ]]; then
        if [[ "$ok" -eq "0" ]]; then
          ok=1
          date +"Date : %d/%m/%Y Time : %H.%M.%S"
          echo "Connection established"
          echo
        fi
      # No pings to wan were successful
      else
        ok=0
        date +"Date : %d/%m/%Y Time : %H.%M.%S"
        echo "Can't ping WAN"
        echo
      fi
    # No pings to the gateway were successful
    else
      ok=0
      date +"Date : %d/%m/%Y Time : %H.%M.%S"
      echo "Extender disconnected from gateway, need reboot"
      echo
    fi
  # No pings to the extender were successful
  else
    c=$((c+1))
    ok=0
    if [[ "$c" -lt "3" ]]; then
      date +"Date : %d/%m/%Y Time : %H.%M.%S"
      echo "Extender unreachable, waiting"
      echo
      sleep 10
    elif [[ "$c" -eq "3" ]]; then
      date +"Date : %d/%m/%Y Time : %H.%M.%S"
      echo "Extender unreachable, rebooting"
      echo
      sudo reboot
    fi
  fi

  sleep 5
done

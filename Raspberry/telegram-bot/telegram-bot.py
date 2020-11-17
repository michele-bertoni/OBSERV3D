###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import time

import requests
import telebot

import authentication as auth

TOKEN = ""
try:
    with open('token.txt') as f:
        line = f.readline()
        TOKEN = line.split('\n')[0]
except IOError:
    print('API token not found')

COMMANDS = '/start', '/help', '/login', '/logout', 'snap'
pending_auth = {}

conf_path = "/home/pi/Printy-McPrintface/Raspberry/.conf/"
duet_ip_conf_path = conf_path + "duet_ip.conf"
motion_ip_conf_path = conf_path + "motion_ip.conf"
motion_files_conf_path = conf_path + "motion_snap_path.conf"

duet_ip = '192.168.0.3'
try:
    with open(duet_ip_conf_path, 'r') as f:
        duet_ip = f.readline().rstrip('\n\r')
except Exception as e:
    print(e)

motion_ip = '127.0.0.1:8080'
try:
    with open(motion_ip_conf_path, 'r') as f:
        motion_ip = f.readline().rstrip('\n\r')
except Exception as e:
    print(e)

motion_lastSnap_path = "/var/lib/motion/lastSnap.jpg"
try:
    with open(motion_files_conf_path, 'r') as f:
        motion_lastSnap_path = f.readline().rstrip('\n\r')
except Exception as e:
    print(e)

send_gcode = 'http://{}/rr_gcode?gcode='.format('duet_ip')
send_request_snapshot = "http://{}/0/action/snapshot".format(motion_ip)

def listener(messages):
    """
    When new messages arrive TeleBot will call this function.
    """
    for message in messages:
        if (message.content_type != 'text') & (message.text not in COMMANDS):
            # print the sent message to the console
            print(str(message.chat.first_name) + " " + str(message.chat.last_name) + " [" + str(message.chat.id) + "]: " + message.text)
            bot.send_message(message.chat.id, "Invalid message")


bot = telebot.TeleBot(TOKEN)
bot.set_update_listener(listener)


@bot.message_handler(commands=['start'])
def send_welcome(message):
    bot.reply_to(message, "Welcome to Printy McPrintface Bot")
    send_help(message)


@bot.message_handler(commands=['login'])
def register(message):
    if auth.authentication(message.chat.id):
        bot.send_message(message.chat.id, "Already authenticated")
        return

    if message.chat.id in pending_auth:
        bot.send_message(message.chat.id, "Other authentication pending")
        return

    reply_text = "Send me the OTP displayed by the printer \n({} seconds left)"

    i = 60
    reply = bot.send_message(message.chat.id, reply_text.format(i))

    pending_auth[message.chat.id] = auth.generate_otp()
    requests.get(
        send_gcode+'M291%20R%22Telegram%20Authentication%20{}%20{}%22%20P%22{}%22%20T100%20S1'.format(
            message.chat.first_name, message.chat.last_name, pending_auth[message.chat.id]))

    t = time.time()

    while i>0 and not auth.authentication(message.chat.id):
        s = 60-i-(time.time()-t)
        if s>0:
            time.sleep(s)
        i = i-1
        bot.edit_message_text(chat_id=reply.chat.id, message_id=reply.message_id,
                              text=reply_text.format(i))

    requests.get(send_gcode + 'M292')
    try:
        del pending_auth[message.chat.id]
    except KeyError:
        pass


@bot.message_handler(func=lambda message: message.chat.id in pending_auth)
def msg_image_select(message):
    if auth.authentication(message.chat.id):
        bot.send_message(message.chat.id, "Already authenticated")
        return
    try:
        if message.chat.id in pending_auth and pending_auth[message.chat.id] == message.text:
            bot.send_message(message.chat.id, "Authenticated")
            auth.register(message.chat.id)
        else:
            bot.send_message(message.chat.id, "Authentication failed, try again")
    except KeyError:
        pass


@bot.message_handler(commands=['logout'])
def unregister(message):
    if not auth.authentication(message.chat.id):
        bot.send_message(message.chat.id, "Authentication failed: /login")
        return

    if auth.unregister(message.chat.id) and not auth.authentication(message.chat.id):
        bot.send_message(message.chat.id, "Unregistered correctly")
    else:
        bot.send_message(message.chat.id, "De-authentication failed")


@bot.message_handler(commands=['snap'])
def send_snapshot(message):
    if not auth.authentication(message.chat.id):
        bot.send_message(message.chat.id, "Authentication failed: /login")
        return

    try:
        requests.get(send_request_snapshot)
        time.sleep(1)

        with open(motion_lastSnap_path, 'rb') as snap:
            bot.send_photo(message.chat.id, snap)
    except Exception as exc:
        print(exc)
        bot.send_message(message.chat.id, "Unable to send snapshot: " + str(exc))


@bot.message_handler(commands=['help'])
def send_help(message):
    bot.send_message(message.chat.id,

                     "/login - Authenticate your Telegram account\n" +
                     "/logout - Unregister your Telegram account\n" +
                     "/snap - Request live snapshot of the printer")


if __name__ == "__main__":
    print("Printy McPrintface Telegram Bot listening...")
    bot.polling()

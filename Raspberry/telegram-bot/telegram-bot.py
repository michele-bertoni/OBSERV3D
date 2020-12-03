###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import time
import socket
import requests
import telebot
import threading

import colorsys

import authentication as auth
from connections_server import SocketServerLineProtocol

TOKEN = ""
try:
    with open('token.txt') as f:
        line = f.readline()
        TOKEN = line.split('\n')[0]
except IOError:
    print('API token not found', flush=True)

pending_auth = {}

_next_step_lock = threading.Lock()
next_step = {}

def get_next_step(chat_id):
    with _next_step_lock:
        return next_step.get(chat_id, "")

def set_next_step(chat_id, func_name):
    with _next_step_lock:
        next_step[chat_id] = func_name

def reset_if_next_step(chat_id, curr_func_name):
    with _next_step_lock:
        if next_step.get(chat_id, "") == curr_func_name:
            del next_step[chat_id]


pending_controller_message = {}

conf_path = "/home/pi/Printy-McPrintface/Raspberry/.config/"
duet_ip_conf_path = conf_path + "duet_ip.conf"
motion_ip_conf_path = conf_path + "motion_ip.conf"
socket_port_conf_path = conf_path + "telegram-bot_socket_port.conf"
motion_files_conf_path = conf_path + "motion_snap_path.conf"

duet_ip = '192.168.0.3'
try:
    with open(duet_ip_conf_path, 'r') as f:
        duet_ip = f.readline().rstrip('\n\r')
except Exception as e:
    print(e, flush=True)

motion_ip = '127.0.0.1:8080'
try:
    with open(motion_ip_conf_path, 'r') as f:
        motion_ip = f.readline().rstrip('\n\r')
except Exception as e:
    print(e, flush=True)

socket_port = 6126
try:
    with open(socket_port_conf_path, 'r') as f:
        socket_port = int(f.readline().rstrip('\n\r'))
except Exception as e:
    print(e, flush=True)

motion_lastSnap_path = "/var/lib/motion/lastSnap.jpg"
try:
    with open(motion_files_conf_path, 'r') as f:
        motion_lastSnap_path = f.readline().rstrip('\n\r')
except Exception as e:
    print(e, flush=True)

send_gcode = 'http://{}/rr_gcode?gcode='.format(duet_ip)
send_request_snapshot = "http://{}/0/action/snapshot".format(motion_ip)

controller_variables = {
    'lights': ('*Lights', [0, 1]),
    'chamber_lights': ('chamberLights', [0, 1]),
    'extruder_lights': ('extruderLights', [0, 1]),
    'hue': ('*_hue', [0, 1, 5, 10]),
    'saturation': ('*_saturation', [0, 1, 5, 10]),
    'value': ('*_value', [0, 1, 5, 10]),
    'chamber_hue': ('chamber_hue', [0, 1, 5, 10]),
    'chamber_saturation': ('chamber_saturation', [0, 1, 5, 10]),
    'chamber_value': ('chamber_value', [0, 1, 5, 10]),
    'extruder_hue': ('extruder_hue', [0, 1, 5, 10]),
    'extruder_saturation': ('extruder_saturation', [0, 1, 5, 10]),
    'extruder_value': ('extruder_value', [0, 1, 5, 10]),
    'effect_duration': ('effectDuration', [0, 1, 5, 10]),
    'fading_duration': ('fadingDuration', [0, 1, 3, 5]),
    'lights_mode': ('lightsMode', [0, 1, 5, 10]),
    'fading_mode': ('fadingMode', [0, 1, 5, 10])
}

controller_functions = {
    'lights_off': '*LightsOff',
    'lights_on': '*LightsOn',
    'chamber_lights_off': 'chamberLightsOff',
    'chamber_lights_on': 'chamberLightsOn',
    'extruder_lights_off': 'extruderLightsOff',
    'extruder_lights_on': 'extruderLightsOn',
    'revert_lights': 'revertLights',
    'reset_settings': 'resetSettings',
    'load_settings': 'loadSettings',
    'store_setting': 'storeSettings',
    'schedule_reboot': 'scheduleReboot',
    'unschedule_reboot': 'unScheduleReboot',
    'schedule_shutdown': 'scheduleShutdown',
    'unschedule_shutdown': 'unScheduleShutdown',
    'keep_raspberry_on': 'keepRaspberryOn',
    'keep_raspberry_off': 'keepRaspberryOff',
    'update': 'update'
}

sock = socket.create_server(('127.0.0.1', socket_port))
conn = None

bot = telebot.TeleBot(TOKEN)

@bot.message_handler(commands=['start'])
def send_welcome(message):
    bot.reply_to(message, "Welcome to Printy McPrintface Bot")
    send_help(message)


@bot.message_handler(commands=['login'])
def register(message):
    if auth.authentication(message.chat.id):
        bot.reply_to(message, "Already authenticated")
        return

    if message.chat.id in pending_auth:
        bot.reply_to(message, "Other authentication pending")
        return

    reply_text = "Send me the OTP displayed by the printer \n({} seconds left)"

    i = 60
    reply = bot.reply_to(message, reply_text.format(i))

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
        bot.reply_to(message, "Already authenticated")
        return
    try:
        if message.chat.id in pending_auth and pending_auth[message.chat.id] == message.text:
            bot.reply_to(message, "Authenticated")
            auth.register(message.chat.id)
        else:
            bot.reply_to(message, "Authentication failed, try again")
    except KeyError:
        pass


@bot.message_handler(commands=['logout'])
def unregister(message):
    if not auth.authentication(message.chat.id):
        bot.reply_to(message, "Authentication failed: /login")
        return

    if auth.unregister(message.chat.id) and not auth.authentication(message.chat.id):
        bot.reply_to(message, "Unregistered correctly")
    else:
        bot.reply_to(message, "De-authentication failed")


@bot.message_handler(commands=['snap'])
def send_snapshot(message):
    if not auth.authentication(message.chat.id):
        bot.reply_to(message, "Authentication failed: /login")
        return

    try:
        requests.get(send_request_snapshot)
        time.sleep(1)

        with open(motion_lastSnap_path+'lastsnap.jpg', 'rb') as snap:
            bot.send_photo(message.chat.id, snap)
    except Exception as exc:
        print(exc, flush=True)
        bot.reply_to(message, "Unable to send snapshot: " + str(exc))

@bot.message_handler(regexp="^#[0-9a-fA-F]{6}([ ]{1}[a-z]*)?$")
def color(message):
    if not auth.authentication(message.chat.id):
        bot.send_message(message.chat.id, "Authentication failed: /login")
        return

    commands = message.text.split(' ')
    hsv_color = colorsys.rgb_to_hsv(int(commands[0][1:3], 16), int(commands[0][3:5], 16), int(commands[0][5:7], 16))
    hsv_color = (round(hsv_color[0]*255), round(hsv_color[1]*255), round(hsv_color[2]))
    lights = '*'
    if len(commands)==2 and (commands[1] == 'chamber' or commands[1] == 'extruder'):
        lights = commands[1]

    if lights == '*':
        conn.write('{}_hue:={}, {}_saturation:={}'.format(lights, hsv_color[0],
                                                          lights, hsv_color[1]))
    else:
        conn.write('{}_hue:={}, {}_saturation:={}, {}_value:={}'.format(lights, hsv_color[0],
                                                                        lights, hsv_color[1],
                                                                        lights, hsv_color[2]))
    response = conn.read_line()
    bot.reply_to(message, response)

@bot.message_handler(commands=[*controller_variables])
def handle_controller_variable(message):
    args = message.text.split(' ')
    if len(args) == 2:
        try:
            argument = controller_variables[args[1]][0]
        except KeyError:
            argument = args[1]
        conn.write('{}:={}'.format(controller_variables[args[0][1:]][0], argument))
        bot.reply_to(message, conn.read_line())
    elif len(args) == 1:
        conn.write(controller_variables[args[0][1:]][0])
        pending_controller_message[message.chat.id] = controller_variables[args[0][1:]][0]+'+={}, '+controller_variables[args[0][1:]][0]
        ask_increment(message, controller_variables[args[0][1:]][1])
    else:
        bot.reply_to(message, 'Too many arguments ({}); expected 0 or 1'.format(len(args)-1))

def ask_increment(message, increments=None):
    set_next_step(message.chat.id, 'get_increment')
    if increments is None:
        bot.send_message(message.chat.id, conn.read_line())
    else:
        markup = telebot.types.ReplyKeyboardMarkup()
        for n in increments:
            if n == 0:
                markup.row(telebot.types.KeyboardButton('EXIT'))
            else:
                markup.row(telebot.types.KeyboardButton(str(-n)),
                           telebot.types.KeyboardButton('+'+str(n)))
        bot.send_message(message.chat.id, conn.read_line().replace(', ', '\n'), reply_markup=markup)

@bot.message_handler(func=lambda message: get_next_step(message.chat.id)=='get_increment')
def get_increment(message):
    if message.text == 'EXIT':
        bot.send_message(message.chat.id, 'ok', reply_markup=telebot.types.ReplyKeyboardRemove(selective=False))
        reset_if_next_step(message.chat.id, 'get_increment')
        try:
            del pending_controller_message[message.chat.id]
        except KeyError:
            pass
        return
    conn.write(pending_controller_message.get(message.chat.id, 'last+={}, last').format(message.text))
    ask_increment(message)

@bot.message_handler(commands=controller_functions)
def handle_controller_function(message):
    conn.write(controller_functions[message.text[1:]])
    bot.reply_to(message, conn.read_line())

@bot.message_handler(commands=['backup'])
def backup_controller_variables(message):
    controller_msg = ''
    args = message.text.split(' ')
    for a in args[1:]:
        controller_msg += '{}^+=0, '.format(a)

    if len(controller_msg)>2:
        conn.write(controller_msg)
        bot.reply_to(message, conn.read_line())
    else:
        pending_controller_message[message.chat.id] = '{}^+=0'
        ask_variable(message, 'Which variable do you want to backup?')

def ask_variable(message, reply_text=''):
    set_next_step(message.chat.id, 'get_variable')
    markup = telebot.types.ReplyKeyboardMarkup()
    variables = [*controller_variables]
    i=0
    while i<len(variables):
        if i+5==len(variables):
            markup.row(telebot.types.KeyboardButton(variables[i]),
                       telebot.types.KeyboardButton(variables[i+1]),
                       telebot.types.KeyboardButton(variables[i+2]),
                       telebot.types.KeyboardButton(variables[i+3]),
                       telebot.types.KeyboardButton(variables[i+4]))
            i+=5
        elif i+4==len(variables):
            markup.row(telebot.types.KeyboardButton(variables[i]),
                       telebot.types.KeyboardButton(variables[i+1]),
                       telebot.types.KeyboardButton(variables[i+2]),
                       telebot.types.KeyboardButton(variables[i+3]))
            i+=4
        else:
            markup.row(telebot.types.KeyboardButton(variables[i]),
                       telebot.types.KeyboardButton(variables[i+1]),
                       telebot.types.KeyboardButton(variables[i+2]))
            i+=3
    bot.send_message(message.chat.id, reply_text, reply_markup=markup)

@bot.message_handler(func=lambda message: get_next_step(message.chat.id)=='get_variable')
def get_variable(message):
    reset_if_next_step(message.chat.id, 'get_variable')
    conn.write(pending_controller_message.get(message.chat.id, '{}').format(message.text))
    try:
        del pending_controller_message[message.chat.id]
    except KeyError:
        pass
    bot.send_message(message.chat.id, conn.read_line(), reply_markup=telebot.types.ReplyKeyboardRemove(selective=False))

@bot.message_handler(commands=['help'])
def send_help(message):
    bot.send_message(message.chat.id,
                    "/login - Authenticate your Telegram account\n" +
                    "/logout - Unregister your Telegram account\n" +
                    "/snap - Request live snapshot of the printer\n" +
                    "/update - Pull changes and update software\n" +
                    "#xxxxxx - Set led color to the given hex color\n" +
                    "#xxxxxx chamber - Set chamber led color\n" +
                    "#xxxxxx extruder - Set extruder led color\n" +
                    "Variables: (no args increase, 1 arg set to arg)\n"
                    "/lights - Set all lights on (1) or off (0)\n" +
                    "/chamber_lights - Set chamber lights on or off\n" +
                    "/extruder_lights - Set extruder lights on or off\n" +
                    "/hue - Set all lights hue\n" +
                    "/saturation - Set all lights saturation\n" +
                    "/value - Set all lights brightness\n" +
                    "/chamber_hue - Set chamber hue\n" +
                    "/chamber_saturation - Set chamber saturation\n" +
                    "/chamber_value - Set chamber brightness\n" +
                    "/extruder_hue - Set extruder hue\n" +
                    "/extruder_saturation - Set extruder saturation\n" +
                    "/extruder_value - Set extruder brightness\n" +
                    "/effect_duration - Set effect duration\n" +
                    "/fading_duration - Set fading duration\n" +
                    "/lights_mode - Set lights mode\n" +
                    "/fading_mode - Set fading mode\n" +
                    "Functions: (take 0 arguments)\n" +
                    "/lights_off - Switch all lights off\n" +
                    "/lights_on - Switch all lights on\n" +
                    "/chamber_lights_off - Switch chamber lights off\n" +
                    "/chamber_lights_on - Switch chamber lights on\n" +
                    "/extruder_lights_off - Switch extruder lights off\n" +
                    "/extruder_lights_on - Switch extruder lights on\n" +
                    "/revert_lights - Revert lights from backup\n" +
                    "/reset_settings - Reset settings to default\n" +
                    "/load_settings - Load stored settings\n" +
                    "/store_settings - Store current settings\n" +
                    "/schedule_reboot - Printy reboots when possible\n" +
                    "/unschedule_reboot - Unschedule reboot\n" +
                    "/schedule_shutdown - Switch off when possible\n" +
                    "/unschedule_shutdown - Unschefule shutdown\n" +
                    "/keep_raspberry_on - Keep Raspberry always on\n" +
                    "/keep_raspberry_off - RPi off when Printy is off\n" +
                    "Varargs functions: (take any number of arguments)\n"
                    "/backup - Backup variables for a future revert",
                     reply_markup=telebot.types.ReplyKeyboardRemove(selective=False))

# Default command handler. A lambda expression which always returns True is used for this purpose.
@bot.message_handler(func=lambda message: True, content_types=['audio', 'video', 'document', 'text', 'location', 'contact', 'sticker'])
def default_command(message):
    bot.reply_to(message, "Unknown message")
    send_help(message)



if __name__ == "__main__":
    print("Socket listening on 127.0.0.1:{}".format(socket_port))
    sock.listen()
    conn = SocketServerLineProtocol(sock.accept()[0])
    print("Printy McPrintface Telegram Bot listening...", flush=True)
    bot.polling()
    while True:
        time.sleep(1)

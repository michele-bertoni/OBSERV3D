###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

WHITELIST_FILE = 'whitelist.txt'

def authentication(chat_id):
    cid = str(chat_id)
    try:
        with open(WHITELIST_FILE) as f:
            lines = f.readlines()
            for line in lines:
                if line == cid + "\n":
                    return True
    except IOError:
        open(WHITELIST_FILE, 'a').close()
    return False


def register(chat_id):
    with open(WHITELIST_FILE, 'a') as f:
        f.write(str(chat_id) + "\n")


def unregister(chat_id):
    cid = str(chat_id)

    try:
        with open('new'+WHITELIST_FILE, 'w') as newf:
            with open(WHITELIST_FILE) as f:
                lines = f.readlines()
                for line in lines:
                    if line != cid + "\n":
                        newf.write(line)
    except IOError as err:
        print(err, flush=True)
        return False

    import os
    os.replace(src='new' + WHITELIST_FILE, dst=WHITELIST_FILE)
    return True


def generate_otp(length=8):
    import random
    import string
    letters_and_digits = string.ascii_letters + string.digits
    return ''.join((random.choice(letters_and_digits) for i in range(length)))

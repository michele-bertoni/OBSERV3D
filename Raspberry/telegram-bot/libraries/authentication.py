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
        print(err)
        return False

    import os
    os.replace(src='new' + WHITELIST_FILE, dst=WHITELIST_FILE)
    return True


def generate_otp(length=8):
    import random
    import string
    letters_and_digits = string.ascii_letters + string.digits
    return ''.join((random.choice(letters_and_digits) for i in range(length)))

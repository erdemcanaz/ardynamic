import random

import pyrebase
import time

FIREBASE_CONFIG = {
    "apiKey": "AIzaSyDKk7JBHuUSsTEXIVm56dkPJxFyCeCeSrA",
    "authDomain": "python-user-database-test.firebaseapp.com",
    "databaseURL": "https://python-user-database-test-default-rtdb.firebaseio.com",
    "projectId": "python-user-database-test",
    "storageBucket": "python-user-database-test.appspot.com",
    "messagingSenderId": "1083168668001",
    "appId": "1:1083168668001:web:bfb107fbb4a8abfe49ad20",
    "measurementId": "G-5L7MY34RR6"
}


##--------------
def current_date():  # year,month,day,hour,minute,seconds, miliseconds
    strings = time.strftime("%Y,%m,%d,%H,%M,%S") + "," + str(round((time.time() % 1) * 1000) % 1000)
    t = strings.split(',')
    k = 3 - len(t[6])
    s = ('0' * k)
    string_date = t[3] + ":" + t[4] + ":" + t[5] + ":" + t[6] + s + "-" + t[2] + "." + t[1] + "." + t[0]
    return string_date


def sign_up(email, password):
    global auth
    global when_token_is_received
    user = None
    try:
        user = auth.create_user_with_email_and_password(email, password)
        print(current_date() + "-(pyrebase)-[sign_up]-{signed up as (" + email + "," + password + ")}")
        when_token_is_received = time.time()
        return user
    except:
        print(current_date() + "-(pyrebase)-[sign_up]-{could not signed up as (" + email + "," + password + ")}")
        return user


def sign_in(email, password):
    global auth
    global when_token_is_received
    user = None
    try:
        user = auth.sign_in_with_email_and_password(email, password)
        print(current_date() + "-(pyrebase)-[sign_in]-{signed in as (" + email + "," + password + ")}")
        when_token_is_received = time.time()
        return user
    except:
        print(current_date() + "-(pyrebase)-[sign_up]-{could not signed in as (" + email + "," + password + ")}")
        return user


def if_needed_refresh_token(current_user, token_time_out_seconds):
    global when_token_is_received
    global auth
    if (when_token_is_received == None):
        print(current_date() + "-(pyrebase)-[refresh_token]-{There is no token received yet}")
        return False

    time_passed_since_last_token = time.time() - when_token_is_received
    if (time_passed_since_last_token < token_time_out_seconds):
        print(current_date() + "-(pyrebase)-[refresh_token]-{No need to refresh token. Seconds left:" + str(
            round(token_time_out_seconds - time_passed_since_last_token, 2)) + "}")
        return False

    auth.refresh(current_user['refreshToken'])  # refresh token

    when_token_is_received = time.time()
    print(current_date() + "-(pyrebase)-[refresh_token]-{token is refreshed for email:" + str(current_user['email']) + "}")


def keep_user_active(email_new, password_new, token_timeout):

    if (keep_user_active.VARIABLES[0] != None and time.time() - keep_user_active.VARIABLES[3] < 1): return keep_user_active.VARIABLES[0]
    keep_user_active.VARIABLES[3] = time.time()

    if (keep_user_active.VARIABLES[1] == None or keep_user_active.VARIABLES[1] != email_new):
        keep_user_active.VARIABLES[0] = None
        keep_user_active.VARIABLES[1] = email_new
        keep_user_active.VARIABLES[2] = password_new

    if (keep_user_active.VARIABLES[0] == None): keep_user_active.VARIABLES[0] = sign_in(keep_user_active.VARIABLES[1], keep_user_active.VARIABLES[2])
    if (keep_user_active.VARIABLES[0] == None): keep_user_active.VARIABLES[0] = sign_up(keep_user_active.VARIABLES[1], keep_user_active.VARIABLES[2])
    else:
        try:
            if_needed_refresh_token(keep_user_active.VARIABLES[0], token_timeout)
        except:
            keep_user_active.VARIABLES[0] = None

    return  keep_user_active.VARIABLES[0]

keep_user_active.VARIABLES = [None, None, None, 0] # User, email, password, last_time


def firebase_setup(firebaseConfig):
    global firebase_server
    global auth
    global data_base
    firebase_server = pyrebase.initialize_app(firebaseConfig)
    auth = firebase_server.auth()
    data_base = firebase_server.database()

def append_pyrebase_write_queue(data_name, data):
    global PYREBASE_DATA_QUEUE
    max_queue_limit = 500
    # DO NOT MESS
    if (len(PYREBASE_DATA_QUEUE) >= max_queue_limit):
        return False

    try_to_append_this = [data_name, data]
    if (try_to_append_this not in PYREBASE_DATA_QUEUE):
        PYREBASE_DATA_QUEUE.append(try_to_append_this)
        return True
    else:
        return False

# GLOBALS
firebase_server = None
auth = None
data_base = None
user = None
when_token_is_received = None
PYREBASE_DATA_QUEUE = []
# CODE
firebase_setup(FIREBASE_CONFIG)
while True:
    user = keep_user_active("ahmetcem@gmail.com", "12345678", 15)
    append_pyrebase_write_queue("erdem",1)
    time.sleep(3)

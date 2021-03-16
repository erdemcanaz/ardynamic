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
def keep_user_active(email_new, password_new, token_timeout,check_interval_seconds):

    if (keep_user_active.VARIABLES[0] != None and time.time() - keep_user_active.VARIABLES[3] < check_interval_seconds): return keep_user_active.VARIABLES[0]
    keep_user_active.VARIABLES[3] = time.time()

    if (keep_user_active.VARIABLES[1] == None or keep_user_active.VARIABLES[1] != email_new):
        if (keep_user_active.VARIABLES[0] != None):
            print(current_date() + "-(pyrebase)-[keep_user_active]-{new account email:" + str(email_new)+"}")
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

def push_from_queue_to_firebase(interval_seconds):
    global  data_base
    global  user
    global  recent_data_to_push

    time_passed = time.time()-push_from_queue_to_firebase.LAST_TIME
    if(time_passed<interval_seconds):
        return False
    push_from_queue_to_firebase.LAST_TIME = time.time()

    if(user == None): return False

    ID = user['localId']
    if(len(recent_data_to_push)==0): return False

    recent_data_to_push['timeStamp'] = time.time()
    #try:
    #data_base.child("test").child(ID).update({str(data_name):str(data)}, user['idToken'])
    data_base.child("test").child(ID).update(recent_data_to_push, user['idToken'])
    print(current_date() + "-(pyrebase)-[push_from_queue_to_firebase]-{data is pushed to the server}")
    return True
    #except:
    print(current_date() + "-(pyrebase)-[push_from_queue_to_firebase]-{an error occured while pushing}")
    user = None
    return False
push_from_queue_to_firebase.LAST_TIME = 0
#-----------------------------------------------------------------
# GLOBALS
firebase_server = None
auth = None
data_base = None
user = None
when_token_is_received = None
PYREBASE_DATA_QUEUE = []
recent_data_to_push = {}
# CODE

#EXAMPLE CODE BLOCK
firebase_setup(FIREBASE_CONFIG)
start = time.time()
while True:
    if( time.time()-start<20):
        user = keep_user_active("a@gmail.com", "12345678", 5,1)
    else:
        user = keep_user_active("b@gmail.com", "12345678", 60, 1)
    recent_data_to_push = {"A":random.random(),"B":random.random(),"C":random.random(),"D":random.random(),"E":random.random(),"F":random.random(),"G":random.random(),}
    push_from_queue_to_firebase(2)


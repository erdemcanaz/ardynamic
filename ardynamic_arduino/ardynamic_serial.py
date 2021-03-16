import time
import serial.tools.list_ports
import ardynamic


def current_date():  # year,month,day,hour,minute,seconds, miliseconds
    strings = time.strftime("%Y,%m,%d,%H,%M,%S") + "," + str(round((time.time() % 1) * 1000) % 1000)
    t = strings.split(',')
    k = 3 - len(t[6])
    s = ('0' * k)
    string_date = t[3] + ":" + t[4] + ":" + t[5] + ":" + t[6] + s + "-" + t[2] + "." + t[1] + "." + t[0]
    return string_date


def remove_unaccepted_characters(string):
    # harf, sayı, #, $, , "," , /
    return_string = ""
    for char in string:
        c1 = (char >= 'a' and char <= 'z') or (char >= 'A' and char <= 'z')
        c2 = (char >= '0' and char <= '9')
        c3 = char == '#' or char == '$' or char == ':' or char == ' ' or char == ',' or char == '/'
        if (c1 or c2 or c3):
            return_string += char
    return return_string


def write_to_port():
    # Instead of true or false you may return-> 0:None; 1:instruction has been sent;...
    write_to_port.LAST_TIME  # seconds from epoch
    global SERIAL
    global INSTRUCTION_QUEUE
    # SERIAL PROTOCOL: UART-SERIAL_8N1
    global write_wait_seconds_before_write # Do not write continuosuly, wait this much before another insturction
    baud_rate = 9600
    write_timeout = 0.2
    read_timeout = 0.2
    expected_acknowledgment_msg = "#YES,I AM HERE$"
    wait_seconds_acknowledgment = 0.05  # ask for acknowledgement, then wait slave to send all the bytes over the port
    wait_seconds_let_slave_configure_himself = 0.1  # After connecting to a port, wait this much
    delay_if_trying_to_connect = 1  # if SERIAL.isOpen() == False; wait this much until next attempt
    # INFORM
    global write_print_connection  # if port is opened
    global write_print_sent  # data writed to the port
    global write_print_error  # any kind of error
    global write_print_salute  # slave salutes you as

    # DO NOT MESS
    if (time.time() - write_to_port.LAST_TIME < write_wait_seconds_before_write):
        return False
    write_to_port.LAST_TIME = time.time()

    instruction = ""
    if (len(INSTRUCTION_QUEUE) == 0):
        return False
    instruction = INSTRUCTION_QUEUE[0]

    if (SERIAL.isOpen()):
        try:
            SERIAL.write(instruction.encode())
            if (write_print_sent): print(
                current_date() + "-(instruction \"" + instruction + "\" has been succesfully sent over \"" + str(
                    SERIAL.name) + "\")")
            INSTRUCTION_QUEUE.pop(0)  # remove this instruction from queue
            return True  # instruction has been sent
        except:
            if (write_print_error): print(current_date() + "-{communication has been lost during data transfer}")
            SERIAL.close()
            return False
    else:
        write_to_port.LAST_TIME = time.time() + delay_if_trying_to_connect
        comlist = serial.tools.list_ports.comports()
        if (len(comlist) == 0 and write_print_error): print(current_date() + "-(no port is available)")
        for ports in comlist:
            try:
                SERIAL = serial.Serial(port=ports.device, baudrate=baud_rate, parity=serial.PARITY_NONE,
                                       stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,
                                       write_timeout=write_timeout, timeout=read_timeout)
                time.sleep(wait_seconds_let_slave_configure_himself)
                SERIAL.reset_input_buffer()
                SERIAL.write(ardynamic.arduino_are_you_there().encode("utf-8"))
                time.sleep(wait_seconds_acknowledgment)
                reply_from_slave = ""
                if (SERIAL.in_waiting != 0):
                    while (SERIAL.in_waiting > 0):
                        reply_from_slave += SERIAL.read().decode("utf-8")
                    reply_from_slave = remove_unaccepted_characters(reply_from_slave)
                print(reply_from_slave)
                if (expected_acknowledgment_msg == reply_from_slave):
                    if (write_print_connection): print(
                        current_date() + "-(connected to port \"" + str(ports.device) + "\")")
                    if (write_print_salute):  print(
                        current_date() + "-(slave salutes you \"" + reply_from_slave + "\" port:\"" + str(
                            ports.device) + "\")")
                    return True
                else:
                    if (write_print_error): print(
                        current_date() + "-{this \"" + str(ports.device) + "\" device is not my slave!}")
                    SERIAL.close()
            except:
                if (write_print_error): print(current_date() + "-{could not connected to (" + str(ports.device) + ")}")
                SERIAL.close()
    return False


def read_from_port():
    global SERIAL
    read_from_port.LAST_TIME  # seconds from epoch
    wait_seconds_before_read = 0.01

    if (time.time() - write_to_port.LAST_TIME < wait_seconds_before_read):
        return False
    read_from_port.LAST_TIME = time.time()

    if (SERIAL.isOpen() == False):
        add_instruction_to_queue(ardynamic.arduino_are_you_there(), False)  # try to find arduino
        return False

    try:
        reading = ""
        if (SERIAL.in_waiting != 0):
            while (SERIAL.in_waiting > 0):
                reading += SERIAL.read().decode("utf-8")
            reading = remove_unaccepted_characters(reading)
        if (reading != ""):
            return reading
        else:
            return False
    except:
        return False


def add_instruction_to_queue(instruction, should_append_existing_instruction):
    global INSTRUCTION_QUEUE
    global queue_print_append
    add_instruction_to_queue.LAST_TIME
    wait_seconds_before_trying_to_append_it = 0.1
    max_queue_limit = 50
    # DO NOT MESS
    if (should_append_existing_instruction and time.time() - add_instruction_to_queue.LAST_TIME < wait_seconds_before_trying_to_append_it):
        return False
    add_instruction_to_queue.LAST_TIME = time.time()

    if (len(INSTRUCTION_QUEUE) >= max_queue_limit):
        return False

    if (should_append_existing_instruction == False):

        if (instruction not in INSTRUCTION_QUEUE):
            INSTRUCTION_QUEUE.append(instruction)
            if (queue_print_append): print(
                current_date() + "-[queue:" + str(len(INSTRUCTION_QUEUE)) + " instruction:" + instruction + "]")
        else:
            return False
    else:
        INSTRUCTION_QUEUE.append(instruction)
        if (queue_print_append): print(
            current_date() + "-[queue:" + str(len(INSTRUCTION_QUEUE)) + " instruction:" + instruction + "]")


# GLOBALS
INSTRUCTION_QUEUE = []
# INITIALS
SERIAL = serial.Serial()
write_to_port.LAST_TIME = time.time()
read_from_port.LAST_TIME = time.time()
add_instruction_to_queue.LAST_TIME = time.time()

# HEADER

write_wait_seconds_before_write = 0.05
write_print_connection = True  # if port is opened
write_print_sent = True  # data writed to the port
write_print_error = True  # any kind of error
write_print_salute = True  # slave salutes you as

queue_print_append = True

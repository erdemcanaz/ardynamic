import time
import serial.tools.list_ports
import ardynamic


def current_date():  # year,month,day,hour,minute,seconds
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


SERIAL = serial.Serial()


def write_to_port(instruction):
    # Instead of true or false you may return-> 0:None; 1:instruction has been sent;...
    write_to_port.LAST_TIME  # seconds from epoch
    global SERIAL
    # SERIAL PROTOCOL: UART-SERIAL_8N1
    baud_rate = 9600
    write_timeout = 0.2
    read_timeout = 0.2
    expected_acknowledgment_msg = "#YES,I AM HERE$"
    wait_seconds_acknowledgment = 0.05
    wait_seconds_before_write = 0.2
    wait_seconds_let_arduino_configure_himself = 0.1
    delay_if_trying_to_connect = 1
    print_connection = True
    print_sent = False
    print_error = True
    print_salute = False

    if (time.time() - write_to_port.LAST_TIME < wait_seconds_before_write):
        return False
    else:
        write_to_port.LAST_TIME = time.time()
    if (SERIAL.isOpen()):
        try:
            SERIAL.write(instruction.encode())
            if (print_sent): print(
                current_date() + "-(instruction \"" + instruction + "\" has been succesfully sent over \"" + str(
                    SERIAL.name) + "\")")
            return True  # data has been sent
        except:
            if (print_error): print(current_date() + "-{communication has been lost during data transfer}")
            SERIAL.close()
            return False
    else:
        write_to_port.LAST_TIME = time.time() + delay_if_trying_to_connect
        comlist = serial.tools.list_ports.comports()
        if (len(comlist) == 0 and print_error): print(current_date() + "-(no port is available)")
        for ports in comlist:
            try:
                SERIAL = serial.Serial(port=ports.device, baudrate=baud_rate, parity=serial.PARITY_NONE,
                                       stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,
                                       write_timeout=write_timeout, read_timeout=read_timeout)
                time.sleep(wait_seconds_let_arduino_configure_himself)
                SERIAL.write(ardynamic.arduino_are_you_there().encode("utf-8"))
                time.sleep(wait_seconds_acknowledgment)

                reply_from_slave = ""
                if (SERIAL.in_waiting != 0):
                    while (SERIAL.in_waiting > 0):
                        reply_from_slave += SERIAL.read().decode("utf-8")
                    reply_from_slave = remove_unaccepted_characters(reply_from_slave)

                print(reply_from_slave)
                if (expected_acknowledgment_msg == reply_from_slave):
                    if (print_connection): print(current_date() + "-(connected to port \"" + str(ports.device) + "\")")
                    if (print_salute):  print(
                        current_date() + "-(slave salutes you \"" + reply_from_slave + "\" port:\"" + str(
                            ports.device) + "\")")
                    return True
                else:
                    if (print_error): print(
                        current_date() + "-{this \"" + str(ports.device) + "\" device is not my slave!}")
                    SERIAL.close()
            except:
                if (print_error): print(current_date() + "-{could not connected to (" + str(ports.device) + ")}")
                SERIAL.close()
    return False


def read_from_port():
    global SERIAL
    read_from_port.LAST_TIME  # seconds from epoch
    wait_seconds_before_read = 0.05

    if (time.time() - write_to_port.LAST_TIME < wait_seconds_before_read):
        return False

    if (SERIAL.isOpen() == False):
        write_to_port(ardynamic.arduino_are_you_there())  # try to find arduino
        return False

    try:
        reading = ""
        if (SERIAL.in_waiting != 0):
            while (SERIAL.in_waiting > 0):
                reading += SERIAL.read().decode("utf-8")
            reading = remove_unaccepted_characters(reading)
        return reading
    except:
        pass
    return ""

##INITIALS
write_to_port.LAST_TIME = time.time()
read_from_port.LAST_TIME = time.time()

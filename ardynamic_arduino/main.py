import time

import serial.tools.list_ports
import ardynamic



def current_date():#year,month,day,hour,minute,seconds
    strings = time.strftime("%Y,%m,%d,%H,%M,%S")+","+str(round((time.time()%1)*1000)%1000)
    t = strings.split(',')
    k = 3-len(t[6])
    s = ('0'*k)
    string_date = t[3] + ":" + t[4] + ":" + t[5] + ":" + t[6]+s+ "-" + t[2] + "." + t[1] + "." + t[0]
    return string_date
def remove_unaccepted_characters(string):
    #harf, sayı, #, $, :
    return_string = ""
    for char in string:
        c1= (char>='a' and char<='z') or (char >='A' and char <='z')
        c2= (char>='0' and char<='9')
        c3= char == '#' or char =='$' or char ==':' or char ==' ' or char ==','
        if(c1 or c2 or c3):
            return_string +=char
    return return_string

SERIAL = serial.Serial()
def write_to_port(instruction):
    #0:None; 1:instruction has been sent; .........
    #seconds from epoch
    write_to_port.LAST_TIME
    global SERIAL
    wait_seconds_acknowledgment = 0.05
    expected_acknowledgment_msg = "#HELLO,ITS ME$"
    wait_seconds = 0.01
    print_succes = True
    print_error  = True
    print_reply  = True
    baud_rate = 9600

    if(time.time()-write_to_port.LAST_TIME<wait_seconds):
        return False
    else:
        write_to_port.LAST_TIME=time.time()
    if(SERIAL.isOpen()):
        try:
            SERIAL.write(instruction.encode())
            if(print_succes):print(current_date()+"-(instruction \""+instruction+"\" has been succesfully sent over \""+str(SERIAL.name)+"\")")
            return True # data has been sent
        except:
            if(print_error):print(current_date()+"-{communication has been lost during data transfer}")
            SERIAL.close()
            return False
    else:
        comlist = serial.tools.list_ports.comports()
        if(len(comlist)==0 and print_error): print(current_date() + "-(no port is available)")
        for ports in comlist:
            try:
                    SERIAL = serial.Serial(port=ports.device, baudrate=baud_rate, parity=serial.PARITY_ODD,stopbits=serial.STOPBITS_TWO, bytesize=serial.SEVENBITS, timeout=0.2)
                    #connection is established
                    SERIAL.write(ardynamic.arduino_is_that_you().encode("utf-8"))
                    time.sleep(wait_seconds_acknowledgment)
                    reply_from_slave = remove_unaccepted_characters(SERIAL.readline().decode("utf-8"))

                    if(expected_acknowledgment_msg == reply_from_slave):
                        if (print_succes): print(current_date() + "-(connected to port \"" +str(ports.device)+ "\")")
                        if (print_reply):  print(current_date() + "-(slave salutes you \""+reply_from_slave+"\" port:\"" +str(ports.device)+ "\")")
                        return True
                    else:
                        if (print_error): print(current_date() + "-{this \""+str(ports.device)+"\" device is not arduino}")
                        SERIAL.close()


            except:
                    if (print_error): print(current_date() + "-{could not connected to ("+str(ports.device)+")}")
                    SERIAL.close()

    return False
write_to_port.LAST_TIME = time.time()

def read_from_port(instruction):

    pass



#PROGRAM---------------------
while True:
    write_to_port(ardynamic.arduino_is_that_you())






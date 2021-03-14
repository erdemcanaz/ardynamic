import time

import serial.tools.list_ports
import ardynamic



def current_date():#year,month,day,hour,minute,seconds
    strings = time.strftime("%Y,%m,%d,%H,%M,%S")+","+str(round((time.time()%1)*1000))
    t = strings.split(',')
    string_date=t[3]+":"+t[4]+":"+t[5]+":"+t[6]+"-"+t[2]+"."+t[1]+"."+t[0]
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

def write_to_port(instruction):
    #0:None; 1:instruction has been sent; .........
    #seconds from epoch
    write_to_port.LAST_TIME
    global SERIAL
    wait_seconds_acknowledgment = 0.05
    expected_acknowledgment_msg = "#HELLO,ITS ME$"
    wait_seconds = 0.5
    print_succes = True
    print_error  = True
    print_reply  = True


    if(time.time()-write_to_port.LAST_TIME<wait_seconds):
        return False
    else:
        write_to_port.LAST_TIME=time.time()

    if(SERIAL.isOpen()):
        comlist = serial.tools.list_ports.comports()
        available_ports = []
        for ports in comlist:
            available_ports.append(ports.device)
        if(len(available_ports)!=1):
            if(print_error): print(current_date() + "-{more than one ports are available}")
            SERIAL.close()
            return False
        try:
            SERIAL.write(instruction.encode())
            if(print_succes):print(current_date()+"-(instruction \""+instruction+"\" has been succesfully sent over \""+str(SERIAL.name)+"\")")
            return True # data has been sent
        except:
            if(print_error):print(current_date()+"-{communication has been lost during data transfer}")
            SERIAL.close()
            return False
            pass
    else:
        comlist = serial.tools.list_ports.comports()
        available_ports = []
        for ports in comlist:
                available_ports.append(ports.device)

        if(len(available_ports) != 1):
            if (len(available_ports)==0 and print_error):print(current_date()+"-{there is no available port}")
            elif(print_error):print(current_date()+"-{more than one ports are available}")
            SERIAL.close()
            return False
        else:
            try:
                SERIAL = serial.Serial(port=available_ports[0], baudrate=9600, parity=serial.PARITY_ODD,stopbits=serial.STOPBITS_TWO, bytesize=serial.SEVENBITS)
                #connection is established
                SERIAL.write(ardynamic.arduino_is_that_you().encode("utf-8"))
                time.sleep(wait_seconds_acknowledgment)
                reply_from_slave = remove_unaccepted_characters(SERIAL.readline().decode("utf-8"))
                if(expected_acknowledgment_msg == reply_from_slave):
                    if (print_succes): print(current_date() + "-(connected to port \"" +str(available_ports[0])+ "\")")
                    if (print_reply):  print(current_date() + "-(slave salutes as \""+reply_from_slave+"\" port:\"" +str(available_ports[0])+ "\")")
                    return True
                else:
                    if (print_error): print(current_date() + "-{this \""+str(available_ports[0])+"\" device is not arduino}")
                    SERIAL.close()
                    return  False

            except:
                if (print_error): print(current_date() + "-{could not connected to ("+str(available_ports[0])+")}")
                SERIAL.close()
                return False

SERIAL = serial.Serial()
write_to_port.LAST_TIME = time.time()

#PROGRAM---------------------
while True:
    write_to_port(ardynamic.arduino_is_that_you())



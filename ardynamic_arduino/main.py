

import serial.tools.list_ports


ser = serial.Serial()
comlist = serial.tools.list_ports.comports()
available_ports = []
while True:
    comlist = serial.tools.list_ports.comports()
    for element in comlist:
        available_ports.append(element.device)
    if (len(available_ports)==1):break
ser = serial.Serial(port=available_ports[0],  baudrate=9600,  parity=serial.PARITY_ODD,  stopbits=serial.STOPBITS_TWO,  bytesize=serial.SEVENBITS)
print (ser.isOpen())
ser.close()
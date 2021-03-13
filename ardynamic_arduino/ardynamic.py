
def to_str(integer, length):
    return_str=""
    number_of_additional_zeros= length-len(str(integer))
    return_str =  "0"*number_of_additional_zeros+ str(integer)
    return return_str

def pin_mode(pin_no, mode):
    if(mode == "INPUT"):
        return  "#001:003:"+to_str(pin_no,3)+"$"
    elif(mode=="INPUT_PULLUP"):
        return  "#002:003:"+to_str(pin_no,3)+"$"
    elif(mode =="OUTPUT"):
        return None
    else:
        print("ERROR:given pin mode (" + str(mode) + ") is not defined")
        return None

def digital_read(pin_no,period):
    return "#004:008:" + to_str(pin_no, 3) +  to_str(period,5)+ "$"

def digital_write(pin_no, mode):
    if(mode=="HIGH" or mode == 1):
        return "#005:004:"+to_str(pin_no,3)+"1$"
    elif(mode =="LOW" or mode==0):
        return "#005:004:"+to_str(pin_no,3)+"0$"
    else:
        print("ERROR:given write mode (" + str(mode) + ") is not defined")
        return None

def analog_read(pin_no, period):
    return "#006:008:"+to_str(pin_no,3)+to_str(period,5)+"$"

def analog_write(pin_no, dutty_255):
    if(dutty_255<0 or dutty_255>255):
        print("ERROR: dutty € [0,255] but dutty=(" + str(dutty_255) + ") ")
        return None
    return "#007:006:"+to_str(pin_no,3)+to_str(dutty_255,3)+"$"

def fake_analog_write(pin_no, period, dutty_100):
    if(dutty_100<0 or dutty_100>100):
        print("ERROR: dutty € [0,100] but dutty=(" + str(dutty_100) + ") ")
        return None
    if(period<25 or period >32767):
        print("ERROR: period € [25,32767] but period=(" + str(period) + ") ")
        return  None
    return "#008:011:"+to_str(pin_no,3)+to_str(period,5)+to_str(dutty_100,3)+"$"

def set_variable_register(variable_register_index, value):
    if(value<-32767 or value >32767):
        print("ERROR: variable register value € [-32767,32767] but value=(" + str(value) + ") ")
        return None

    sign ='+'
    if(value<0):
        sign = '-'
    return "#010:009:"+sign+ to_str(value,5)+to_str(variable_register_index,3)+"$"

def set_string_register(which_string, string):
    if(len(string)>48):
        print("ERROR: len(string) must be <=48   but len(string) =(" + str(len(string)) + ") ")
        return None
    



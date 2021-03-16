import ardynamic_serial
import ardynamic


while True:
    #write_to_port()
    ardynamic_serial.write_wait_seconds_before_write=0.5
    ardynamic_serial.write_print_sent=True
    ardynamic_serial.write_print_salute=True
    ardynamic_serial.write_print_error=True
    ardynamic_serial.write_print_connection=True
    #queue informative
    ardynamic_serial.queue_print_append=True

    # LOOPS
    


    # FROM DATABASE

    # TO ARDUINO
    ardynamic_serial.write_to_port(ardynamic.analog_read())
    ardynamic_serial.write_to_port(ardynamic.digital_write(12, "HIGH"))

    # FROM ARDUINO

    # TO DATABASE

    # KEEP LOG





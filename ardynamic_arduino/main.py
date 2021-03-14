import ardynamic_serial
import ardynamic

while True:
    #write_to_port()
    ardynamic_serial.write_wait_seconds_before_write=0.5
    ardynamic_serial.write_print_sent=False
    ardynamic_serial.write_print_salute=False
    ardynamic_serial.write_print_error=False
    ardynamic_serial.write_print_connection=True
    #queue informative
    ardynamic_serial.queue_print_append=True

    # UI()-------------------------------------------------------

    string_arduino_are_you_there = ardynamic.arduino_are_you_there() #just to show there is such a functiom
    #ardynamic_serial.add_instruction_to_queue(string_arduino_are_you_there, False)

    string_digital_write = ardynamic.digital_write(12, "HIGH")
    ardynamic_serial.add_instruction_to_queue(string_digital_write, False)

    string_fake_analog_write = ardynamic.fake_analog_write(13, 500, 50)
    ardynamic_serial.add_instruction_to_queue(string_fake_analog_write, False)
    # ---------------------------------------------------------------------------
    read = ardynamic_serial.read_from_port()
    #if (read != False):print(ardynamic_serial.current_date()+":"+read)
    #if (read != False): print(read)
    ardynamic_serial.write_to_port()

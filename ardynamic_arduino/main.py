import ardynamic_serial
import ardynamic


while True:
    #write_to_port()
    ardynamic_serial.write_wait_seconds_before_write=0.5
    ardynamic_serial.write_print_sent=False
    ardynamic_serial.write_print_salute=True
    ardynamic_serial.write_print_error=True
    ardynamic_serial.write_print_connection=True
    #queue informative
    ardynamic_serial.queue_print_append=False

    # LOOPS



    # FROM DATABASE

    # TO ARDUINO
    ardynamic_serial.add_instruction_to_queue( ardynamic.change_print_mode(2) ,False)
    ardynamic_serial.add_instruction_to_queue(ardynamic.should_arduino_give_feedback("YES"), False)
    ardynamic_serial.add_instruction_to_queue(ardynamic.should_print_read(14, "YES"), False)
    ardynamic_serial.add_instruction_to_queue(ardynamic.analog_read(14,5000), False)
    ardynamic_serial.add_instruction_to_queue(ardynamic.digital_write(12, "HIGH"), False)
    ardynamic_serial.add_instruction_to_queue(ardynamic.fake_analog_write(13,2500,80), False)

    ardynamic_serial.write_to_port()
    read = ardynamic_serial.read_from_port()
    ardynamic.isInputReadValue(read)

    # FROM ARDUINO

    # TO DATABASE

    # KEEP LOG





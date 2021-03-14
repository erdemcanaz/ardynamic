import ardynamic_serial
import ardynamic


while True:
    string_arduino_are_you_there = ardynamic.arduino_are_you_there()
    ardynamic_serial.add_instruction_to_queue(string_arduino_are_you_there, False)
    ardynamic_serial.write_to_port()
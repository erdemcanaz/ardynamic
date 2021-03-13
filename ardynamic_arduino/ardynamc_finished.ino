//PARAMETERS
char data[80];                    //Records serial read to be processed later
const char start_del = '#';       //first character of any serial data
const char end_del = '$';         //last character of any serial data
const char seperator_del = ':';   //seperates serial data to sections| #value_type:value_length:value$
const char minus_del = '-';       //
const char plus_del = '+';        //
#define MAX_VALUE_LENGTH 50       //#3:3:len<=MAX_VALUE_LENGTH$, also maximum possible length of a string
#define MAX_TYPE_LENGTH 999       //#value_of_this<MAX_TYPE_LENGTH:3:value
#define MAX_STRINGS 5            //number of strings to keep
#define MAX_VARIABLE_INT 50       //number of integer variables to keep
#define MAX_PIN 25                //number of virtual pins (50 is a bit overkill, arduino has only 20 pins. But 50 is used considering there are other boards such as mege. Greater value decreases performance.


//VIRTUAL_REGISTERS
char STRING_DATA_REGISTER[MAX_STRINGS][MAX_VALUE_LENGTH];// keeps strings
int  STRING_LENGTH_REGISTER[MAX_STRINGS];                // keeps string lengths
int  READ_REGISTER[MAX_PIN];                             // keeps analog and digital read values
int  VARIABLE_REGISTER_INT[MAX_VARIABLE_INT];            // keeps variables
int  PRINT_READ_REGISTER[MAX_PIN];                       // keeps list of read register indexes to print if asked
int  PRINT_VARIABLE_REGISTER_INT[MAX_PIN];               // keeps list of variable register indexes to print if asked
int  PRINT_MODE = 0;                                     // 0:DO NOT PRINT; 1:print once than change it to 0; 2:print continiously
boolean GIVE_FEEDBACK = true;                            // When a data is received, arduino informs master on what instruction is performed if this is true.

boolean PIN_RESERVED_REGISTER[MAX_PIN];// if you manually programmed arduino and used some pins, set those pins as true (reserved if true, so ardynamic will not make any change on them)
int PIN_FUNCTION_REGISTER[MAX_PIN];   // 0:set_mode; 1:digital_read; 2:digital_write; 3:analog_read; 4:analog_write; 5:fake_analog_write
int PIN_MODE_REGISTER[MAX_PIN];       // 0:input ; 1:input_pullup
int PIN_STATE_REGISTER[MAX_PIN];      // 0:low; 1:high
int PIN_PERIOD_REGISTER[MAX_PIN];     // [0,32767] in ms
int PIN_DUTY_REGISTER[MAX_PIN];       // [0,100](fake_analog_write) or [0,255] (analog write)
int PIN_ON_TIME_REGISTER[MAX_PIN];    // PIN_ON_TIME_REGISTER[pin_no] = (PIN_PERIOD_REGISTER[pin_no] * PIN_DUTY_REGISTER[pin_no]/100)
//SETUP

//LOOP
void ARDYNAMIC_RUN() {
  read_serial();
  set_pins();
  print_read_and_variable_registers();
}
//FUNCTIONS
boolean accept_data = false; //Set true if read=='#' and Set false if read=='$'
int counter = 0;             //Set zero if read == '#' or read =='$'
void read_serial() {
  while (Serial.available()) {
    char c = Serial.read();
    if ( !isNumeric_or_Letter_or_Delimeter(c) )return;//READ IS NOT AN ACCEPTED CHARACACTER
    //START-----------------------------------------------------
    if (c == start_del) {
      accept_data = true;
      counter = 0;
      for ( int i = 0; i < sizeof(data);  i++ ) data[i] = '0';
    }//END------------------------------------------------------
    else if (c == end_del) {
      accept_data = false;
      counter = 0;
      try_to_use_serial_data();
    }
    //MID------------------------------------------------------
    else if (accept_data) {
      data[counter] = c;
      counter ++;
    }
  }
}
void try_to_use_serial_data() { // used in read_serial()
  int value_length = -1;
  int value_type   = -1;
  //VALUE TYPE----------------------------------------------------------------------
  if (isValueNumeric(0, 3)) {
    value_type = (int)(data[0] - '0') * 100 + (int)(data[1] - '0') * 10 + (int)(data[2] - '0')  ;
  } else return;
  //CHECK
  if (value_type > MAX_TYPE_LENGTH || value_type < 0 || data[3] != ':') return;
  //VALUE LENGTH---------------------------------------------------------------------
  if (isValueNumeric(4, 3)) {
    value_length = (int)(data[4] - '0') * 100 + (int)(data[5] - '0') * 10 + (int)(data[6] - '0')  ;
  }
  //CHECK
  if (value_length > MAX_VALUE_LENGTH || value_length < 0 || data[7] != ':') return;

  //------------------------------------------------------------------------------
  if (value_type == 0)      value_type_0(); //HALT
  else if (value_type == 1) value_type_1(); //INPUT PIN
  else if (value_type == 2) value_type_2(); //INPUT_PULLUP PIN
  else if (value_type == 3) value_type_3(); //OUTPUT PIN
  else if (value_type == 4) value_type_4(); //DIGITAL_READ
  else if (value_type == 5) value_type_5(); //DIGITAL_WRITE
  else if (value_type == 6) value_type_6(); //ANALOG_READ
  else if (value_type == 7) value_type_7(); //ANALOG_WRITE
  else if (value_type == 8) value_type_8(); //FAKE_ANALOG_WRITE
  else if (value_type == 9) value_type_9(); //UPDATE_BAUD_RATE
  else if (value_type == 10)value_type_10();//SAVE_SARIAL_DATA_TO_VARIABLE_REGISTER_INT
  else if (value_type == 11)value_type_11(value_length);//SAVE_STRING_DATA_TO_STRING_REGISTER
  else if (value_type == 12)value_type_12();//PRINT_STATE_READ_REGISTER
  else if (value_type == 13)value_type_13();//PRINT_STATE_VARIABLE_REGISTER_INT
  else if (value_type == 14)value_type_14();//PRINT_MODE
  else if (value_type == 15)value_type_15();//GIVE_FEEDBACK
  else if (value_type == 16)value_type_16();//RESET_PRINT_REGISTER
}
void set_pins() {
  //0:set_mode; 1:digital_read; 2:digital_write; 3:analog_read; 4:analog_write; 5:fake_analog_write
  for (int i = 0; i < MAX_PIN; i++) {
    if (PIN_RESERVED_REGISTER[i])continue;

    int pin_function = PIN_FUNCTION_REGISTER[i];
    if (pin_function == 0) { //SET
      if (PIN_MODE_REGISTER[i] == 0)pinMode(i, INPUT);
      else pinMode(i, INPUT_PULLUP);
    }
    else if (pin_function == 1) { //digital_read
      if (PIN_MODE_REGISTER[i] == 0)pinMode(i, INPUT);
      else pinMode(i, INPUT_PULLUP);
      READ_REGISTER[i] = digitalRead(i);
    }
    else if (pin_function == 2) { //digital_write
      pinMode(i, OUTPUT);
      if (PIN_STATE_REGISTER[i] == 1)digitalWrite(i, HIGH);
      else digitalWrite(i, LOW);
    }
    else if (pin_function == 3) { //analog_read
      pinMode(i, INPUT);
      READ_REGISTER[i] = analogRead(i);
    }
    else if (pin_function == 4) { //analog_write
      pinMode(i, OUTPUT);
      analogWrite(i, PIN_DUTY_REGISTER[i]);
    }
    else if (pin_function == 5) { //fake_analog_write
      pinMode(i, OUTPUT);
      if ( (millis() % PIN_PERIOD_REGISTER[i]) <= PIN_ON_TIME_REGISTER[i]) digitalWrite(i, HIGH);
      else digitalWrite(i, LOW);
    }
  }
}
void print_read_and_variable_registers() {
  if (PRINT_MODE == 0)return;
  //READ
  for (int i = 0; i < MAX_PIN; i++)if (PRINT_READ_REGISTER[i])Serial.println("#R:" + String(i) + ":" + String(READ_REGISTER[i]) + "$");
  //VARIABLE
  for (int i = 0; i < MAX_PIN; i++)if (PRINT_VARIABLE_REGISTER_INT[i])Serial.println("#V:" + String(i) + ":" + String(VARIABLE_REGISTER_INT[i]) + "$");
  if (PRINT_MODE == 1)PRINT_MODE = 0;
}
//SETTING
void set_as_reserved(int pin_no) {
  PIN_RESERVED_REGISTER[pin_no] = true;
}
void set_settings_as_default() {
  //CODE...
}
void export_ardynamic_object_to_EEPROM() {
  //CODE...
}
void import_ardynamic_object_from_EEPROM() {
  //CODE...
}

//TOOLS
boolean isNumeric_or_Letter_or_Delimeter(char c) {
  boolean c1 = !(c >= '0' && c <= '9');
  boolean c2 = !(c >= 'A' && c <= 'Z');
  boolean c3 = !(c >= 'a' && c <= 'z');
  boolean c4 = !(c == start_del || c == end_del || c == seperator_del || c == minus_del || c == plus_del);

  if (c1 && c2 && c3 && c4) return false;

  return true;
}
boolean isNumeric(char c) {
  if ((c >= '0' && c <= '9') || c == '-' || c == '+') return true;
  return false;
}
boolean isValueNumeric(int start, int value_length) {
  //applies isNumeric() on each char starting from data[start] to data[start+value_length-1] (included)
  if (value_length < 1)return false;

  for (int i = start; i < (start + value_length); i++) {
    if (!isNumeric(data[i]))return false;
  }
  return true;
}
int convert_to_int(int start, int integer_length) {
  int multiplier_base_ten[] = {1, 10, 100, 1000, 10000};
  int sum = 0;
  for (int i = start; i < start + integer_length; i++) {
    int digit = (int)(data[i] - '0');
    int multiply_with =  multiplier_base_ten[(integer_length - 1) - (i - start)];
    int add = digit * multiply_with;
    sum += add;
  }
  return sum;
}

//TO SIMPLFY CODE
//PRINT FORMAT; TYPE:type.val:  COMMENT: COMMENT.val:    PIN:pin.val:  ---ANYTHING---);
void value_type_0() {//---
  if (GIVE_FEEDBACK)Serial.println("#TPYE:0:DESCRIPTION:not_in_use$");
}
void value_type_1() {//INPUT_FLOAT
  if (!isValueNumeric(8, 3))return;
  int pin_no = convert_to_int(8, 3);

  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:1:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  PIN_MODE_REGISTER[pin_no] = 0;
  if (GIVE_FEEDBACK)Serial.println("#TYPE:1:COMMENT:pin floats:PIN:" + String(pin_no) + "$");

}
void value_type_2() {//INPUT_PULL_UP
  if (!isValueNumeric(8, 3))return;
  int pin_no = (int)(data[8] - '0') * 100 + (int)(data[9] - '0') * 10 + (int)(data[10] - '0')  ;

  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:2:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  PIN_MODE_REGISTER[pin_no] = 1;
  if (GIVE_FEEDBACK)Serial.println("#TYPE:2:COMMENT:pin is pulled_up:PIN:" + String(pin_no) + "$");

}
void value_type_3() {//---
  if (GIVE_FEEDBACK)Serial.println("#TPYE:3:DESCRIPTION:not_in_use$");
}
void value_type_4() {//DIGITAL_READ
  if (!isValueNumeric(8, 8))return;
  int pin_no = convert_to_int(8, 3);
  int period_ms = convert_to_int(11, 5);
  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:4:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  PIN_FUNCTION_REGISTER[pin_no] = 1;
  PIN_PERIOD_REGISTER[pin_no] = period_ms;

  if (GIVE_FEEDBACK)Serial.println("#TYPE:4:COMMENT:pin " + String(pin_no) + " performs digital read for once after every" + String(period_ms) + "ms:PIN:" + String(pin_no) + "$");

}
void value_type_5() {//DIGITAL_WRITE
  // 0:set_mode; 1:digital_read; 2:digital_write; 3:analog_read; 4:analog_write; 5:fake_analog_write
  if (!isValueNumeric(8, 4))return;
  int pin_no = convert_to_int(8, 3);
  int logic_value = convert_to_int (11, 1);

  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:5:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  if (GIVE_FEEDBACK)Serial.println("#TYPE:5:COMMENT:pin " + String(pin_no) + " is written digitaly as(" + String(logic_value) + "):PIN:" + String(pin_no) + "$");
  PIN_FUNCTION_REGISTER[pin_no] = 2;
  PIN_STATE_REGISTER[pin_no] = logic_value;

}
void value_type_6() {//ANALOG_READ
  if (!isValueNumeric(8, 8))return;
  int pin_no = convert_to_int(8, 3);
  int period_ms = convert_to_int (11, 5);
  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:6:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  if (GIVE_FEEDBACK)Serial.println("#TYPE:6:COMMENT:pin " + String(pin_no) + " performs analog read for once after every" + String(period_ms) + "ms:PIN:" + String(pin_no) + "$");
  PIN_FUNCTION_REGISTER[pin_no] = 3;
  PIN_PERIOD_REGISTER[pin_no] = period_ms;

}
void value_type_7() {//ANALOG_WRITE
  if (!isValueNumeric(8, 6))return;
  int pin_no = convert_to_int(8, 3);
  int byte_duty = convert_to_int (11, 3);
  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:7:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  if (GIVE_FEEDBACK)Serial.println("#TYPE:7:COMMENT:pin " + String(pin_no) + " has dutty of (" + String(byte_duty) + "/255):PIN:" + String(pin_no) + "$");
  PIN_FUNCTION_REGISTER[pin_no] = 4;
  PIN_DUTY_REGISTER[pin_no] = byte_duty;

}
void value_type_8() {//FAKE_ANALOG_WRITE
  if (!isValueNumeric(8, 11))return;
  int pin_no = convert_to_int(8, 3);
  int period_ms = convert_to_int (11, 5);
  int duty = convert_to_int (16, 3);
  if (duty > 100) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:8:COMMENT:duty can not be greater than 100:PIN:" + String(pin_no) + "$");
  }
  if (PIN_RESERVED_REGISTER[pin_no]) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:8:COMMENT:pin is reserved:PIN:" + String(pin_no) + "$");
    return;
  }
  if (GIVE_FEEDBACK)Serial.println("#TYPE:8:COMMENT:pin " + String(pin_no) + "'s dutty is (" + String(duty) + "/100) and it's period is " + String(period_ms) + " ms:PIN:" + String(pin_no) + "$");
  PIN_FUNCTION_REGISTER[pin_no] = 5;
  PIN_PERIOD_REGISTER[pin_no] = period_ms;
  PIN_DUTY_REGISTER[pin_no] = duty;
  PIN_ON_TIME_REGISTER[pin_no] = (int)(period_ms * (duty / 100.0));
}
void value_type_9() {//CHANGE_BAUD_RATE
  if (GIVE_FEEDBACK)Serial.println("#TPYE:9:DESCRIPTION:not_in_use$");
}
void value_type_10() { //SAVE_INTEGER_VALUE_TO_VARIABLE_REGISTER
  if (!isValueNumeric(8, 9))return;
  int value = convert_to_int(9, 5);
  int variable_register_index = convert_to_int(14, 3);
  if (data[8] == '-') value = -value;
  VARIABLE_REGISTER_INT[variable_register_index] = value;
  if (GIVE_FEEDBACK)Serial.println("#TYPE:10:COMMENT:integer value(" + String(value) + ") is saved to variable_register[" + String(variable_register_index) + "] :INDEX:" + String(variable_register_index) + "$");
}
void value_type_11(int value_length) { //SAVE_STRING_TO_STRING_DATA_REGISTER
  if (isValueNumeric(8, 2)) {
    int string_length = value_length - 2;
    int which_string = convert_to_int(8, 2);
    STRING_LENGTH_REGISTER[which_string] = string_length;
    if (GIVE_FEEDBACK)Serial.print("#TYPE:11:DESCRIPTION: save string (len==" + String(string_length) + ") to string_register[" + String(which_string) + "]:STRING:");
    for (int i = 0 ; i < string_length; i++)
    {
      STRING_DATA_REGISTER[which_string][i] = data[10 + i];
      if (GIVE_FEEDBACK)Serial.print((char)data[10 + i]);
    }
    if (GIVE_FEEDBACK)Serial.println('$');
  }
}
void value_type_12() { //PRINT_READ_REGISTER
  if (!isValueNumeric(8, 4))return;
  int logic_val = convert_to_int(8, 1);
  int print_read_register_index = convert_to_int(9, 3);
  PRINT_READ_REGISTER[print_read_register_index] = logic_val;
  if (logic_val == 0) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:12:COMMENT: read_register[" + String(print_read_register_index) + "] will not be printed:INDEX:" + String(print_read_register_index) + "$");
  } else {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:12:COMMENT: read_register[" + String(print_read_register_index) + "] will be printed:INDEX:" + String(print_read_register_index) + "$");
  }
}
void value_type_13() { //PRINT_VARIABLE_REGISTER
  if (!isValueNumeric(8, 4))return;
  int logic_val = convert_to_int(8, 1);
  int print_variable_register_index = convert_to_int(9, 3);
  PRINT_VARIABLE_REGISTER_INT[print_variable_register_index] = logic_val;
  if (logic_val == 0) {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:12:COMMENT: variable_register[" + String(print_variable_register_index) + "] will not be printed:INDEX:" + String(print_variable_register_index) + "$");
  } else {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:12:COMMENT: variable_register[" + String(print_variable_register_index) + "] will be printed:INDEX:" + String(print_variable_register_index) + "$");
  }
}
void value_type_14() { //CHANGE_PRINT_MODE
  if (!isValueNumeric(8, 2))return;
  int print_mode = convert_to_int(8, 2);
  PRINT_MODE = print_mode;
  if (GIVE_FEEDBACK)Serial.println("#TYPE:14:COMMENT: print_mode is " + String(PRINT_MODE) + ":PRINT_MODE:" + String(PRINT_MODE) + "$");
}
void value_type_15() { //GIVE FEEDBACK
  if (!isValueNumeric(8, 1))return;
  int logic_val = convert_to_int(8, 1);
  if (logic_val) GIVE_FEEDBACK = true;
  else GIVE_FEEDBACK = false;

  if (GIVE_FEEDBACK)Serial.println("#TYPE:15:COMMENT: arduino gives feedback$");
  else {
    if (GIVE_FEEDBACK)Serial.println("#TYPE:15:COMMENT: arduino does not give feedback$");
  }
}
void value_type_16() { //RESET_PRINT_REGISTERS
  if (!isValueNumeric(8, 1))return;
  int which_to_reset = convert_to_int(8, 1);//T={0,1,2}, 0:reset both; 1:reset read; 2:reset variable;
  if (which_to_reset == 1) {
    for (int i = 0; i < MAX_PIN; i++)PRINT_READ_REGISTER[i] = 0;
    if (GIVE_FEEDBACK)Serial.println("#TYPE:16:COMMENT:print_read_register is refreshed$");
  }
  else if (which_to_reset == 2) {
    for (int i = 0; i < MAX_PIN; i++)PRINT_VARIABLE_REGISTER_INT[i] = 0;
    if (GIVE_FEEDBACK)Serial.println("#TYPE:16:COMMENT:print_variable_register_int is refreshed$");
  }
  else if (which_to_reset == 0) {
    for (int i = 0; i < MAX_PIN; i++)PRINT_READ_REGISTER[i] = 0;
    for (int i = 0; i < MAX_PIN; i++)PRINT_VARIABLE_REGISTER_INT[i] = 0;
    if (GIVE_FEEDBACK)Serial.println("#TYPE:16:COMMENT:print_read_register & print_variable_register_int are refreshed$");
  }

}

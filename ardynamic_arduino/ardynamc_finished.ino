//PARAMETERS
char data[80];                    //Records serial read to be processed later
const char start_del = '#';       //first character of any serial data
const char end_del = '$';         //last character of any serial data
const char seperator_del = ':';   //seperates serial data to sections| #value_type:value_length:value$
const char minus_del = '-';       //
const char plus_del = '+';        //
#define MAX_VALUE_LENGTH 50       //#3:3:len<=MAX_VALUE_LENGTH$, also maximum possible length of a string
#define MAX_TYPE_LENGTH 999       //#value_of_this<MAX_TYPE_LENGTH:3:value
#define MAX_STRINGS 10            //number of strings to keep
#define MAX_VARIABLE_INT 50       //number of integer variables to keep
#define MAX_PIN 50                //number of virtual pins (50 is a bit overkill, arduino has only 20 pins. But 50 is used considering there are other boards such as mege. Greater value decreases performance.


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
void ARDYNAMIC_RUN(){
  read_serial();
  //CODE set pin
  //CODE print
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
  int multiplier_base_ten[] = {0,10,100,1000,10000};
  int sum = 0;
  for (int i = start; i < start + integer_length; i++) {    
    int digit = (int)(data[i] - '0');
    int multiply_with =  multiplier_base_ten[(integer_length - 1) - (i - start)];
    int add=digit*multiply_with;
    sum += add;    
  }
  return sum;
}

//TO SIMPLFY CODE
//PRINT FORMAT; TYPE:type.val:  COMMENT: COMMENT.val:    PIN:pin.val:  ---ANYTHING---);
void value_type_0(){
  Serial.println("TPYE:0:DESCRIPTION:not_in_use");
}
void value_type_1() {
    if(!isValueNumeric(8, 3))return;
    int pin_no = (int)(data[8] - '0') * 100 + (int)(data[9] - '0') * 10 + (int)(data[10] - '0')  ; 

    if(PIN_RESERVED_REGISTER[pin_no]){
      Serial.println("#TYPE:1:COMMENT:pin is reserved:PIN:"+String(pin_no)+"$");
      return;
    }
    PIN_MODE_REGISTER[pin_no] = 0;    
    Serial.println("#TYPE:1:COMMENT:if any read is performed, this pin floats:PIN:"+String(pin_no)+"$");
  
}
void value_type_2() { 
  if (!isValueNumeric(8, 3))return;
  int pin_no = (int)(data[8] - '0') * 100 + (int)(data[9] - '0') * 10 + (int)(data[10] - '0')  ;    

  if(PIN_RESERVED_REGISTER[pin_no]){
    Serial.println("#TYPE:2:COMMENT:pin is reserved:PIN:"+String(pin_no)+"$");  
    return;
  }
  PIN_MODE_REGISTER[pin_no] = 1;    
  Serial.println("#TYPE:2:COMMENT:if any read is performed, this pin is pulled_up:PIN:"+String(pin_no)+"$");
  
}
void value_type_3() {
  Serial.println("TPYE:3:DESCRIPTION:not_in_use");
}
void value_type_4() {
    //#004:008:XXXYYYYY$
    if(!isValueNumeric(8, 8))return;
    int pin_no = convert_to_int(8, 3);
    int period_ms = convert_to_int(11, 5);
    Serial.println(pin_no);
    if(PIN_RESERVED_REGISTER[pin_no]){
      Serial.println("#TYPE:4:COMMENT:pin is reserved:PIN:"+String(pin_no)+"$");
      return;
    }
    PIN_FUNCTION_REGISTER[pin_no]=1;
    PIN_PERIOD_REGISTER[pin_no]=period_ms;
    
    Serial.println("#TYPE:4:COMMENT:pin "+String(pin_no)+" performs digital read for once in every"+String(period_ms)+"ms:PIN:"+String(pin_no)+"$");   
 
}
void value_type_5() {
    // 0:set_mode; 1:digital_read; 2:digital_write; 3:analog_read; 4:analog_write; 5:fake_analog_write
    if (!isValueNumeric(8, 4))return;
    int pin_no = convert_to_int(8, 3);
    int logic_value = convert_to_int (11, 1);

    if(PIN_RESERVED_REGISTER[pin_no]){
      Serial.println("#TYPE:4:COMMENT:pin is reserved:PIN:"+String(pin_no)+"$");
      return;
    }
    PIN_FUNCTION_REGISTER[pin_no]=2;
    PIN_STATE_REGISTER[pin_no]=logic_value;
    Serial.println("#TYPE:5:COMMENT:pin "+String(pin_no)+" is written digitaly as(+"+String(logic_value)+"):PIN:"+String(pin_no)+"$"); 
  
}
void value_type_6() {
  if (isValueNumeric(8, 8)) {
    int pin_no = convert_to_int(8, 3);
    int period_ms = convert_to_int (11, 5);
    //set_pin_registers(pin_no, 3, 0, -1, period_ms, -1);
    Serial.print('#');
    Serial.print("SETUP:");
    Serial.print(pin_no);
    Serial.print(":PERIOD:");
    Serial.print(period_ms);
    Serial.println(":ANALOG_READ_PIN$");
  }
}
void value_type_7() {
  if (isValueNumeric(8, 6)) {
    int pin_no = convert_to_int(8, 3);
    int byte_duty = convert_to_int (11, 3);
    //set_pin_registers(pin_no, 4, 2, -1, -1, byte_duty);
    Serial.print('#');
    Serial.print("SETUP:");
    Serial.print(pin_no);
    Serial.print(":BYTE_DUTY:");
    Serial.print(byte_duty);
    Serial.println(":ANALOG_WRITE_PIN$");
  }
}
void value_type_8(){
  if (isValueNumeric(8, 11)) {
    int pin_no = convert_to_int(8, 3);
    int period = convert_to_int (11, 5);
    int duty = convert_to_int (16, 3);
    //set_pin_registers(pin_no, 5, 2, -1, period, duty);
    Serial.print('#');
    Serial.print("SETUP:");
    Serial.print(pin_no);
    Serial.print(":PERIOD_MS:");
    Serial.print(period);
    Serial.print(":DUTY_100:");
    Serial.print(duty);
    Serial.println(":FAKE_ANALOG_WRITE_PIN$");
  }
}
void value_type_9(){
  //CODE...
}
void value_type_10(){
  if (isValueNumeric(8, 9)) {
    int value = convert_to_int(9, 5);
    int variable_register_index = convert_to_int(14, 3);
    if (data[8] == '-') value = -value;
    VARIABLE_REGISTER_INT[variable_register_index] = value;
    Serial.print('#');
    Serial.print("SAVE_VARIABLE:");
    Serial.print(value);
    Serial.print(":TO_REGISTER:");
    Serial.print(variable_register_index);
    Serial.println('$');
  }
}
void value_type_11(int value_length){
   if (isValueNumeric(8, 2)) {
    int string_length = value_length - 2;
    int which_string = convert_to_int(8, 2);
    STRING_LENGTH_REGISTER[which_string] = string_length;
    Serial.print('#');
    Serial.print("TO_REGISTER:");
    Serial.print(which_string);
    Serial.print(":LENGTH:");
    Serial.print(string_length);
    Serial.print(":SAVE_STRING:");
    for (int i = 0 ; i < string_length; i++)
    {
      STRING_DATA_REGISTER[which_string][i] = data[10 + i];
      Serial.print((char)data[10 + i]);
    }
    Serial.println('$');
  }
}
void value_type_12(){
  if (isValueNumeric(8, 4)) {
    int logic_val = convert_to_int(8, 1);
    int print_read_register_index = convert_to_int(9, 3);
    PRINT_READ_REGISTER[print_read_register_index] = logic_val;
    Serial.print('#');
    Serial.print("SETTING:");
    Serial.print(":PRINT_READ:");
    Serial.print(print_read_register_index);
    Serial.print(":IF:");
    Serial.print(logic_val);
    Serial.println('$');
  }
}
void value_type_13(){
  if (isValueNumeric(8, 4)) {
    int logic_val = convert_to_int(8, 1);
    int print_variable_register_index = convert_to_int(9, 3);
    PRINT_VARIABLE_REGISTER_INT[print_variable_register_index] = logic_val;
    Serial.print('#');
    Serial.print("SETTING:");
    Serial.print(":PRINT_VARIABLE:");
    Serial.print(print_variable_register_index);
    Serial.print(":IF:");
    Serial.print(logic_val);
    Serial.println('$');
  }
}
void value_type_14(){
  if (isValueNumeric(8, 2)) {
    int print_mode = convert_to_int(8, 2);
    PRINT_MODE = print_mode;
    Serial.print('#');
    Serial.print("SETTING");
    Serial.print(":PRINT_MODE:");
    Serial.print(PRINT_MODE);
    Serial.println('$');
  }
}
void value_type_15(){
  if (isValueNumeric(8, 1)) {
    int logic_val = convert_to_int(8, 2);
    if(logic_val) GIVE_FEEDBACK=true;
    else GIVE_FEEDBACK=false;   
  }
}

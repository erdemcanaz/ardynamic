void setup() {
  //one start bit, 8 data bits, no parity bit, 1 stop bit
  Serial.begin(9600, SERIAL_8N1);
  set_as_reserved(0);
  set_as_reserved(1);   
}

void loop() {
  //ARDYNAMIC_RUN();  
}


void setup() {
  Serial.begin(9600);
  set_as_reserved(0);
  set_as_reserved(1);   
}

void loop() {
  ARDYNAMIC_RUN();  
}

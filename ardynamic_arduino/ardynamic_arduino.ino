
void setup() {
  set_as_reserved(0);
  set_as_reserved(1);
  set_as_reserved(7);
  set_as_reserved(8);
  Serial.begin(9600);
  pinMode(4,OUTPUT);
}

void loop() {
  ARDYNAMIC_RUN();
  
}

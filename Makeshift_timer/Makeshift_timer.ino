#define LED 2
#define Light 4
#define Motion 15

int count = 0;

void setup() {
  pinMode(LED,OUTPUT);
  pinMode(Light,INPUT);  
  pinMode(Motion, INPUT);
  attachInterrupt(digitalPinToInterrupt(Motion), triggeredMotion, RISING);
  Serial.begin(9600);
}

void loop() {
  count = count + 1;
  if(count == 10000000){
    digitalWrite(LED,LOW);
  }
  /*digitalWrite(LED,LOW);
  Serial.println(analogRead(Light));
  if(analogRead(Light) < 1000 && digitalRead(Motion) != 0){
    digitalWrite(LED,HIGH);
    delay(500);
  }
  else{
    digitalWrite(LED,LOW);
    delay(500);
  }
  */
}

 void triggeredMotion(){
  if(analogRead(Light) < 1000){
    Serial.println(digitalRead(Motion));
    Serial.println(analogRead(Light));
    digitalWrite(LED,HIGH);
    count = 0;
  }
}

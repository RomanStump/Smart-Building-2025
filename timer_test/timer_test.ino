#include "driver/timer.h"
#define LED 2
#define Light 4
#define Motion 15

hw_timer_t *timer0 = NULL;

void IRAM_ATTR LEDOn()
{
  Serial.println("timer interrupt");
  digitalWrite(LED,!digitalRead(LED));
 //if(digitalRead(LED)){
 // digitalWrite(LED,LOW);   
 //}
}

void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  pinMode(Light,INPUT);  
  pinMode(Motion, INPUT);
  timer0 = timerBegin(1000000);
  timerAttachInterrupt(timer0, LEDOn);
  timerWrite(timer0, 0);
  timerAlarm(timer0, 5000000, true, 0);
}

void loop() {
  timerStart(timer0);
  if(digitalRead(Motion)){
    timerRestart(timer0);
    Serial.println(digitalRead(Motion));
  }
  /*analogReadResolution(12);
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

/* void triggeredMotion(){
  if(analogRead(Light) < 1000){
    Serial.println(digitalRead(Motion));
    digitalWrite(LED,HIGH);
  }
}*/

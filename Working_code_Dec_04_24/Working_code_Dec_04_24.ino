#include "driver/timer.h"
#define LED 2
#define Light 4
#define Motion 15

hw_timer_t *timer0 = NULL;

void IRAM_ATTR LEDOn()
{
  Serial.println("timer interrupt");
  digitalWrite(LED,!digitalRead(LED));
  if(digitalRead(LED)){
    digitalWrite(LED,LOW);   
  }
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   pinMode(LED,OUTPUT);
   pinMode(Light,INPUT);  
   pinMode(Motion, INPUT);
   attachInterrupt(digitalPinToInterrupt(Motion), triggeredMotion, RISING);
   timer0 = timerBegin(1000000);
   timerAttachInterrupt(timer0, LEDOn);
   timerWrite(timer0, 0);
   timerAlarm(timer0, 5000000, true, 0);
   
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED,HIGH);
  //delay(500);
  //digitalWrite(LED,LOW);
  //delay(500);
  //Serial.println(digitalRead(Motion));
  //Serial.println(analogRead(Light));

}

 void triggeredMotion(){
  if(analogRead(Light) < 1000){
    Serial.println(digitalRead(Motion));
    Serial.println(analogRead(Light));
    digitalWrite(LED,HIGH);
    timerRestart(timer0);
  }
 }

 

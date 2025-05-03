#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <WiFi.h>
//#include "driver/timer.h"
#include "esp_wifi.h"
//#include "esp_sleep.h"
#include "H:/Capstone/ESP_NOW_Test/myData.h"
#define LED 33             //LED is at pin 25
hw_timer_t *timer0 = NULL;  //Timer variable
struct_message myData;      //Data being received
float lux = 0;              //Amount of lux from light sensor
bool motionDetected = false;  //Has motion been detected
bool timerDone = true;


void IRAM_ATTR LEDOff()  //Timer interrupt function
{
  digitalWrite(LED, LOW); //Turn LED off
  timerDone = true;

}

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  lux = myData.lightValue;
  motionDetected = myData.Movement;
}


void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);

  pinMode(LED, OUTPUT);   //Set LED to be an output

  WiFi.mode(WIFI_STA);    //Set device as a Wi-Fi Station

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_sleep_enable_timer_wakeup(1000000);  //enable sleep timer to trigger every 1 second (ESP32 wakes up after 1 second of sleeping)

  //Register "OnDataRecv" function to be the callback function that runs when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  timer0 = timerBegin(100000);            //Begin timer at 100kHz
  timerAttachInterrupt(timer0, LEDOff);   //Register "timer0" to be the interrupt timer and "LEDOff" to be the interrupt function
  timerWrite(timer0, 0);                  //Make Sure timer starts at zero

  //Register timer0 to be the alarm to set off the timer interrupt, 18000000/100000 = 180 seconds
  //alarm auto resets when timer interrupt is finished, and 0 means it auto resets indefinitely
  timerAlarm(timer0, 18000000, true, 0);

}

void loop() {

  if (timerDone) {
    esp_wifi_stop();          //Turn off wifi to allow sleep mode
    delay(200);
    esp_light_sleep_start();  //Enter light sleep
    esp_wifi_start();         //Turn on wifi to allow communication
  }

  while (motionDetected) {
    if (lux < 300) {
      digitalWrite(LED, HIGH);  //Turn on LED when light levels are too low
      timerDone = false;
    }
    timerRestart(timer0);       //Restart timer when motion is detected
    motionDetected = false;     //Return "motionDetected" back to false so it's not an infinite loop
  }
  //Serial.println(lux);

  delay(1000); //Keep receiver awake for 1 second for receiving data

}

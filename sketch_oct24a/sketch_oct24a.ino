#include "driver/timer.h"
#include "FS.h"
#include <LittleFS.h>
#define LED 2
#define Light 4
#define Motion 15
#define FORMAT_LITTLEFS_IF_FAILED true


hw_timer_t *timer0 = NULL;
int mydata;


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}


void IRAM_ATTR LEDOn()
{
  Serial.println("timer interrupt");
  //digitalWrite(LED,!digitalRead(LED));
  if(digitalRead(LED)){
    digitalWrite(LED,LOW);   
  }
}

void setup() {
  Serial.begin(9600);
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
    Serial.println("LittleFS Mount Failed");
    return;
  }
  else{
    Serial.println("LittleFS Mounted Successfully");
  }
  //writeFile(LittleFS, "/capstoneData.txt", "MY ESP32 DATA \r\n");
  deleteFile(LittleFS, "/capstoneData.txt");
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
  //do nothing
  Serial.println(digitalRead(Motion));
  delay(5000);
  Serial.println(analogRead(Light));
  delay(5000);
}

 void triggeredMotion(){
  if(analogRead(Light) < 1000){
    mydata = analogRead(Light);
    //appendFile(LittleFS, "/capstoneData.txt", (String(mydata) + "\r\n").c_str());
    //readFile(LittleFS, "/capstoneData.txt");
    Serial.println(digitalRead(Motion));
    Serial.println(analogRead(Light));
    Serial.println(mydata);
    digitalWrite(LED,HIGH);
    timerRestart(timer0);
    //timerStart(timer0);
  }
}

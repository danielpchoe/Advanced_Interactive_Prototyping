#include "M5CoreInk.h"
#include <Adafruit_NeoPixel.h>
#include "TimeFunctions.h"
//#include <Servo.h>              // servo library

Ink_Sprite TimePageSprite(&M5.M5Ink);
Ink_Sprite PageSprite(&M5.M5Ink);

RTC_TimeTypeDef RTCtime, RTCTimeSave;
RTC_TimeTypeDef AlarmTime;
uint8_t second = 0, minutes = 0;

int i = 0, j = 1, k = 2;

const int STATE_DEFAULT = 0;
const int STATE_EDIT_HOURS = 1;
const int STATE_EDIT_MINUTES = 2;
const int STATE_ALARM = 4;
const int STATE_ALARM_FINISHED = 5;
// const int STATE_ALARM_RESET = 6;
int program_state = STATE_DEFAULT;

unsigned long rtcTimer = 0;

unsigned long underlineTimer = 0;
bool underlineOn = false;

unsigned long ledBlinkTimer = 0;
bool ledBlinkOn = false;

unsigned long beepTimer = 0;


const int sensorPin = 33;  // 4-wire bottom connector input pin used by M5 units
const int relayPin = 26;
int sensorVal = 0;
unsigned long sensorTimer = 0;
int brightnessVal = 0;

const int rgbledPin = 32; 

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(
    3, // number of LEDs
    rgbledPin, // pin number
    NEO_GRB + NEO_KHZ800);  // LED type
    
bool relayTriggered = false;

void setup() {
  M5.begin();
  pinMode(sensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);
  
  M5.begin(true,true,true);

  M5.rtc.GetTime(&RTCTimeSave);
  AlarmTime = RTCTimeSave;
  AlarmTime.Minutes = AlarmTime.Minutes + 1;  // set alarm 1 minutes ahead 
  M5.update();
  
  M5.M5Ink.clear();
  delay(500);

  checkRTC();
  PageSprite.creatSprite(0, 0, 200, 200);
  drawTime();
  
  pixels.begin();           // initialize NeoPixel strip object 
  pixels.show();            // turn OFF all pixels 
  pixels.setBrightness(50); // set brightness to about 1/5 (max = 255)
}

void loop() {
  
  // M5.update();
  // M5.Speaker.tone(1000,1000);
  // if( M5.BtnPWR.wasPressed()){
  //   Serial.printf("Btn wasPressed!");
  // }
  // delay(1000);
  
  sensorVal = analogRead(sensorPin);
  brightnessVal = map(sensorVal, 0, 4095, 0, 255);
  
  M5.update();
  // if(millis() > beepTimer + 1000){
  //   M5.Speaker.tone(500,300);
  //   beepTimer = millis();
  // }
  
  // if(sensorVal > 2000)
  //   digitalWrite(relayPin, HIGH);
  // else
  //   digitalWrite(relayPin, LOW);
  
  // check if data has been received on the Serial port:
  if(Serial.available() > 0)
  {
    // input format is "hh:mm" 
    char input[6];
    int charsRead = Serial.readBytes(input, 6);    // read 6 characters 
    if(charsRead == 6 && input[2] == ':') {
      int mm = int(input[4] - '0') + int(input[3] - '0')*10; 
      Serial.print("minutes: ");
      Serial.println(mm);
      int hr = int(input[1] - '0') + int(input[0] - '0')*10; 
      Serial.print("hours: ");
      Serial.println(hr);
      RTCtime.Minutes = mm;
      RTCtime.Hours = hr;
      M5.rtc.SetTime(&RTCtime);      
    }
    else {
      Serial.print("received wrong time format.. ");
      Serial.println(input);
    }
  }
  
  if( program_state == STATE_DEFAULT) {
    // state behavior: check and update time every second
    if(millis() > rtcTimer + 1000) {
      updateTime();
      drawTime();
      drawTimeToAlarm();      
      rtcTimer = millis();
    }
    // state behavior: read sensor and print its value to Serial port
    if(millis() > sensorTimer + 100) {
      sensorVal = analogRead(sensorPin);
      brightnessVal = map(sensorVal, 0, 4095, 0, 255);
      Serial.println(brightnessVal);
      sensorTimer = millis();    
    }
    // (OPTIONAL) state behavior: change RGB LEDs green level according to sensor value:
    // for( int i=0; i<3; i++) {
    //   pixels.setPixelColor(i, pixels.Color(0, brightnessVal, 0)); 
    //   pixels.show(); 
    // }
    pixels.setPixelColor(i++, pixels.Color(200, 0, 0));  // Bright red
    pixels.setPixelColor(j++, pixels.Color(0, 200, 0));  // Bright green
    pixels.setPixelColor(k++, pixels.Color(0, 0, 200));  // Bright blue
    pixels.show();  // sends the updated color to the hardware.

    delay(100);
  if (i == 3)
    i = 0;
  else if (j == 3)
    j = 0;
  else if (k == 3)
    k = 0;
   
    // state transition: MID button 
    if ( M5.BtnMID.wasPressed()) {
      AlarmTime = RTCtime;
      program_state = STATE_EDIT_MINUTES;
      Serial.println("program_state => STATE_EDIT_MINUTES");
    }
    // state transition: alarm time equals real time clock 
    if(AlarmTime.Hours == RTCtime.Hours && AlarmTime.Minutes == RTCtime.Minutes) {
      program_state = STATE_ALARM;
      Serial.println("program_state => STATE_ALARM");
    }
  }
  else if( program_state == STATE_EDIT_MINUTES) {
    // state behavior: blink underline under alarm minutes
    if( millis() > underlineTimer + 250 ) {
      underlineOn = !underlineOn;
      PageSprite.drawString(30, 20, "SET ALARM MINUTES:");
      drawAlarmTime();      
      if(underlineOn)
        PageSprite.FillRect(110, 110, 80, 6, 0); // underline minutes black
      else
        PageSprite.FillRect(110, 110, 80, 6, 1); // underline minutes white
      PageSprite.pushSprite();
      underlineTimer = millis();
    }
    // state transition: UP button to increase alarm minutes
    if( M5.BtnUP.wasPressed()) {
      Serial.println("BtnUP wasPressed!");
      if(AlarmTime.Minutes < 59) {
        AlarmTime.Minutes++;
        drawAlarmTime();
      }        
    }
    // another state transition: DOWN button to decrease alarm minutes
    else if( M5.BtnDOWN.wasPressed()) {
      Serial.println("BtnDOWN wasPressed!");
      if(AlarmTime.Minutes > 0) {
        AlarmTime.Minutes--;
        drawAlarmTime();
      }        
    }
    // another state transition: MID button to edit alarm hour
    else if (M5.BtnMID.wasPressed()) {
      PageSprite.FillRect(110, 110, 80, 6, 1); // underline minutes white
      program_state = STATE_EDIT_HOURS;
      Serial.println("program_state => STATE_EDIT_HOURS");
    }
  }
  else if(program_state == STATE_EDIT_HOURS) {
    // state behavior: blink underline under alarm hours
    if(millis() > underlineTimer + 250) {
      PageSprite.drawString(30, 20, " SET ALARM HOURS: ");
      underlineOn = !underlineOn;
      drawAlarmTime();
      if(underlineOn)
        PageSprite.FillRect(10, 110, 80, 6, 0); // underline hours black
      else
        PageSprite.FillRect(10, 110, 80, 6, 1); // underline hours white
      PageSprite.pushSprite();
      underlineTimer = millis();
    }
    // state behavior: increase alarm hour with UP button
    if( M5.BtnUP.wasPressed()) {
      Serial.println("BtnUP wasPressed!");
      if(AlarmTime.Hours < 23) {
        AlarmTime.Hours++;
        drawAlarmTime();
      }        
    }
    // state behavior: decrease alarm hour with DOWN button 
    else if( M5.BtnDOWN.wasPressed()) {
      Serial.println("BtnDOWN wasPressed!");
      if(AlarmTime.Hours > 0) {
        AlarmTime.Hours--;
        drawAlarmTime();
      }        
    }
    // state transition: MID button to go back to default state 
    else if (M5.BtnMID.wasPressed()) {
      //PageSprite.FillRect(10, 110, 80, 6, 1); // underline hours white
      M5.M5Ink.clear();
      PageSprite.clear(CLEAR_DRAWBUFF | CLEAR_LASTBUFF);
      M5.rtc.GetTime(&RTCtime);
      drawTime();
      
      program_state = STATE_DEFAULT;
      Serial.println("program_state => STATE_DEFAULT");
    }
  }
  else if( program_state == STATE_ALARM) {
    // state behavior: check and update time every second
    // if(millis() > rtcTimer + 1000) {
    if(millis() > beepTimer + 1000){
      M5.Speaker.tone(500,300);
      beepTimer = millis();
      updateTime();
      drawTime();
      drawTimeToAlarm();      
      rtcTimer = millis();
    }
    
if(sensorVal > 4000) { // sensor is covered by block

   if(relayTriggered == false) { // relay has not been triggered yet

      // code to turn on and off the relay to move the block:

      digitalWrite(relayPin, HIGH);  // turn on the relay

      delay(100);  // short delay

      digitalWrite(relayPin, LOW);  // turn off the relay

      relayTriggered = true;

   }

    // (OPTIONAL) state behavior: blink RGB LEDs red every 500ms
   if(millis() > ledBlinkTimer + 100) {
      if(ledBlinkOn) {
        // turn all pixels red:
        // for( int i=0; i<3; i++) {
        //   pixels.setPixelColor(i, pixels.Color(255, 0, 0)); 
        //   pixels.show(); 
        // }
        ledBlinkOn = false;    
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));  // Bright red
        pixels.setPixelColor(1, pixels.Color(0, 0, 0));  // Bright green
        pixels.setPixelColor(2, pixels.Color(0, 0, 255));  // Bright blue
        pixels.show();  // sends the updated color to the hardware.
      }
      else {
        // turn all pixels off:
        //for( int i=0; i<3; i++) {
        //  pixels.setPixelColor(i, pixels.Color(0, 0, 0)); 
        //  pixels.show(); 
        //}
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // Bright blue
        pixels.setPixelColor(1, pixels.Color(0, 0, 0));  // Bright green
        pixels.setPixelColor(2, pixels.Color(255, 0, 0));  // Bright red
        pixels.show();
        ledBlinkOn = true;
      }
      ledBlinkTimer = millis();
    }

    // while(STATE_ALARM == false) 
    
  

    // state transition: top button press to finish alarm
    // if ( M5.BtnEXT.wasPressed()) {
    // if(sensorVal > 4000) {
    else {
      sensorVal = false;
      digitalWrite(relayPin, LOW);
      Serial.println("BtnEXT wasPressed!");
      M5.M5Ink.clear();
      PageSprite.clear(CLEAR_DRAWBUFF | CLEAR_LASTBUFF);
      program_state = STATE_ALARM_FINISHED;
      Serial.println("program_state => STATE_ALARM_FINISHED");
    }
  }
  else if(program_state == STATE_ALARM_FINISHED) {
    // state behavior: check and update time every second
    if(millis() > rtcTimer + 1000) {
      updateTime();
      drawTime();
      PageSprite.drawString(50, 120, "ALARM IS OFF");
      PageSprite.pushSprite();    
      rtcTimer = millis();
    }
    // state transition: re-insert piece to return to default state 
    // if ( sensorVal > 2000) {
    //   Serial.println("restarted");
    //   M5.M5Ink.clear();
    //   PageSprite.clear(CLEAR_DRAWBUFF | CLEAR_LASTBUFF);
    //   program_state = STATE_ALARM_FINISHED;
    //   Serial.println("program_state => STATE_DEFAULT");
    // }
    // state transition: MID button 
    if ( M5.BtnMID.wasPressed()) {
      AlarmTime = RTCtime;
      program_state = STATE_EDIT_MINUTES;
      Serial.println("program_state => STATE_EDIT_MINUTES");
    }
  }
  
  M5.update();
 }
} 





// Example of controlling a servo using the scroll wheel on M5Core-Ink
// Example of receiving (subscribing to) and sending (publishing) MQTT messages
// fill in SECRET_SSID and SECRET_PASS in Secret tab to connect to the network

#include "M5CoreInk.h"          // M5Core-Ink library
#include <ESP32Servo.h>         // Servo library
#include "arduino_secrets.h"
#include <WiFi.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_NeoPixel.h>

const int rgbledPin = 25;   // bottom connector output pin (G25)

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(
    15,                      // number of pixels 
    rgbledPin,              // pin number
    NEO_GRB + NEO_KHZ800);  // LED type

// initialize WiFi connection:
WiFiClient wifi;
AdafruitIO_WiFi io(SECRET_AIO_USERNAME, SECRET_AIO_KEY, SECRET_SSID, SECRET_PASS);
AdafruitIO_Feed *catFeed = io.feed("catfeed");
AdafruitIO_Feed *ledFeed = io.feed("ledfeed");

const int servoPin = 26;        // Servo pin
Servo servoObj;                 // Servo object
int servoVal = 0;               // Servo output value
//const int buttonPin = 33;     // the number of the pushbutton pin
const int ledPin = 10;          // built-in LED pin (G10)

const int blueButtonPin = 33;  
int blueButtonVal = HIGH;
int blueButtonLast = HIGH;

const int redButtonPin = 32;  
int redButtonVal = HIGH;
int redButtonLast = HIGH;

int redVal, redValFinal;
int greenVal, greenValFinal;
int blueVal, blueValFinal;

unsigned long fadeTimer = 0;
unsigned long buttonTimer = 0;
unsigned long lastMessageTimer = 0;
bool isOpen = false;


void setup() {
  M5.begin();
  Serial.begin(9600);
  pixels.begin();  
  pixels.setBrightness(50); // set brightness to about 1/5 (max = 255)

  redVal = 0;
  greenVal = 0;
  blueVal = 0;

  redValFinal = 255;
  greenValFinal = 255;
  blueValFinal = 255;

  pinMode(servoPin, OUTPUT);
  pinMode(rgbledPin, OUTPUT);
  
  Serial.println("M5Core-Ink Servo Example");
  Serial.println("M5Core-Ink Dual-Button Unit Example");

  servoObj.attach(servoPin);    // attach Servo to servoPin

  pinMode(blueButtonPin, INPUT);
  pinMode(redButtonPin, INPUT);
  digitalWrite(rgbledPin, LOW);

    // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the ledFeed:
  catFeed->onMessage(handleMessage);
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
   catFeed->get();

  // print connection status
  Serial.println(io.statusText());
}


void loop() {
   // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();


  // check if data has been received on the Serial port:
  if (Serial.available() > 0) {
    // read red, green and blue color values:
    redValFinal = Serial.parseInt();   // read the first value as an integer
    Serial.read();  // read the comma character (,)
    greenValFinal = Serial.parseInt(); // read the second value as an integer
    Serial.read();  // read the comma character (,)
    blueValFinal = Serial.parseInt();  // read the third value as an integer
    Serial.read();  // read the newline character (\n)

    // print the received values:
    Serial.print("Received: ");
    Serial.print(redValFinal);
    Serial.print(", ");
    Serial.print(greenValFinal);
    Serial.print(", ");
    Serial.println(blueValFinal);
  }

  if(millis() > fadeTimer + 2) {  // 10 ms passed    
    // increment color values:
    if(redValFinal > redVal)
      redVal++;
    else if(redValFinal < redVal)
      redVal--;
    if(greenValFinal > greenVal)
      greenVal++;
    else if(greenValFinal < greenVal)
      greenVal--;
    if(blueValFinal > blueVal)
      blueVal++;
    else if(blueValFinal < blueVal)
      blueVal--;
    
    // set pixel colors:
    for(int i=0; i<15; i++) {
      pixels.setPixelColor(i, redVal, greenVal, blueVal);
    } 
    pixels.show(); 
    
    fadeTimer = millis();   // update timer
  }


    // read button inputs every 100ms:
  if(millis() > buttonTimer + 100) {
    
    blueButtonVal = digitalRead(blueButtonPin);
    
    if(blueButtonVal != blueButtonLast) {
      Serial.print("blue button: ");
      Serial.println(blueButtonVal);
      blueButtonLast = blueButtonVal;
      digitalWrite(servoPin, HIGH);
      digitalWrite(rgbledPin, HIGH);
      servoVal = 16;
      servoObj.write(servoVal);
      Serial.print("move servo to ");
      Serial.println(servoVal);
      Serial.println("Received: 0, 0, 255");
      
      for (int i=0; i<15; i++) {
        int redVal = map(i, 0, 255, 10, 255);
        int greenVal = map(i, 0, 255, 10, 0);
        int blueVal = map(i, 0, 255, 255, 100);
        delay(50);
        
    pixels.setPixelColor(i, redVal, greenVal, blueVal);

    pixels.show(); 
    
    fadeTimer = millis();   // update timer
  }
    }
   
    redButtonVal = digitalRead(redButtonPin);
    
    if(redButtonVal != redButtonLast) {
      Serial.print("red button: ");
      Serial.println(redButtonVal);
      redButtonLast = redButtonVal;
      digitalWrite(servoPin, LOW);
      digitalWrite(rgbledPin, LOW);
      servoVal = 130;
      servoObj.write(servoVal);
      Serial.println("move servo to ");
      Serial.println(servoVal);

      for (int i=0; i<15; i++) {
        int redVal = map(i, 0, 255, 100, 255);
        int greenVal = map(i, 0, 100, 10, 0);
        int blueVal = map(i, 0, 0, 10, 100);
        delay(50);
      

    pixels.setPixelColor(i, redVal, greenVal, blueVal);
     
    pixels.show(); 
    
    fadeTimer = millis();   // update timer
  }
    }
    
    buttonTimer = millis();
  }


  if(isOpen == true) {
    if(millis() > lastMessageTimer + 2000) {
      Serial.println("feeder time out..");
      closeFeeder();
    }
  }
  
  M5.update();
}

void handleMessage(AdafruitIO_Data *data) {
 
  Serial.print("received: ");
  Serial.println(data->value());

  if(strcmp((char*)data->value(), "kibbles")) {
    closeFeeder();
  }
  else {
    openFeeder();
  }

  lastMessageTimer = millis();
}

void closeFeeder() {
  digitalWrite(servoPin, LOW);
  digitalWrite(rgbledPin, LOW);
  servoVal = 130;
  servoObj.write(servoVal);
  Serial.print("move servo to ");
  Serial.println(servoVal);
  isOpen = false;

  for (int i=0; i<15; i++) {
    int redVal = map(i, 0, 255, 100, 255);
    int greenVal = map(i, 0, 100, 10, 0);
    int blueVal = map(i, 0, 0, 10, 100);
    delay(50);


  pixels.setPixelColor(i, redVal, greenVal, blueVal);
   
  pixels.show(); 
  
  fadeTimer = millis();   // update timer
  }
}

void openFeeder() {
  digitalWrite(servoPin, HIGH);
  digitalWrite(rgbledPin, HIGH);
  servoVal = 16;
  servoObj.write(servoVal);
  Serial.print("move servo to ");
  Serial.println(servoVal);
  isOpen = true;
  
  for (int i=0; i<15; i++) {
    int redVal = map(i, 0, 255, 10, 255);
    int greenVal = map(i, 0, 255, 10, 0);
    int blueVal = map(i, 0, 255, 255, 100);
    delay(50);
      
  pixels.setPixelColor(i, redVal, greenVal, blueVal);

  pixels.show(); 
  
  fadeTimer = millis();   // update timer
  }
}


# Introduction

##

This device meant to be a remote cat feeder that dispenses kibbles or treats when away from the pet's premises. The feeder can be operated through IFTTT. If internet connection is not ideal, the feeder can also be operated by using the mini dual button. 

# Hardware

##

Servo (for feeder lid), led strip 15 pixels, and a mini dual button.
LED strip was meant to be placed in 2 grooves in front to act as the cat’s eyes. Dual button will be recessed to the side of the enclosure.

# Firmware

##

https://github.com/danielpchoe/AlarmClock/blob/main/Final_Project_Sketch/Final_Project_Sketch.ino

```
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
```

```
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
```
# Software

##

Similar to above, explain the important software components of your project with relevant code snippets and links (for example, a link to your p5 editor sketch).

# Integrations

##

Include a link to and/or screenshots of other functional components of your project, like Adafruit IO feeds, dashboards, IFTTT applets, etc. In general, think of your audience as someone new trying to learn how to make your project and make sure to cover anything helpful to explain the functional parts of it.

# Enclosure / Mechanical Design

##

Explain how you made the enclosure or any other physical or mechanical aspects of your project with photos, screenshots of relevant files such as laser-cut patterns, 3D models, etc. (it’s great if you’re willing to share the editable source files too!)

# Project outcome

##

Summarize the results of your final project implementation and include at least 2 photos of the prototype and a video walkthrough of the functioning demo.

# Conclusion

##

As you wrap up the project, reflect on your experience of creating it. Use this as an opportunity to mention any discoveries or challenges you came across along the way. If there is anything you would have done differently, or have a chance to continue the project development given more time or resources, it’s a good way to conclude this section.

# Project references

##

Please include links to any online resources like videos or tutorials that you may have found helpful in your process of implementing the prototype. If you used any substantial code from an online resource, make sure to credit the author(s) or sources.


# Introduction

##

This device meant to be a remote cat feeder that dispenses kibbles or treats when away from the pet's premises. The feeder can be operated through IFTTT. If internet connection is not ideal, the feeder can also be operated by using the mini dual button. 

Medicine accountability tracker for relatives and loved ones using a weight sensor:

[Medicine Tracker.pdf](https://github.com/danielpchoe/AlarmClock/files/10232977/Medicine.Tracker.pdf)

Weather data and LED animation system:

[Weather Animation.pdf](https://github.com/danielpchoe/AlarmClock/files/10232998/Weather.Animation.pdf)

Final concept and selected project:

[Remote Pet Feeder.pdf](https://github.com/danielpchoe/AlarmClock/files/10232999/Remote.Pet.Feeder.pdf)


# Hardware

##

![IMG_3589](https://user-images.githubusercontent.com/118408896/207751509-d2b29678-5339-4e0f-b880-9f72d70e16f3.JPG)

Servo (for feeder lid), led strip 15 pixels, a mini dual button, and an M5CoreInk microprocessor.
LED strip was meant to be placed in 2 grooves in front to act as the cat’s eyes. Dual button will be recessed to the side of the enclosure.

Schematic:

![Final Schematic _schem](https://user-images.githubusercontent.com/118408896/207751393-92384b04-1d39-4ca7-af9f-8185e9051f75.jpg)

# Firmware

##

Link to final code:

https://github.com/danielpchoe/AlarmClock/blob/main/Final_Project_Sketch/Final_Project_Sketch.ino

The LED’s default state is white. Upon opening the feeder, pixels change to blue and chase each other individually down the strip. So essentially the eyes would blink one after the other then fade gradually back to white. Upon closing the feeder, the LEDs flash pink then gradually back to white:


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

This second portion below shows the IFTTT integration. Once the trigger is activated, feeder opens then closes automatically after a few seconds. The LED light animation remains the same as above:

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

# Integrations

##

For the IFTTT integration, the user can send an email with the subject line "#feedme" to trigger the feeder tray to open:

<img width="586" alt="Screenshot 2022-12-14 at 8 15 32 AM" src="https://user-images.githubusercontent.com/118408896/207751170-0cb5dd02-06bc-4b9b-b599-158677e0aa13.png">

<img width="1114" alt="Screenshot 2022-12-13 at 8 36 31 PM" src="https://user-images.githubusercontent.com/118408896/207748458-ec34fe19-098f-4999-928a-ac9582b28082.png">

Then the value titled "kibbles" will be received by Adafruit IO, which will then trigger open the feeder from thr IO dashboard:

<img width="367" alt="Screenshot 2022-12-13 at 8 36 40 PM" src="https://user-images.githubusercontent.com/118408896/207748480-b442753d-ab74-4899-8d70-addeca153399.png">


# Enclosure / Mechanical Design

##

The enclosure was made using MDF material which are 6.5" squares. One challenge was making sure the kibbles, which vary in weight, would use gravity to allow it to fall into the food bowl upon opening. I created a slope in the feeding canal, but also a halfpipe ramp to ensure contents are forced down as well. All the hardware was arranged in a 1.5" crevice and equal space opposing it in case need to make different positions. 

![IMG_3599](https://user-images.githubusercontent.com/118408896/207751562-b8b838d7-5ce2-4166-bc75-98de0354a9e7.JPG)

![IMG_3616](https://user-images.githubusercontent.com/118408896/207751617-e6953b1b-388f-4ec2-a9b5-eecdff7bc9cb.JPG)

Manual button is recessed into side of the enclosure:

![IMG_3628](https://user-images.githubusercontent.com/118408896/207751658-5022996a-3529-41f1-9633-1a7ba0e0def4.JPG)

The servo is connected to the feeding tray lid:

![IMG_3611](https://user-images.githubusercontent.com/118408896/207751707-ccc4ae4a-00c5-413a-8837-83d0238479c2.JPG)

# Project outcome

##

Final outcome photos:

![IMG_3633](https://user-images.githubusercontent.com/118408896/207752218-5dbc7714-f606-41af-89f3-3c81e5119e9e.JPG)

![IMG_3634](https://user-images.githubusercontent.com/118408896/207752254-68d44e36-c40d-426a-88af-6076c2e69b17.JPG)

![IMG_3635](https://user-images.githubusercontent.com/118408896/207752265-9fd9addd-49f3-477c-900e-ebd9e1a21d70.JPG)

![IMG_3636](https://user-images.githubusercontent.com/118408896/207752281-043f9c9a-18d2-426b-8dec-8b0d78735c82.JPG)

Top view: lid to open up the feeder canal to insert kibbles:

![IMG_3637](https://user-images.githubusercontent.com/118408896/207752295-d22cd0c0-d156-4a45-858a-85255eb7c46d.JPG)

# Conclusion

##

This particular assignment was one of those projects I had to just start creating and taking on the challenges head-on with improvising the final enclosure. I was so used to using cardboard for paper prototypes this entire term until now. Using something less pliable in my opinion, such as MDF in this case. Given more time and resources, One thing i'd improve on is creating a closed off portion above the feeder mouth by making it seamless and complelety closed. Also, I would like to explore and utilize different light animations then perhaps adding another sensor so that my cats can interact with the device a well. But overall this was such a great experience and I feel a lot more comfortable with coding.

# Project references

##

Please include links to any online resources like videos or tutorials that you may have found helpful in your process of implementing the prototype. If you used any substantial code from an online resource, make sure to credit the author(s) or sources.

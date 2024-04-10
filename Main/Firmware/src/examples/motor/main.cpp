#include <Arduino.h>
#include <ESP32Servo.h>

Servo esc;

#define pinPot 13
#define pinEsc 33

void setup()
{
esc.attach(pinEsc); 
// esc.writeMicroseconds(1500); 
// Serial.begin(112500);

delay(255);

// for(int i = 1500; i<=2000; i++){
//     esc.writeMicroseconds(i); 
// }
esc.writeMicroseconds(1500); 
}

void loop()
{


delay(2000); 

esc.writeMicroseconds(1800); 

}
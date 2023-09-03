#include <Arduino.h>
#include <ESP32Servo.h>

Servo esc;

#define pinPot 13
#define pinEsc 33

void setup()
{
esc.attach(pinEsc); 
esc.writeMicroseconds(1500); 
Serial.begin(112500);
}

void loop()
{
int val; 

val= analogRead(pinPot);
// Serial.print(val);
// Serial.print(" | ");

val= map(val, 0, 4096,1000,2000); 
Serial.println(val);

esc.writeMicroseconds(2000); 
}
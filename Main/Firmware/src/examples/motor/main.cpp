#include <Arduino.h>
#include <ESP32Servo.h>

Servo esc_one;
Servo esc_two;

#define pinPot 13
#define pinEsc 33

void setup()
{
esc_one.attach(pinEsc); 
esc_two.attach(32);
// esc.writeMicroseconds(1500); 
// Serial.begin(112500);

delay(255);

// for(int i = 1500; i<=2000; i++){
//     esc.writeMicroseconds(i); 
// }
esc_one.writeMicroseconds(1500); 
esc_two.writeMicroseconds(1500);
}

void loop()
{


delay(2000); 

esc_one.writeMicroseconds(1800); 
esc_two.writeMicroseconds(1800);

}
#include <AS5040.h>
#include <Arduino.h> 


// CHANGE THESE AS APPROPRIATE
#define CSpin   18
#define CLKpin  19
#define DOpin   15
#define PROGpin 5

// OUTPUT PINS
#define enc_a  14
#define enc_b  2
#define enc_z  13


AS5040 enc (CLKpin, CSpin, DOpin, PROGpin) ;

// Set mode to quadrature (A + B + index), monitor via serial IF
void setup ()
{
  Serial.begin (115200) ;   // NOTE BAUD RATE
  if (!enc.begin (AS5040_QUADRATURE, true, 0x200))  // example setting reverse sense and an offset
    Serial.println ("Error setting up AS5040") ;
}

void loop ()
{
  Serial.print (enc.read (), HEX) ;
  Serial.print ("   ") ;
  Serial.print (enc.status (), BIN) ;
  Serial.print ("   ") ;
  Serial.print (enc.valid () ? "OK" : "Fault") ;
  Serial.print ("   ") ;
  Serial.println (enc.Zaxis ()) ;
  delay (400) ;
}
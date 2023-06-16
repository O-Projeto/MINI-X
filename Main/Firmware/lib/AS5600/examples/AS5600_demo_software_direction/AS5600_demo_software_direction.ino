//
//    FILE: AS5600_demo_software_direction.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo software direction control


//  connect the DIR pin of teh AS5600 to GND


#include "AS5600.h"
#include "Wire.h"

AS5600 as5600;   //  use default Wire

uint8_t counter = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("AS5600_LIB_VERSION: ");
  Serial.println(AS5600_LIB_VERSION);

  Wire.begin();

  as5600.begin();      //  set software direction control. default param = 255
  as5600.setDirection(AS5600_CLOCK_WISE);  // default, just be explicit.
}


void loop()
{
  //  toggle direction every 10 reads.
  counter++;
  if (counter < 10) as5600.setDirection(AS5600_CLOCK_WISE);
  else              as5600.setDirection(AS5600_COUNTERCLOCK_WISE);
  if (counter >= 20) counter = 0;
  
  Serial.print(millis());
  Serial.print("\t");
  Serial.print(as5600.getDirection());
  Serial.print("\t");
  Serial.print(as5600.readAngle());
  Serial.print("\t");
  Serial.println(as5600.rawAngle() * AS5600_RAW_TO_DEGREES);

  delay(1000);
}


// -- END OF FILE --

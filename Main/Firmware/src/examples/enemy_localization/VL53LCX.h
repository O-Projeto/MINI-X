
#include <Wire.h>        /*include Wire.h library*/
#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h> 

TwoWire Wire2CX((uint8_t)sizeof(uint8_t));

class VL53LCX
{
private:
    bool data_ready; 
    float avg ,sum ; 
    int closer_distance; 
    int SDA,SCL;

    SparkFun_VL53L5CX myImager;
    VL53L5CX_ResultsData VL53L5_data; // Result data class structure, 1356 byes of RAM

    int imageResolution = 0; //Used to pretty print output
    int imageWidth = 0; //Used to pretty print output




public:
    float position,distance ; 
    VL53LCX(int SDA, int SCL );
    bool update();
    void init();
    ~VL53LCX(); 
};

VL53LCX::VL53LCX(int SDA, int SCL )
{   
    this->SDA = SDA ;
    this->SCL = SCL;

}

void VL53LCX::init(){
    //  Serial.println("SparkFun VL53L5CX Imager Example");
    Wire2CX.setPins(SDA,SCL);
    Wire2CX.begin();

    // Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz

    // myImager.setWireMaxPacketSize(64); //Increase default from 32 bytes to 128 - not supported on all platforms
    uint8_t CX_ADDR=0x29;
    bool startup = myImager.begin(CX_ADDR,Wire2CX);
    
    if (startup == false)
    {
        Serial.println(F("Sensor not found - check your wiring. Freezing"));
        while (1) ;
    }

    myImager.setResolution(8*8); //Enable all 16 pads
    myImager.setSharpenerPercent(50);
    myImager.setRangingFrequency(15);

    imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution); //Calculate printing width

    myImager.startRanging();
}


bool VL53LCX::update(){

  data_ready = false ; 
  avg = 0; 
  sum = 0; 
  closer_distance = 10000; 

    if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&VL53L5_data)) //Read distance data into array
    {
      // The ST library returns the data transposed from zone mapping shown in datasheet
      //Pretty-print data with increasing y, decreasing x to reflect reality
      // for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      // {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          // Serial.print("\t");
          // Serial.print(VL53L5_data.distance_mm[x]);

          avg += VL53L5_data.distance_mm[x]*x*1000;
          sum += VL53L5_data.distance_mm[x];

          if(VL53L5_data.distance_mm[x]<closer_distance){
              closer_distance = VL53L5_data.distance_mm[x];
          }
        }
        // Serial.print(avg/sum);
        // Serial.println();
        distance = closer_distance ; 
        position =  avg/sum;


      // }
      // Serial.println();
    }
    data_ready = true ; 
  }

return data_ready ; 
}


VL53LCX::~VL53LCX()
{
}

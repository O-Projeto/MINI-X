#include <Wire.h>        /*include Wire.h library*/
#include <SparkFun_VL53L5CX_Library.h> 

class VL_Fodao {
private:
    int _pinSDA;
    int _pinSCL;
    float avg;
    float sum;
    int imageResolution = 0; // Used to pretty print output
    int imageWidth = 0;       // Used to pretty print output
    SparkFun_VL53L5CX myImager;
    VL53L5CX_ResultsData VL53L5_data;
    TwoWire Wire2CX;

public:
    VL_Fodao(int pinSDA, int pinSCL);
    void init();
    float VL53L5_get_position();
    float VL53L5_get_distance();
};

VL_Fodao::VL_Fodao(int pinSDA, int pinSCL) : _pinSDA(pinSDA), _pinSCL(pinSCL), Wire2CX(Wire) {
}

void VL_Fodao::init() {
    Serial.println("SparkFun VL53L5CX Imager Example");
    Wire2CX.begin(_pinSDA, _pinSCL);

    Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");

    long startTime = millis();
    uint8_t CX_ADDR = 0x29;
    bool startup = myImager.begin(CX_ADDR, Wire2CX);
    long stopTime = millis();

    if (!startup) {
        Serial.println(F("Sensor not found - check your wiring. Freezing"));
        while (1);
    }

    Serial.print("Firmware transfer time: ");
    float timeTaken = (stopTime - startTime) / 1000.0;
    Serial.print(timeTaken, 3);
    Serial.println("s");

    myImager.setResolution(8 * 8); // Habilita todos os 16 pads
    myImager.setSharpenerPercent(50);
    myImager.setRangingFrequency(15);

    imageResolution = myImager.getResolution();
    imageWidth = sqrt(imageResolution);

    myImager.startRanging();
}

float VL_Fodao::VL53L5_get_position() {
    avg = 0;
    sum = 0;

    if (myImager.isDataReady()) {
        if (myImager.getRangingData(&VL53L5_data)) {
            for (int x = imageWidth - 1; x >= 0; x--) {
                avg += VL53L5_data.distance_mm[x] * x * 1000;
                sum += VL53L5_data.distance_mm[x];
            }
            return avg / sum;
        }
    }
    return 0.0;
}

float VL_Fodao::VL53L5_get_distance() {
    int closer_distance = 10000;

    if (myImager.isDataReady()) {
        if (myImager.getRangingData(&VL53L5_data)) {
            for (int x = imageWidth - 1; x >= 0; x--) {
                if (VL53L5_data.distance_mm[x] < closer_distance) {
                    closer_distance = VL53L5_data.distance_mm[x];
                }
            }
            return closer_distance;
        }
    }
    return 0.0;
}
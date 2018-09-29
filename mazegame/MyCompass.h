#ifndef MyCompass_h
#define MyCompass_h
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SparkFunLSM9DS1.h>
#include "Arduino.h"


// I2C Setup
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

#define CALIBRATION_MILLIS 10000     // If CALIBRATE, for how long? (ms)

class MyCompass {
  public:
    MyCompass();
    void
      setOffsets(int x, int y, int z),
      calibrate(Adafruit_SSD1306& display, LSM9DS1& imu, uint32_t calibrationTime),
      update(LSM9DS1& imu);
    String  
      convertHeadingToCardinal(float heading);
    int
      convertCardinalToHeading(String cardinal);
    float heading;
  private:
    void
      displayCalibration(Adafruit_SSD1306& display, int secondsLeft);
};

#endif // MYCOMPASS_H

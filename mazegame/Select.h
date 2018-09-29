#ifndef Select_h
#define Select_h
#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"

class KerberosSelect{
  public:
    KerberosSelect(int button_1, int button_2, int led);
    String playerkerb;
    void
      update(Adafruit_SSD1306& display);      
};

class LevelSelect{
  public:
    LevelSelect(int button_1, int button_2, int led);
    int chosenlevel;
    int req_chests;
    void
      update(Adafruit_SSD1306& display);
};

#endif //SELECT_H

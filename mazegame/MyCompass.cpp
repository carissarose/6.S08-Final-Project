#include "MyCompass.h"
#include "Arduino.h"

int offx, offy, offz;
float heading;
float const declination = -14.75; //For Cambridge, MA area

//class MyCompass{
  
  MyCompass::MyCompass() {
    offx = 0; //You can change use this as default and forego calibration, if you want
    offy = 0; //You can change use this as default and forego calibration, if you want
    offz = 0; //You can change use this as default and forego calibration, if you want
    heading = 0;
  }

  
  void MyCompass::displayCalibration(Adafruit_SSD1306& display, int secondsLeft) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Please rotate device on a flat surface to calibrate");
    display.print("Time left: ");
    display.println(secondsLeft);
    display.display();
  }
  
  void MyCompass::calibrate(Adafruit_SSD1306& display, LSM9DS1& imu, uint32_t calibrationTime) {
    //Initial values for min_x, max_x, etc, will all get overwritten for sure
    int min_x = 32767;
    int min_y = 32767;
    int min_z = 32767;
    int max_x = -32768;
    int max_y = -32768;
    int max_z = -32768;
    uint32_t startTime = millis();
    int lastSecondsLeft = -1; //initial value doesn't mean anything
    while (millis() - startTime < calibrationTime) {
      imu.readMag();
      min_x = min(min_x, imu.mx);
      max_x = max(max_x, imu.mx);
      min_y = min(min_y, imu.my);
      max_y = max(max_y, imu.my);
      min_z = min(min_z, imu.mz);
      max_z = max(max_z, imu.mz);
      
      //update display only once per second
      int secondsLeft = (calibrationTime - (millis() - startTime))/1000;
      if (secondsLeft != lastSecondsLeft) {
        displayCalibration(display, secondsLeft);
        lastSecondsLeft = secondsLeft;
      }
    }
    
    offx = (min_x+max_x)/2;
    offy = (min_y+max_y)/2;
    offz = 0; //With flat turning, we can't tell "hard" interference with earth magnetism
    Serial.println("Magnetic offsets found");
    Serial.print("Offx: ");
    Serial.println(offx);
    Serial.print("Offy: ");
    Serial.println(offy);
    Serial.print("Offz: ");
    Serial.println(offz);
  }
  void MyCompass::update(LSM9DS1& imu) {
    // Get magnetometer reading
    imu.readMag();
    int mx = -1*(imu.mx-offx); // Aligning with diagram on the board
    int my = imu.my-offy;
    int mz = imu.mz-offz;
  
    // Get accelerometer reading
    imu.readAccel();
    int ax = -1*imu.ax; // Aligning with diagram on the board
    int ay = -1*imu.ay; // Aligning with diagram on the board
    int az = -1*imu.az; // Aligning with diagram on the board

    // Calculate component of magnetism that is perpendicular to gravity 
    float gravMagnitude = 1.0*(mx*ax + my*ay + mz*az) / (ax*ax + ay*ay + az*az);
    float northx = mx - gravMagnitude * ax;
    float northy = my - gravMagnitude * ay;

    //note: northAngle is "math-y", so it's 0 on +x-axis and increases counterclockwise
    float northAngle = -1*atan2(northy, northx)*180/PI; //Heading is based on X,Y components of North
    northAngle += declination; //Account for declination
    heading = -northAngle;  // Find our heading, given northAngle
    heading += 90; //Change axes: Now 0 degrees is straight ahead (+y-axis)
    heading = int(heading+360)%360 + (heading - int(heading)); //Hack-y mod operation for floats
    Serial.println(heading);
  }
  //Converts a heading (not necessarily between 0 and 360) to cardinal direction
  String MyCompass::convertHeadingToCardinal(float heading) {
    heading = float(int(heading)%360);
    Serial.print("Heading: ");
    Serial.println(heading);
    if (heading < 0) {
      heading += 360; //Make sure we're in 0-360 range
    }

    if (heading >= 45 && heading < 135) {
      return "w";
    }
    else if (heading >= 135 && heading < 225) {
      return "s";
    }
    else if (heading >= 225 && heading < 315) {
      return "e";
    }
    else {
      return "n";
    }
  }

  // Converts cardinal ("n","e", "s", "w") to heading (0, 90, 180, 270)
  // Returns -1 if we give some other string as input
  int MyCompass::convertCardinalToHeading(String cardinal) {
    if (cardinal=="N" || cardinal=="n") {
      return 0;
    } else if (cardinal=="W" || cardinal=="w") {
      return 90;
    } else if (cardinal=="S" || cardinal=="s") {
      return 180;
    } else if (cardinal =="E" || cardinal == "e") {
      return 270;
    } else {
      return -1;
    }
  }


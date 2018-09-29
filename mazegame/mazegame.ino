#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ESP8266.h>
//#include <Snooze.h>
#include "MyCompass.h"
#include "Select.h"

// I2C Setup
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

// turnturnrevolution info
//#define serialYes true
#define BAR_HEIGHT 16

// Initialize library classes
#define OLED_RESET 4
#define VERBOSE_WIFI true // Verbose ESP8266 output

#define WIFI_CH_PD_PIN 2 // pin connecting Teensy to WiFi
uint8_t tempRegValue=0;  // holds register value from IMU
SnoozeBlock config;

Adafruit_SSD1306 display(OLED_RESET);
ESP8266 wifi(VERBOSE_WIFI);
LSM9DS1 imu;


int button_1 = 12;
int button_2 = 11;
int led = 13;
    
// Display constants, and variables
#define DISPLAY_UPDATE_INTERVAL 100  // How often to update display (ms)
uint32_t tLastDisplayUpdate = 0;     // time of last display update
#define CALIBRATION_MILLIS 10000     // If CALIBRATE, for how long? (ms)

// stepcounter info
#define ARRAYSIZE 10                // number of accel elements to hold
#define wifiSerial Serial1          // for ESP chip
#define serialYes true             // print to serial port

// Wifi options, constants, and variables
#define IOT true
#define IOT_UPDATE_INTERVAL 10000  // How often to send/pull from cloud (ms)
#define SSID "MIT"               // PUT SSID HERE
#define PASSWORD ""         // PUT PASSWORD HERE
const String KERBEROS = "gadsoncr";  // PUT KERBEROS HERE
uint32_t tLastIotReq = 0;       // time of last send/pull
uint32_t tLastIotResp = 0;      // time of last response
String MAC = "";
String resp = "";

int const UPDATEINTERVAL = 2000;    // how often to update display (ms)
int const MA = 4;                    // length of moving average (samples)
int const STEPCOUNTING = 1;         // are we counting steps?
//int const DISPLAYING = 0;           // are we sending to display?
int const DISPLAYING = 1;           // are we sending to display?
int const MINIMUMSTEPTIME = 200;    // minimum time between steps (ms)


float stepCount = 0;                  // # of steps
int tLastUpdate = 0;                // time since last screen update
int iottLastUpdate = 0;             // time since last screen update
int waitTime = 0;                   // time since last step
float accelData[ARRAYSIZE] = {0};   // initialize array holding accel data
float ax, ay, az, magAccel; 



char maze_array[18][13][4];
int user_y_pos = 2;
int user_x_pos = 3;
/*
class PowerMonitor {
  const float BATTERY_CAPACITY = 10;     // battery capacity in mA-h
  const float BATTERY_VOLTAGE = 5.0;       // V
  
  // ALL CURRENTS IN MILLIAMPS! Fill in with your values
  const float OLED_ON_CURRENT = 10.0; // Make assumption about % of pixels lit
  const float OLED_SLEEP_CURRENT = 0.008;

  const float ACCEL_ON_CURRENT = 0.32;
  const float ACCEL_SLEEP_CURRENT = 0.0;
  const float GYRO_ON_CURRENT = 0.0; // Isn't used, so doesn't need to be given current
  const float GYRO_SLEEP_CURRENT = 0.0; // Isn't used, so doesn't need to be given current
  const float MAG_ON_CURRENT = 1.03;
  const float MAG_SLEEP_CURRENT = 0.0;
  
  const float GPS_ON_CURRENT = 25.3;
  const float GPS_SLEEP_CURRENT = 0.0;
  
  const float WIFI_NOTCONNECTED_CURRENT = 71.4;
  const float WIFI_DEEPSLEEP_CURRENT = 21.3;
  const float WIFI_MODEM_SLEEP_CURRENT = 18.5;

  const float TEENSY_ON_CURRENT = 36.1;       
  const float TEENSY_SLEEP_CURRENT = 0.0;
  const float TEENSY_DEEPSLEEP_CURRENT = 0.0;

  // All VOLTAGES IN VOLTS!  Fill in with your values 
  const float TEENSY_SUPPLY_VOLTAGE = 0.361;
  const float GPS_SUPPLY_VOLTAGE = 2.53;
  const float WIFI_SUPPLY_VOLTAGE = 0.0175;
  const float OLED_SUPPLY_VOLTAGE = 0.25;
  const float IMU_SUPPLY_VOLTAGE = 0.0103;

  // These hold state of the various components
  uint8_t state_teensy, state_gps, state_wifi, state_oled, state_imu;
  bool iswifi, isgps, isoled, isimu;    // do these exist (1) or not (0)
  long int timestamp;                   // current time (in millis)
  long int time_offset;                 // holds interval for when 
                                        // Teensy is sleeping, since millis() 
                                        // doesn't run during Teensy sleep

  float energyConsumed;     // running total of energy consumed, in mJ
  float batteryCapacity;    // calculated battery capacity, in mJ
  float percentEnergyLeft;  // percent of battery energy left

  public:

  /* Constructor. Takes in one 8-bit input, c, that determines which
  components are being used in our system. We assume Teensy is always present!
  Other components each take 1 bit, as shown below:   
        c: 0 0 0 0 IMU OLED GPS WIFI
        So 00001101 = 0x0D would mean GPS is not present   
  PowerMonitor(uint8_t c)
  {     

    timestamp = millis();
    energyConsumed = 0;
    batteryCapacity = BATTERY_CAPACITY * BATTERY_VOLTAGE * 3600; // mJoules
    percentEnergyLeft = 100;  // Start out at 100%!


    iswifi = isgps = isoled = isimu = false;    // Assume no components
    if (c | 0x01 << 0) iswifi = true;     // Add components depending on bits in c
    if (c | 0x01 << 1) isgps = true;
    if (c | 0x01 << 2) isoled = true;
    if (c | 0x01 << 3) isimu = true;    

    // initially, all components are on
    state_teensy = 2;   //0=deepsleep, 1 = sleep, 2 = normal
    state_gps = 1;      //0=sleep, 1 = on
    state_wifi = 1;     //0=sleep, 1 = on aka modem-sleep
    state_oled = 1;     //0=sleep, 1 = on
    state_imu = 0x07; //[accel,gyro,mag], 0=sleep, 1 =on
  }

  /* Determine the energy consumed since the last update. Updates 
  class variables energyConsumed and percentEnergyLeft 
  void updateEnergy() {
    long int dT = millis() + time_offset - timestamp; //Time since last update
    float pw = 0;   // power usage during this interval

    // Depending on state of each component and whether it is present, 
    // choose appropriate power setting
    if (state_teensy==2) {
      pw += TEENSY_ON_CURRENT * TEENSY_SUPPLY_VOLTAGE;
    } else if (state_teensy==1) {
      pw += TEENSY_SLEEP_CURRENT * TEENSY_SUPPLY_VOLTAGE;
    } else if (state_teensy==0) {
      pw += TEENSY_DEEPSLEEP_CURRENT * TEENSY_SUPPLY_VOLTAGE;
    }
    if (isgps) pw += (state_gps) ? GPS_ON_CURRENT*GPS_SUPPLY_VOLTAGE : GPS_SLEEP_CURRENT*GPS_SUPPLY_VOLTAGE;    
    if (isoled) pw += (state_oled) ? OLED_ON_CURRENT*OLED_SUPPLY_VOLTAGE : OLED_SLEEP_CURRENT*OLED_SUPPLY_VOLTAGE;    
    if (isimu) {
      pw += (state_imu & 0x01) ? ACCEL_ON_CURRENT*IMU_SUPPLY_VOLTAGE : ACCEL_SLEEP_CURRENT*IMU_SUPPLY_VOLTAGE;    
      pw += (state_imu & (0x01 << 1)) ? GYRO_ON_CURRENT*IMU_SUPPLY_VOLTAGE : GYRO_SLEEP_CURRENT*IMU_SUPPLY_VOLTAGE;    
      pw += (state_imu & (0x01 << 2)) ? MAG_ON_CURRENT*IMU_SUPPLY_VOLTAGE : MAG_SLEEP_CURRENT*IMU_SUPPLY_VOLTAGE;    
    }

    // WiFi power depends on whether we are connected to AP, and based on 
    // how many transmit and receive operations we have undertaken
    if (iswifi) {
      if (state_wifi) {
        if (wifi.isConnected()) {
          pw += WIFI_MODEM_SLEEP_CURRENT*WIFI_SUPPLY_VOLTAGE;
          } else {
          pw += WIFI_NOTCONNECTED_CURRENT*WIFI_SUPPLY_VOLTAGE;
          }
      } else {
        pw += WIFI_DEEPSLEEP_CURRENT*WIFI_SUPPLY_VOLTAGE;
      }
    }
    float deltaEnergy = dT / 1000.0 * pw;
    energyConsumed += deltaEnergy;  // mJ
    percentEnergyLeft -= deltaEnergy / batteryCapacity * 100;
    timestamp = millis() + time_offset;
  }
  
  /* Set power mode of any particular component 
  Inputs:
   Arduino String p: can be "teensy", "wifi", "oled", "imu", "gps"
   8-bit integer m: value depends on component 
  void setPowerMode(String p, uint8_t m){
    // Before changing power mode, update the energy consumed for last interval
    updateEnergy();   

    // Wifi modes are 1: On, 0: DeepSleep
    if (iswifi && (p=="wifi")) {
      if (m != state_wifi) {
        if (m==1) {
          digitalWrite(WIFI_CH_PD_PIN, HIGH);
        } else if (m==0) {
          Serial.println("Turning off wifi");
          digitalWrite(WIFI_CH_PD_PIN, LOW);
        }
        state_wifi = m;
      }
      
    }
    /*  IMU modes are more complicated because IMU has 3 peripherals. Thus we
      use the three least-significant bits in m to hold the desired state of
      each sensor: [0][0][0][0][0][accel][gyro][mag].  So 0x05 means accel and mag on, 
      gyro sleep 
    else if (isimu && (p=="imu")) {
      // Cannot have gyro ON and accel OFF.  Right now this is not
      // explicitly forbidden, but will not result in intended behavior,
      // and will create inaccuracy in estimated current.
      if (m != state_imu) {
        uint8_t tempRegValue=0;   // Hold register value
        uint8_t dbits = ~ (m ^ state_imu);    // Find which bits are different
        if  (dbits & (0x01 << 2) ) {    // If accel bit is diff
          tempRegValue = imu.xgReadByte(CTRL_REG6_XL);  // read accel register
          if (m & (0x01 << 2)) {    // if we want to turn ON,
            tempRegValue |= 0xE0;   // set bits 7:5 to 1, keep other bits
            imu.xgWriteByte(CTRL_REG6_XL, tempRegValue);
          } else {                  // turn off
            tempRegValue &= 0x1F;   // set bits 7:5 to 0, keep other bits
            imu.xgWriteByte(CTRL_REG6_XL, tempRegValue);
          }
        }
        if  (dbits & (0x01 << 1) ) {  // If gyro bit is different
          tempRegValue = imu.xgReadByte(CTRL_REG1_G);  
          if (m & (0x01 << 1)) {
            tempRegValue |= 0xE0;   //set bits 7:5 to 1, keep other bits
            imu.xgWriteByte(CTRL_REG1_G, tempRegValue);
          } else {
            tempRegValue &= 0x1F;   //set bits 7:5 to 0, keep other bits
            imu.xgWriteByte(CTRL_REG1_G, tempRegValue);
          }
        }
        if  (dbits & 0x01) {        // If mag bit is different
          tempRegValue = imu.mReadByte(CTRL_REG3_M);  
          if (m & 0x01) {
            tempRegValue &= 0xFC;   //set bits 1:0 to 0, keep other bits
            imu.mWriteByte(CTRL_REG3_M, tempRegValue);
          } else {
            tempRegValue |= 0x03;   //set bits 1:0 to 1, keep other bits
            imu.mWriteByte(CTRL_REG3_M, tempRegValue);
          }
        }
        state_imu = m;
      }
    }
  }
  // Returns energyConsumed for use by outside world
  float getEnergyConsumed() {
      return energyConsumed;
  }
// Returns percentEnergyLeft for use by outside world
  float getBatteryLeft() {
      return percentEnergyLeft;
  }
  float getTotalElapsedTime() {
    return millis() + time_offset;
  }
};*/

//PowerMonitor pm(0x0F);

//initialize an instance of the MyCompass class
MyCompass compass;


//define player class
class Player
{
  int radius = 2; //radius of Player object

  public:
  double x_pos; //x position
  double y_pos; //y position
  int x_offset;
  int y_offset;
  int previous_pos;
  int num_chests; //records number of chests user collected

  
  Player(int rad=2) {
    x_pos = 64;
    y_pos = 32;
    radius = rad;
    x_offset = 0;
    y_offset = 0;
    previous_pos = 1;
    num_chests = -1;
  }

  public:
  void didTakeStep(){
    // To read from the accelerometer, you must first call the
    // readAccel() function. When this exits, it'll update the
    // imu.ax, imu.ay, and imu.az variables with the most current data.
    imu.readAccel();
    
    // calculate acceleration in g's and store in ax, ay, and az
    ax = float(imu.ax) /32767 * 2;
    ay = float(imu.ay) /32767 * 2;
    az = float(imu.az) /32767 * 2;
  
    // calculate the acceleration magnitude
    magAccel = sqrt(ax*ax + ay*ay + az*az);

    if (STEPCOUNTING) {                   
      // shift new data into array
      updateArray(accelData, magAccel);

      // smooth the data with moving-average filter
      float smData[ARRAYSIZE] = {0};  
      smoothData(accelData, smData, MA);

      // look for peak, if peak is above threshold AND enough time has passed 
      // since last step, then this is a new step
      float peak = findPeak(smData);
      Serial.print("Peak: ");
      Serial.println(peak);

      if (peak > 1.04 && millis() - waitTime >= UPDATEINTERVAL){
        stepCount = 1;
        waitTime = millis();
        movePlayer();
      }
     
    }
    
    stepCount = 0;
  }
  // shift array once to left, add in new datapoint
  void updateArray(float *ar, float newData)
  {
    int s = ARRAYSIZE;
    for (int i=0; i<=s-2; i++) {
      ar[i] = ar[i+1];
    }
    ar[s-1] = newData;
  }


  // m point moving average filter of array ain
  void smoothData(float *ain, float *aout, int m)
  {
    int s = ARRAYSIZE;
    for (int n = 0; n < s ; n++) {
      int kmin = n>(m-1) ? n - m + 1: 0;
      aout[n] = 0;
  
      for (int k = kmin; k <= n; k++) {
        int d = m > n+1 ? n+1 : m;
        aout[n] += ain[k] / d;
      }
    }
  }


  // find peak in the array, not-very-good algorithm (O(n))
  float findPeak(float *ar)
  {
    int s = ARRAYSIZE;   // s is length of vector ar
    float p = 0;
    for (int i = 1; i<s-1; i++) {
      if ((ar[i] >= ar[i-1]) && (ar[i] >= ar[i+1])) {
        p = ar[i];
      }
    }
    return p;
  }

  //call functions from Compass class to calculate proper heading, then check is step is made
  //If a step is made, call check_move depending on the heading
  void movePlayer(){
    String cardinalDir = compass.convertHeadingToCardinal(compass.heading);
    float setAngle = compass.convertCardinalToHeading(cardinalDir);
    //Serial.println(stepCount);
    Serial.println(setAngle);
    if (stepCount >= 1) {
      if (setAngle == 0){
        //y_offset -= (int)(stepCount*2);
        Serial.println("decrease y");
        check_move(0, -1);
      }else if (setAngle == 180){
        //y_offset += (int)(stepCount*2);
        Serial.println("increase y");
        check_move(0, 1);
      }else if (setAngle == 90){
        //x_offset -= (int)(stepCount*2);
        Serial.println("decrease x");
        check_move(-1, 0);
      }else if (setAngle == 270){
        //x_offset += (int)(stepCount*2);
        Serial.println("increase x");
        check_move(1, 0);
      }
    }
    
  }
  //check if move is valid
  void check_move(int x_change, int y_change){
    //check if move is valid, considering all conditions. If one of these sets of conditions is met, move user object in array
    if ((x_change < 0 && maze_array[user_y_pos][user_x_pos][0] == ' ') || (x_change > 0 && maze_array[user_y_pos][user_x_pos + x_change][0] == ' ') || (maze_array[user_y_pos + y_change][user_x_pos + x_change][1] == ' ')){
          Serial.println("We can move!");
          y_offset += y_change;
          Serial.print("y_offset: ");
          Serial.println(y_offset);
          x_offset += x_change;
          Serial.print("x_offset: ");
          Serial.println(x_offset);
          maze_array[user_y_pos][user_x_pos][1] = ' ';
          user_y_pos += y_change;
          user_x_pos += x_change;
          Serial.print("User x position: ");
          Serial.println(user_x_pos);
          Serial.print("User y position: ");
          Serial.println(user_y_pos);
          Serial.print("specific pos: ");
          Serial.println(previous_pos);
          maze_array[user_y_pos][user_x_pos][1] = '=';
    }
    //if previous conditions were not met, check if instead the desired move is to collect a chest.
    //if so, collect the chest, increment score by 1, and move the user object to that position
    else if (maze_array[user_y_pos + y_change][user_x_pos + x_change][1] == 'o'){
          num_chests += 1; // update number of chests collected
          Serial.println("We can move!");
          y_offset += y_change;
          Serial.print("y_offset: ");
          Serial.println(y_offset);
          x_offset += x_change;
          Serial.print("x_offset: ");
          Serial.println(x_offset);
          maze_array[user_y_pos][user_x_pos][previous_pos] = ' ';
          user_y_pos += y_change;
          user_x_pos += x_change;
          Serial.print("User x position: ");
          Serial.println(user_x_pos);
          Serial.print("User y position: ");
          Serial.println(user_y_pos);
          Serial.print("specific pos: ");
          Serial.println(previous_pos);
          maze_array[user_y_pos][user_x_pos][1] = '=';
    }
  }
  //display user object in center of screen
  void updateDisplay() {
    display.fillCircle(x_pos, y_pos, 2, WHITE);
    display.display();
  }
};

// game class
class Game {
  float starttime; //starting time of game play
  String action; //tells Python what to store in the database
  int hor; //defines the horizontal size of maze_array
  int ver; //defines the vertical size of maze_array
  int count;
  String str_resp; //store WIFI response
  String str_maze; //store maze response without html tags
  String str_times; //store times response without html tags
  char mazeString[665];
  int counter;
  
  public:
  float endtime; //ending time of game play
  Player user; //instance of Player class associated with Game
  KerberosSelect ker; //instance of KerberosSelect class associated with Game
  LevelSelect lev; //instance of LevelSelect class associated with Game
  Game(): user(4), ker(button_1, button_2, led), lev(button_1, button_2, led)
  {
    starttime = 0; //start time at 0, reassign at beginning of play
    endtime = 0; //end time at 0, reassign at end of play
    action = ""; //there is no required action yet  
    hor = 0; //maze hasn't been retrieved yet
    ver = 0; //maze hasn't been retrieved yet
    count = 0;
    counter = 0;
  }

  void startGame(){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);

    display.println("Welcome to pirate\ntraining 101!\n");
    display.println("Travel through a mazeand collect chests\nquickly.");
    display.display();
    delay(4000);
    
    display.clearDisplay();
    display.setCursor(0,16);
    display.println("The number of chests you need to collect  depends on the level.");
    display.display();
    delay(2500);

    display.clearDisplay();
    display.setCursor(0,16);
    display.println("Before we start, tellus your kerberos and choose a level!");
    display.display();
    delay(2500);
  }
  //function to run the game! Calls the functions that have been defined
  void runGame(){
    // after starting game, collect start time
    if (ker.playerkerb == "") {
      ker.update(display); 
    }
    else if (lev.chosenlevel == 0){
      lev.update(display);
    }
    else if (counter == 0){
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0,0);
      display.print("Hello ");
      display.print(ker.playerkerb);
      display.println("!\n");
      display.print("Since you chose level");
      display.print(lev.chosenlevel);
      display.print(", you need to\ncollect ");
      display.print(lev.req_chests);
      display.print(" chests!");
      display.display();
      delay(3000);
      display.clearDisplay();
      display.setCursor(16,20);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.println("Okay, let's go!");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(54,20);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.println("3...");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(54,20);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.println("2...");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(54,20);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.println("1...");
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setCursor(26,16);
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.println("START!");
      display.display();
      delay(500);
      getMaze();
      make_maze();
      //pm.setPowerMode("wifi", 0); // turn wifi off
      counter++;
    }
    else {
      if (user.num_chests == -1){
        Serial.println("num_chests = -1!!!");
        starttime = millis();
        Serial.println(starttime);
        user.num_chests = 0; 
      }
      else if (user.num_chests != lev.req_chests){
        compass.update(imu); // update the heading angle
        user.didTakeStep();
        move_maze();
      }
      // once game is finished, collect end time
      else if (user.num_chests == lev.req_chests){
        endtime = millis();
        Serial.println(endtime);
      }
    }
  }
  
  void endGame(){
    //pm.setPowerMode("imu", 0);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.println("Good job!");
    display.setTextSize(1);
    display.println("Now let's see how youdid compared to\nothers.");
    display.display();
    delay(3000);

    getLowestTimes();
    displayLowestTimes();
  }
  //get the five lowest times with corresponding kerberos from the database
  void getLowestTimes(){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("Getting the five besttimes from the\ndatabase...");
    display.display();
    
    action = "storeTimes";


    if (IOT && wifi.hasResponse()){
      resp = wifi.getResponse();
      //tLastIotResp = millis();
    }
    Serial.println("POST request");
    // create POST request to store times, then extract best times from the database
    if (wifi.isConnected() && !wifi.isBusy()) { //Check if we can send request
      Serial.println("making post request now!"); 
      String domain = "iesc-s2.mit.edu";
      int port = 80;
      String path = "/student_code/" + KERBEROS + "/dev1/sb1.py";
      String postParams = "kerberos=" + ker.playerkerb + "&level=" + (String)lev.chosenlevel + "&starttime=" + (String)starttime + "&endtime=" + (String)endtime + "&action=" + action;

      wifi.sendRequest(POST, domain, port, path, postParams);

      unsigned long start_time = millis();
      while (!wifi.hasResponse() && millis() - start_time < 10000);
      
      if (wifi.hasResponse()){
        str_resp = wifi.getResponse();
        Serial.println("We have a response!");
      
        int html = str_resp.indexOf("<html>");
        int end_html = str_resp.indexOf("</html>");
        str_times = str_resp.substring(html + 7, end_html); 
        
        Serial.println(str_times);
      }
     }
     else {
      Serial.println("No response");
     }
  }

  void displayLowestTimes(){
    display.clearDisplay();
    display.setCursor(30,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("Best Times\n");
    display.setTextSize(.3);
    display.println(str_times);
    display.display();
  }
  //function to communicate with Python file to extract maze from database
  void getMaze(){
    //assign action to create the maze in Python
    Serial.println("Generating random\nmaze...");
    
    action = "getMaze";


    if (IOT && wifi.hasResponse()){
      resp = wifi.getResponse();
      //tLastIotResp = millis();
    }
    Serial.println("GET request");
    // create GET request to extract a maze from the database
    if (wifi.isConnected() && !wifi.isBusy()) { //Check if we can send request
      Serial.println("making post request now!"); 
      String domain = "iesc-s2.mit.edu";
      int port = 80;
      String path = "/student_code/" + KERBEROS + "/dev1/sb1.py";
      String getParams = "kerberos=" + ker.playerkerb + "&level=" + (String)lev.chosenlevel + "&action=" + action;

      wifi.sendRequest(GET, domain, port, path, getParams);

      unsigned long start_time = millis();
      while (!wifi.hasResponse() && millis() - start_time < 10000);
      
      if (wifi.hasResponse()){
        str_resp = wifi.getResponse();
        Serial.println("We have a response!");
      
        int html = str_resp.indexOf("<html>");
        int end_html = str_resp.indexOf("</html>");
        str_maze = str_resp.substring(html + 7, end_html); 
        
        Serial.println(str_maze);
      }
      else {
      Serial.println("No response");
      }
     }
  }
  
  void make_maze(){
    // reformat mazeString into a an array of arrays
    Serial.println("making mazeString into 2D array");
    int num;
    for (num = 0; num < 665; num++){
      mazeString[num] = str_maze[num];
    }
    
    String temp_string;
     
    int i;
    int j;
    int index;
     
    for (i = 0; i < 17; i++){
     //Serial.println(i);
     for (j = 0; j < 13; j++){
       //Serial.println(j);
       index = i*39 + j*3;
         if (j == 12){
           //at the end of the maze (the right side), there is only one character to be stored, either a '|' or a '+'
           //maze_array[i][j] = temp_string;
           maze_array[i][j][0] = mazeString[index];
           maze_array[i][j][1] = '\0';
         }
         else{
           //add series of '|  ' and '+--' as separate characters
           maze_array[i][j][0] = mazeString[index];
           maze_array[i][j][1] = mazeString[index+1];
           maze_array[i][j][2] = mazeString[index+2];
           maze_array[i][j][3] = '\0';
         }
        
      }   
     }
     // user object assigned inital position
     maze_array[user_y_pos][user_x_pos][1] = '=';
  }

  void move_maze(){
    // print the maze
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    // print horizontal part of maze
    int x;
    int y;
    int x1, x2, y1, y2;
    for (y = 0; y < 17; y++){
      for (x = 0; x < 7; x++){
        //if the character checked is a -, replace that set of characters with a horizontal line
        if (maze_array[y + user.y_offset][x+user.x_offset][1] == '-'){
          x1 = 2 + x*18;
          x2 = x1 + 18;
          y1 = 3 + y*12;    
          display.drawLine(x1, y1, x2, y1, WHITE);
         }
         //if the character check is a |, replace that character with a vertical line
         else if (maze_array[y+user.y_offset][x+user.x_offset][0] == '|'){
          x1 = 2 + x*18;
          y1 = 3 + (y-1)*12;
          y2 = y1 + 23;
          display.drawLine(x1, y1, x1, y2, WHITE);

          //if the character is an o, draw an o on the screen that corresponds to where it exists in the array
          if (maze_array[y+user.y_offset][x+user.x_offset][1] == 'o'){
            if (y == 1){
              x1 = 2 + 20*x;
              y1 = 10*y + 10;
              display.drawCircle(x1, y1, 2, WHITE); 
            }
            else {
              x1 = 8 + 20*x - x*1.85;
              y1 = 10*y + 22;
              display.drawCircle(x1, y1, 2, WHITE); 
            }
          }
         }
            //Serial.print(maze_array[y][x]);
      }
        display.print('\n');
        //Serial.print('\n');
    }
     user.updateDisplay();
     display.display();
  }
};

Game game;

void setup() {
  //display setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(250);
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(1);  
  Serial.begin(115200);

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Oh, hi!\n");
  display.println("Let me get myself\nset up before we get started...");
  display.display();
  //delay(3000);

  pinMode(WIFI_CH_PD_PIN, OUTPUT);    // set pin for WiFi power-down
  digitalWrite(WIFI_CH_PD_PIN, HIGH);
  
  // Wifi setup
  if (IOT) {
    wifi.begin();
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Connecting to:");
    display.println(SSID);
    display.display();
    wifi.connectWifi(SSID, PASSWORD);
    while (!wifi.isConnected()); //wait for connection
    MAC = wifi.getMAC();
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Connected!!");
    display.display();
    delay(1000);
  }
  
  // IMU setup
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  // The above lines will only take effect AFTER calling
  // imu.begin(), which verifies communication with the IMU
  // and turns it on.
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1){
      Serial.println("Comm Failure with LSM9DS1");
      delay(500);
    }
  }
  
  compass.calibrate(display, imu, CALIBRATION_MILLIS);
  display.clearDisplay();
  display.setCursor(46,20);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Thanks!");
  display.display();
  delay(1500);
  game.startGame();
  pinMode(button_1, INPUT_PULLUP);
  pinMode(button_2, INPUT_PULLUP);
}
int counter = 0;
int check = 0;
void loop() {
  if (game.endtime == 0){
    game.runGame();
  }
  else if (game.endtime != 0 && counter == 0){
    game.endGame();
    counter++;
  }
}

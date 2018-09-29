#include "Select.h"
#include "Arduino.h"

// kerberos select class
  // array of kerberoi in 6.S08
  String kerbs[53] = {"adat", "ahuangg", "ajhall", "akabay", "akmartin", "aladentan", "allanc", "allisonn", "amya", "anshula", "brendaz", "canepa", "cmlang", "cnord", "davebam", "diana96", "dmendels", "dnsmith", "dominikm", "egmontes", "emile", "erickgar", "gadsoncr", "gbelton", "hartz", "hcope", "heyer", "houssam", "iinvent", "infiloop", "jamirose", "jazmineh", "jdulsky", "jennycxu", "jodalyst", "kenneth", "lahuang4", "lapentab", "larry", "marcinja", "matiash", "maxallen", "mchoun95", "molodan", "quacht", "scottcam", "sqshemet", "tiensori", "voldman", "wbraun", "wickj", "\0"};
  int limit; //limit to define end of array
  float iterator; //interates through array
  int select_button; 
  int iterate_button; 
  String currentkerb; //current place in array
  String playerkerb; //chosen kerberos
  

  KerberosSelect::KerberosSelect(int button_1, int button_2, int led){
    limit = 53; //last place in array
    iterator = 0; //start at beginning of array
    select_button = button_1; //pin 11
    iterate_button = button_2; //pin 12
    led = led;
    playerkerb = ""; //no kerberos chosen
    currentkerb = ""; //start with no current kerberos
  }
  
  void KerberosSelect::update(Adafruit_SSD1306& display){
    //if iterate button is pressed, iterate through array
    if (digitalRead(iterate_button) == 0) {
      Serial.println("iterate_button has been pressed!");
      iterator++;
      Serial.println(iterator);
    }
    
    //if at the end of the array, go back to start
    if (iterator >= limit) {
      iterator = 0;
      Serial.println("Looping to start");
    }
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println("Select your kerberos:");
    display.print(currentkerb);
    display.display();
    
    currentkerb = kerbs[(int)(iterator)];
    //if select button is pressed, assign currentkerb to playerkerb 
    if (digitalRead(select_button) == 0) {
      playerkerb = currentkerb;
      Serial.print("Selected player: ");
      Serial.println(playerkerb);
      iterator = 0;
  
    }
    //display.clearDisplay();
    display.display();
  }

// level select class
  String options[4] = {"1", "2", "3", "\0"};
  int levelLimit; //limit of level array
  String currentlevel; //current position in the array
  String level; //string version of level selected
  int req_chests; //required number of chests to be collected depending on level
  int chosenlevel; //int version of level selected

  
  LevelSelect::LevelSelect(int button_1, int button_2, int led){
    levelLimit = 3; //end of array
    iterator = 0; //start a first position
    select_button = button_1;
    iterate_button = button_2;
    led = led;
    level = ""; //start with no chosen level
    currentlevel = ""; //start with no current level
    chosenlevel = 0; //no chosen level
    req_chests = 0; //required chests not determined yet
  }
  
  void LevelSelect::update(Adafruit_SSD1306& display){
    //if iterator button pressed, iterate through array
    if (digitalRead(iterate_button) == 0) {
      Serial.println("iterate_button has been pressed!");
      iterator += 1;
      Serial.println(iterator);
    }
    
    //if at limit, start at beginning of array
    if (iterator >= levelLimit) {
      iterator = 0;
      Serial.println("Looping to start");
    }
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.print("Select your level: ");
    display.print(currentlevel);
    display.display();
    currentlevel = options[(int)(iterator)];
    //if select button is pressed, change level to int
    if (digitalRead(select_button) == 0) {
      level = currentlevel;
      chosenlevel = level.toInt();
      Serial.print("Selected level: ");
      Serial.println(chosenlevel);

      //depending on chosen level, req_chests is determined
      if (chosenlevel == 1){
        req_chests = 2;
      }
      else if (chosenlevel == 2){
        req_chests = 4;
      }
      else if (chosenlevel == 3){
        req_chests = 6;
      }
    }
    
  }

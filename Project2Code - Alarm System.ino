//Libraries
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <NewPing.h>
#include <NewTone.h>

/*-----------------------KEYPAD-----------------------*/
const byte numRows= 4; //Rows on Keypad
const byte numCols= 4; //Columns on Keypad
char keypressed;
//keymap defines the key pressed
char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
//Shows connections of keypad to arduino
byte rowPins[numRows] = {22,23,24,25};//Rows 0 to 3
byte colPins[numCols] = {26,27,28,29};//Columns 0 to 3             
//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

/*--------------------------CONSTANTS-------------------------*/
const int buzzer = 8;      //Buzzer/small speaker
const int doorMagSen = 34;
const int windowMagSen =35;
int pirPin1 =12;
int soundSensor =2;
const int threshold=200;
int redLED = 52;
int greenLED = 53;
int blueLED = 51;
// Constants for Sensors etc



LiquidCrystal lcd(48, 47, 46, 45, 44, 43); // Pins for LCD
NewPing sonar(A0,A0,2000); // Pin for ping sensor and distance
/*--------------------------VARIABLES------------------------*/
String password="2580"; //Variable to store the current password
String tempPassword=""; //Variable to store the input password
int doublecheck;
boolean armed = false;  //Variable for system state (armed:true / unarmed:false)
boolean input_pass;   //Variable for input password (correct:true / wrong:false)
boolean storedPassword = true;
boolean changedPassword = false;
boolean checkPassword = false;
int distance;
int i = 1; //variable to index an array

/**********************************************************************************/

void setup() {
  
  lcd.begin(16, 2);                 //Setup the LCD's number of columns and rows 
  pinMode(doorMagSen,INPUT_PULLUP);       //Set all magnetic sensors as input withn internal pullup resistor
  pinMode(windowMagSen,INPUT_PULLUP);
  pinMode(pirPin1, INPUT); 
  pinMode(soundSensor,INPUT);
  pinMode(redLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(blueLED,OUTPUT); 
}

void loop() { //Main loop
  
  if (armed){
    systemIsArmed();  //Run function to activate the system
  }
  else if (!armed){
    systemIsUnarmed();  //Run fuction to de activate the system
  }
}

/********************************FUNCTIONS************************************/

//While system is unarmed
void systemIsUnarmed(){
  int screenMsg=0;
  lcd.clear();                  //Clear lcd
  unsigned long previousMillis = 0;           //To make a delay by using millis() function
  const long interval = 5000;           //delay will be 5 sec. 
                          //every "page"-msg of lcd will change every 5 sec
  while(!armed){  
    digitalWrite(redLED, HIGH);                            //While system is unarmed do...
    unsigned long currentMillis = millis();   //Store the current run-time of the system (millis function)
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if(screenMsg==0){           //First page-message of lcd
        lcd.setCursor(0,0);
        lcd.print("   SYSTEM OFF ");
        lcd.setCursor(0,1);
        lcd.print("   ----------");
        screenMsg=1;
        
        }
        else{                 //Second page-message of lcd
          lcd.setCursor(0,0);
          lcd.print("A-Activate        ");
          lcd.setCursor(0,1);
        lcd.print("B-Change Pass      ");
        screenMsg=0;
        }
      }
    keypressed = myKeypad.getKey();       //Read the pressed button
    if (keypressed =='A'){            //If A is pressed, activate the system
      NewTone(buzzer,500,200);
      systemIsArmed();            //by calling the systemIsArmed function
    }
    else if (keypressed =='B'){//If B is pressed, change current password
      doublecheck=0;
      NewTone(buzzer,500,200);
      storedPassword=false;
      if(!changedPassword){         //By calling the changePassword function
        changePassword();
      }
    }
  }
}

//While system is armed
void systemIsArmed(){               
  lcd.clear();
  int count=10;               //Countdown of 10 seconds before system is activated
  unsigned long previousMillis = 0;         
  const long interval = 1000; 
  while(!armed){    
    distance = sonar.ping_cm(); //Store distance from sensor only for first time
    //While system is unarmed - for 10sed do...
    lcd.setCursor(0,0);
    lcd.print("System Will Be ");      //Print message to LCD
    lcd.setCursor(0,1);
    lcd.print("Powered On In ");
    unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        //Screen counter 10sec
        if (count>1){
        count--;            //Countdown timer
        }
        else{
          armed=true;       //Activate the system!
        break;
        }
      }
    lcd.setCursor(15,1);
    lcd.print(count);           //Shows timer on LCD in 15 position
  }
  while (armed){  
    digitalWrite(greenLED,HIGH);
    digitalWrite(redLED,LOW);//While system is armed do...
    lcd.setCursor(0,0);
    lcd.print("   ACTIVATED!    ");
    lcd.setCursor(0,1);
    lcd.print("----------------");
    int door = digitalRead(doorMagSen);   //Read sensors to check status
    int window = digitalRead(windowMagSen);
    int curr_distanse = sonar.ping_cm();
    int pir = digitalRead(pirPin1);
    int statusSensor = digitalRead(soundSensor);
    
    

    //Check values
  
    if (door==HIGH){
     
      unlockPassword(); //Disarm the system with correct password
    }

    if (window==HIGH){

      alarmFunction(); // Go to Alarm Function

    }

    if(pir==HIGH){
     
      alarmFunction();
     
    }

    if(statusSensor == 1){
      alarmFunction();
    }
   
        //Ultrasonic sensor code
    if (curr_distanse < (distance -5)){
                   //Check distanse (minus 5 for safety) with current distanse
            
      alarmFunction();
     
    }
  }
}
//Door is opend, unlcok the system!
void unlockPassword() {
  int count=31;             //20 sec for alarm!
  retry:                  //label for goto, retry in case of wrong password
    tempPassword="";            //reset temp password (typing...)
  lcd.clear();              //clear lcd
  i=6;                  //variable to put * while typing pass
  unsigned long previousMillis = 0;       
  const long interval = 1000;
  boolean buzzerState = false;      //variable to help us make  a beep NewTone
  while(!checkPassword){          //While waiting for correct password do...
    unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; //play beep NewTone every 1 sec
        if (!buzzerState){
          NewTone(buzzer, 700);
          buzzerState=true;
        }
        else{
          noNewTone(buzzer);
          buzzerState=false;
        }
        if (count>0){           //Screen counter 20sec
        count--;
        }
        else{
          alarmFunction();      //Times is up, ALARM!
          break;
        }
      }
    keypressed = myKeypad.getKey();
    lcd.setCursor(0,0);
    lcd.print("ALARM IN: "); 
    //For screen counter - 20sec
    if (count>=10){
      lcd.setCursor(14,0);
      lcd.print(count);       //print countdown timer at lcd
    }
    else{               //catch '0'bellow 10 (eg 09)
      lcd.setCursor(14,0);
      lcd.print(" ");
      lcd.print(count);
    }
    lcd.setCursor(0,1);
    lcd.print("PASS>");
    if (keypressed != NO_KEY){      //Accept only numbers and * from keypad
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i,1);
        lcd.print("*");       //Put * on lcd
        i++;
        NewTone(buzzer,500,200);    //Button NewTone
      }
      else if (keypressed == '*'){  //Check for password
        if (password==tempPassword){//If it's correct unarmed the system
          lcd.clear();
          armed=false;
          digitalWrite(greenLED,LOW);
          
          
          NewTone(buzzer,700,500);
          break;
        }
        else{           //if it's false, retry
          tempPassword="";
          NewTone(buzzer,200,200);
          delay(300);
          NewTone(buzzer,200,200);
          delay(300);
          goto retry;
        }
      }
    }
  } 
  
}



                                                      




//Alarm
void alarmFunction(){
  digitalWrite(greenLED,LOW);
  digitalWrite(blueLED,HIGH);
  retry: //label for goto
  tempPassword="";
  lcd.clear();
  i=6;
  unsigned long previousMillis = 0;       
  const long interval = 500;
  boolean buzzerState = false;

  while(!checkPassword){  //Waiting for password to deactivate the alarm...
    unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; //Play a beep NewTone every 0.5 second
        if (!buzzerState){
          NewTone(buzzer, 700);
          buzzerState=true;
        }
        else{
          noNewTone(buzzer);
          buzzerState=false;
        }
      }
  
  
    keypressed = myKeypad.getKey();
    
    lcd.setCursor(0,0);
    lcd.print("Zone Breached"); 
    lcd.setCursor(0,4);
    lcd.print("Pass>");
    if (keypressed != NO_KEY){      //Accept only numbers and *
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i,1);
        lcd.print("*");
        i++;    
      
       
      
      }
      else if (keypressed == '*'){
        if (password==tempPassword){
          armed=false;
          NewTone(buzzer,700,500);
          digitalWrite(redLED,LOW);
          digitalWrite(greenLED,LOW);
          digitalWrite(blueLED,LOW);
          break;
        }
        else{
          tempPassword="";
          NewTone(buzzer,200,200);
          delay(300);
          NewTone(buzzer,200,200);
          delay(300);
          goto retry;
  
        }
      }
    }
  } 
}
//Change current password
void changePassword(){
  retry: //label for goto
  tempPassword="";
  lcd.clear();
  i=1;
  while(!changedPassword){        //Waiting for current password
    keypressed = myKeypad.getKey();   //Read pressed keys
    lcd.setCursor(0,0);
    lcd.print("CURRENT PASSWORD");
    lcd.setCursor(0,1);
    lcd.print(">");
    if (keypressed != NO_KEY){
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i,1);
        lcd.print("*");
        i++;
        NewTone(buzzer,800,200);        
      }
      else if (keypressed=='#'){
        break;
      }
      else if (keypressed == '*'){
        i=1;
        if (password==tempPassword){
          storedPassword=false;
          NewTone(buzzer,500,200);
          newPassword();          //Password is corrent, so call the newPassword function
          break;
        }
        else{               //Try again
          tempPassword="";
          NewTone(buzzer,500,200);
          delay(300);
          NewTone(buzzer,500,200);
          delay(300);
          goto retry;
        }
      }
    }
  }
}
String firstpass;
//Setup new password
void newPassword(){
  tempPassword="";
  changedPassword=false;
  lcd.clear();
  i=1;
  while(!storedPassword){
    keypressed = myKeypad.getKey();   //Read pressed keys
    if (doublecheck==0){
      lcd.setCursor(0,0);
      lcd.print("SET NEW PASSWORD");
      lcd.setCursor(0,1);
      lcd.print(">");
    }
    else{
      lcd.setCursor(0,0);
      lcd.print("Enter Again");
      lcd.setCursor(0,1);
      lcd.print(">");
    }
    if (keypressed != NO_KEY){
      if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
      keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
      keypressed == '8' || keypressed == '9' ){
        tempPassword += keypressed;
        lcd.setCursor(i,1);
        lcd.print("*");
        i++;
          NewTone(buzzer,800,200);
      }
      else if (keypressed=='#'){
        break;
      }
      else if (keypressed == '*'){
        if (doublecheck == 0){
          firstpass=tempPassword;
          doublecheck=1;
          newPassword();
        }
        if (doublecheck==1){
          doublecheck=0;
          if (firstpass==tempPassword){
            i=1;
            firstpass="";
            password = tempPassword; // New password saved
            tempPassword="";//erase temp password
            lcd.setCursor(0,0);
            lcd.print("PASSWORD CHANGED");
            lcd.setCursor(0,1);
            lcd.print("----------------");
              storedPassword=true;
              NewTone(buzzer,500,400);
              delay(2000);
              lcd.clear();
              break;
          }
          else{
            firstpass="";
            newPassword();
          }
        }
      } 
    }
  }
}

#include <MQ2.h> // gas sensor library
#include <LiquidCrystal.h> // lcd library
#include <SoftwareSerial.h>// import the serial library
SoftwareSerial myserial(2, 3); // RX, TX
#include <dht.h> // library for temp & humd
#include <Wire.h> // allows you to communicate with I2C / TWI devices
#include <Adafruit_BMP085.h> // library for barometer

dht DHT;
char Data[38]; // Set Length of String
int temp = 0, humi = 0; // variables for sensors
int co = 0, smoke = 0;
#define DHT11_PIN A0 // defining pin for temp & humd
int pin = A1; // setting pin variable as a1

int sensorPin1 = A7;  //soil
int sensorValue1 = 0;  // value from sensor
int percent1 = 0; // percentage value

int sensorPin2 = A6;  //rain
int sensorValue2 = 0;  
int percent2 = 0;
int bpm = 0,b1 = 0,b2 = 0,b3 = 0, b4 = 0;

String str; // intialising str variable as string
MQ2 mq2(pin); // analog pin is set by calling pin variable
Adafruit_BMP085 bmp;

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // pinouts for lcd

void setup() {
  lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display

  Serial.begin(115200); // sets the data rate in bits per second (baud) for serial data transmission

  
  myserial.begin(115200);
  mq2.begin();
  bmp.begin(); 
  delay(2000);

}



void loop()
{
  DHT.read11(DHT11_PIN); // reading the pin for dht 11
  humi = DHT.humidity; // readings for sensors are stored in variables
  temp = DHT.temperature;
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  sensorValue1 = analogRead(sensorPin1); // Read value for soil sensor and give it variable
  sensorValue1 = constrain(sensorValue1, 85, 540); 
  percent1 = convertToPercent1(sensorValue1); //reading is converted into percentage
  
  sensorValue2 = analogRead(sensorPin2);
  sensorValue2 = constrain(sensorValue2, 85, 290); 
  percent2 = convertToPercent2(sensorValue2);
  bpm = bmp.readPressure()/100;



  Serial.print("To ESP "); // Print to Arduino Serial
  Serial.print("H: ");
  Serial.print(humi);
  Serial.print("% ");
  Serial.print(" T: ");
  Serial.print(temp);
  Serial.print(char(176)); // It should display character for celcius * Broken
  
  Serial.println("C");
  Serial.print("Co: ");
  Serial.print(co);
  Serial.print("Ppm ");
  Serial.print("Smoke: ");
  Serial.print(smoke);
  Serial.println("Ppm");
  
  Serial.print("Pressure = ");
  Serial.print(bpm);
  Serial.println(" HPa");

  Serial.print(percent1);
  Serial.println("% Soil");
  Serial.print(percent2);
  Serial.println("% Rain");
  LCDDisplay();
  
    humi=humi+1000; // To get correct reading 1000 had to be added to the value which is sent over serial. 
    temp=temp+1000;  // Done to keep position of value within the string 
    co=co+1000; // uses last 3 digits of 1000 to complete calculation or to get values from 0 to 100. It allows for value to drop to below 10 also. 
    smoke=smoke+1000;
    percent1=percent1+1000;
    percent2=percent2+1000;

 
  str = (String('H') + String(humi) + String('T') + String(temp)+ String('B') + String(10000+(bmp.readPressure())/100)+ String('S') + String(percent1)+ String('R')+ String(percent2)+ String('C')+ String(co)+ String('M')+ String(smoke)); //H69T32
  myserial.println(str);
  
  
  
}

int convertToPercent1(int value)//For soil moisture
{
  int percentValue1 = 0;
  percentValue1 = map(value, 85, 540, 0, 100); // first two are limits for sensor. Next two are new limits (Percentage)
  return percentValue1;
}

int convertToPercent2(int value)//For Rain sensor
{
  int percentValue2 = 0;
  percentValue2 = map(value, 85, 290, 0, 100);
  return percentValue2;
}

void LCDDisplay(){ 
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");
  
  lcd.setCursor(0, 1);
  lcd.print("Humi: ");
  lcd.print(humi);
  lcd.print(" %");
  delay(1500);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Co: ");
  lcd.print(co);
  lcd.print(" Ppm");
  
  lcd.setCursor(0, 1);
  lcd.print("Smoke: ");
  lcd.print(smoke);
  lcd.print(" Ppm");
  delay(1500);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Pressure: ");
  lcd.print(bpm);
  lcd.print(" Hpa");
  
  lcd.setCursor(0, 1);
  lcd.print("Soil: ");
  lcd.print(percent1);
  lcd.print(" %");
  delay(1500);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Rain: ");
  lcd.print(percent2);
  lcd.print(" %");
  delay(1500);
  lcd.clear();
}

//You can use SDA and SCL as general purpose input or output pins, including analog input on an Uno.

//Only if you need to communicate with an I2C device do they become dedicated, and then only if you use the I2C hardware.

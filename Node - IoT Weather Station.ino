#include <ESP8266WiFi.h> // Library for esp wifi module
#include <BlynkSimpleEsp8266.h> // Blynk library
#include "Adafruit_MQTT.h" // Library for publishing values to adafruit
#include "Adafruit_MQTT_Client.h" // // Establishes connection with the client


/************************* Thingspeak Setup *********************************/

const char* server = "api.thingspeak.com"; // connects to the thingspeak server
String apiKey = "6MA9GR5SOLEEKZ30"; // allows for values to be written to specified fields 

/************************* Blynk Setup *************************************/

char auth[] = "28aef35898034625b07d09621834e119"; // Blynks write key, allows for values to be written and updated


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "FRITZ!Box 4040 OZ"           //  defines ssid
#define WLAN_PASS       "icanttellyou" //  defines password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" // defines adafruit server
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "chukles"          // adafruit username         
#define AIO_KEY         "67d33fece0d143f08f8fafae916cbb05" // adafruit write key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/****************************** Feeds ***************************************/

// Setup a feed for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Publish Humi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish Temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish Co = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Co");
Adafruit_MQTT_Publish Smoke = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Smoke");

Adafruit_MQTT_Publish Pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Pressure");
Adafruit_MQTT_Publish Rain = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Rain");
Adafruit_MQTT_Publish Soil = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Soil");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

char Data[38]; // Expected length of string
int h1=0,h2=0,h3=0,h4=0,t1=0,t2=0,t3=0,t4=0,b1=0,b2=0,b3=0,b4=0,b5=0,s1=0,s2=0,s3=0,s4=0; 
int ra1=0,ra2=0,ra3=0,ra4=0,c3=0,c1=0,c2=0,c4=0,r3=0,sm1=0,sm2=0,sm3=0,sm4=0; // Integer variables for string values from arduino
unsigned long lastrecorded = 0;
BlynkTimer timer; // starts blynk timer
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200); // sets baud rate
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to "); // prints information relating to connection status
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Blynk.begin(auth, WLAN_SSID, WLAN_PASS, "blynk-cloud.com", 8080); // connects to blynk
  timer.setInterval(1000L, sendSensor); // sets interval for sendSensor function
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected with: ");Serial.println(WLAN_SSID);
  Serial.println("IP address: "); Serial.println(WiFi.localIP()); // prints info relating to connection 
  delay(1000); // one second delay
}


void loop() { // run over and over
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  Blynk.run(); // Establish connection with blynk and run continously 
  MQTT_connect();
  unsigned long now = millis(); // Returns the number of milliseconds passed since the Arduino board began running the current program. This number will overflow (go back to zero), after approximately 50 days.
if(Serial.available()) { // if serial port available 
    for(int i=0;i<38;i++){ // for the length of the string
    Data[i] = Serial.read(); // read the string
    //Serial.print(i);
    Serial.print(Data[i]); // print the string 
    //Serial.println();
    delay(5); // five second delay
    }
  if(Data[0] =='H'){ // if the string begins with letter H
    Serial.println();
     h1 = Data[2]-48; // Changes character sent from i to char. When character 1 is sent, Serial.read() returns 49.
     h2 = Data[3]-48;
     h3 = Data[4]-48;
     h4 = 100*h1+10*h2+h3; // Calculation from string, stores as variable which is published to the cloud.
     
     t1 = Data[7]-48;
     t2 = Data[8]-48;
     t3 = Data[9]-48;
     t4 = 100*t1+10*t2+t3;
    
     b1 = Data[12]-48;
     b2 = Data[13]-48;
     b3 = Data[14]-48;
     b4 = Data[15]-48;
     b5 = 1000*b1+100*b2+10*b3+b4;


     s1 = Data[18]-48;
     s2 = Data[19]-48;
     s3 = Data[20]-48;
     s4 = 100*s1+10*s2+s3;

     ra1 = Data[23]-48;
     ra2 = Data[24]-48;
     ra3 = Data[25]-48;
     ra4 = 100*ra1+10*ra2+ra3;

     c1 = Data[28]-48;
     c2 = Data[29]-48;
     c3 = Data[30]-48;
     c4 = 100*c1+10*c2+c3;
     
     sm1 = Data[33]-48;
     sm2 = Data[34]-48;
     sm3 = Data[35]-48;
     sm4 = 100*sm1+10*sm2+sm3;
    
     
     
    
      Serial.println(h4); // prints sensor values to serial
      Serial.println(t4); 
      Serial.println(b5);
      Serial.println(s4);
      Serial.println(ra4);
      Serial.println(c4);
      Serial.println(sm4);
      
     sendSensor(); // runs the function sendsensor
     timer.run(); // runs the timer
  }
 
   
 if(now > lastrecorded + 15000 ){    // Attempts to publish every 15 seconds
     //Publish sensor values to the Adafruit broker
     ///////////////////////////////
  if (! Humi.publish(h4)) 
  {
    Serial.println(F("Failed"));
    }
    else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Body Humidity value "));
    Serial.print(h4);
    Serial.print("...");  
     //Publish the value to the Adafruit broker
   ///////////////////////////////////////
  if (! Temp.publish(t4))  // if it fails to publish, print failed to serial, else print Ok.
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Temperature value "));
    Serial.print(t4);
    Serial.print("..."); 
/////////////////////////////////////
     if (! Co.publish(c4)) 
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Co value "));
    Serial.print(c4);
    Serial.print("..."); 
////////////////////////////////////
     if (! Smoke.publish(sm4)) 
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Smoke value "));
    Serial.print(sm4);
    Serial.print("..."); 
    ///////////////////////////////****
         if (! Pressure.publish(b5)) 
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Smoke value "));
    Serial.print(b5);
    Serial.print("..."); 
    /////////////////////////////////
         if (! Rain.publish(ra4)) 
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Smoke value "));
    Serial.print(ra4);
    Serial.print("..."); 
    ////////////////////////////////
         if (! Soil.publish(s4)) 
  {
    Serial.println(F("Failed"));
    } else 
    {
      Serial.println(F("OK!"));
      }
    Serial.print(F("\nSending Soil value "));
    Serial.print(s4);
    Serial.print("..."); 
    //////////////////////////////////
    
lastrecorded = now; // Flag 
}
}
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
  void MQTT_connect() {
  int8_t ret; // used for 8 bit data calculation

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();

  uint8_t retries = 3;
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }    
  Serial.println("MQTT Connected!");
 }

 void sendSensor(){
    Blynk.virtualWrite(V2,h4); // writes sensor values to virtual pins within the blynk app. 
    Blynk.virtualWrite(V3,t4);
    Blynk.virtualWrite(V4,c4);
    Blynk.virtualWrite(V5,sm4);
    Blynk.virtualWrite(V6,ra4);
    Blynk.virtualWrite(V7,s4);
    Blynk.virtualWrite(V8,b5);

    if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey; // Create a string postStr and let it equal the declared apikey. 
    postStr +="&field1=";
    postStr += String(t4); // writes sensor value to specified fields on thingspeak. 
    postStr +="&field2=";
    postStr += String(h4);
    postStr +="&field3=";
    postStr += String(sm4);
    postStr +="&field4=";
    postStr += String(c4);
    postStr +="&field5=";
    postStr += String(ra4);
    postStr +="&field6=";
    postStr += String(s4);
    postStr +="&field7=";
    postStr += String(b5);
    postStr += "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
  //  
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n\n\n\n\n");
    client.print(postStr);
  }
  
    client.stop(); // stop the client
   // delay(20000);
 
    
 }  
  
 

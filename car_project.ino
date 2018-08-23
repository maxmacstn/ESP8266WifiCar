#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>  
 
//Set Wifi ssid and password
char ssid[] = "<SSID>";
char pass[] = "<password>";
 
//Define the pins
//int STBY = 12; //standby
//Motor A
int PWMA = 12; //Speed control
int AIN1 = 16; //Direction
int AIN2 = 5; //Direction
//Motor B
int PWMB = 13; //Speed control
int BIN1 = 4; //Direction
int BIN2 = 0; //Direction

ESP8266WebServer server (80);
WiFiManager wifiManager;
//This function takes the parameters passed in the URL(the x and y coordinates of the joystick)
//and sets the motor speed based on those parameters. 
void handleJSData(){
  boolean yDir;
  int x = server.arg(0).toInt() * 10;
  int y = server.arg(1).toInt() * 10;
  int aSpeed = abs(y);
  int bSpeed = abs(y);
  //set the direction based on y being negative or positive
  if ( y < 0 ){
    yDir = 0; 
  }
  else { 
    yDir = 1;
  }  
  //adjust to speed of each each motor depending on the x-axis
  //it slows down one motor and speeds up the other proportionately 
  //based on the amount of turning
  aSpeed = constrain(aSpeed + x/2, 0, 1023);
  bSpeed = constrain(bSpeed - x/2, 0, 1023);
 
  //use the speed and direction values to turn the motors
  //if either motor is going in reverse from what is expected,
  //just change the 2 digitalWrite lines for both motors:
  //!ydir would become ydir, and ydir would become !ydir
  //digitalWrite(STBY, HIGH);  
  //MotorA
  Serial.println("motor1");
  digitalWrite(AIN1, !yDir);
  digitalWrite(AIN2, yDir);
  analogWrite(PWMA, aSpeed);
  //MotorB
  Serial.println("motor2");
  digitalWrite(BIN1, !yDir);
  digitalWrite(BIN2, yDir);
  analogWrite(PWMB, bSpeed);
 
  //return an HTTP 200
  server.send(200, "text/plain", "");   
}
 
void setup()
{
  /*
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //set the pins as outputs
  pinMode(STBY, OUTPUT);
  */
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
 
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);  
  // Debug console
  Serial.begin(115200);
  //initialize SPIFFS to be able to serve up the static HTML files. 
  if (!SPIFFS.begin()){
    Serial.println("SPIFFS Mount failed");
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
  }
  //set the static pages on SPIFFS for the html and js

  //Init wifi
  //wifiManager.autoConnect();
  String macAddress = WiFi.macAddress();


   String wifiNameStr = "Wifi Car - " + macAddress.substring(9,11) + macAddress.substring(12,14) + macAddress.substring(15,18);
   char wifiName[wifiNameStr.length()+1];
   wifiNameStr.toCharArray(wifiName,wifiNameStr.length()+1);

   Serial.println("Wifi name :" + wifiNameStr);
   Serial.println("Mac address:" + macAddress);
   Serial.println(WiFi.softAP(wifiName,"12345678")? "Wifi Started succesfully":"Wifi Start failed"); //begin WiFi access point



  server.serveStatic("/", SPIFFS, "/joystick.html"); 
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/jsData.html", handleJSData);  
  server.begin();
  Serial.println("Server started");
}

void loop()
{
  server.handleClient();  
}

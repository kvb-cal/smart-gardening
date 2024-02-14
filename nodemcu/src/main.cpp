//Mini project 
//Smart Gardening System using IOT
#include <Arduino.h>

#define BLYNK_TEMPLATE_ID "TMPL3i9ei3v_"
#define BLYNK_DEVICE_NAME "Smart Gardening System using IOT"
#define BLYNK_AUTH_TOKEN "AS5XMgp-bbsM5ge0_2s7t7cTAqmdxhRq"

//ESP8266WIFI.h library
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>   
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

// Adafruit GFX Library
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

// Adafruit SSD1306
#include <Adafruit_SSD1306.h>
#include <splash.h>

// Wire Library for I2C
#include <Wire.h>

// Set OLED size in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Set OLED parameters
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT sensor library
#include<DHT.h>
#include <DHT_U.h>


//DHT sensor digital pin
int DHTPIN = 14;
#define DHTTYPE DHT11

//Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Constant for dry sensor
const int DryValue = 1023;

// Constant for wet sensor
const int WetValue = 317;

//Soil moisture sensor input Port
int sm_sensor = A0;

//Relay control pin
int RELAY_OUT = 12;

// Define variables for Temperature and Humidity
float temp;
float hum;

// Variables for soil moisture
int moisture_value;
int soilMoisturePercent;

//Variable for trigger level
int pump_trigger;

// Pump Status Text
String pump_status_text = "OFF";

//turn pump on
void pumpOn();

//turn pump off
void pumpOff();

// Prints to OLED and holds display for delay_time
void printOLED(int, String, int, String, int);

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "AndroidAP5EAD";  // type your wifi name
char pass[] = "indo556kvb";  // type your wifi password

//Get the pump trigger value
BLYNK_WRITE(V4)
{
 pump_trigger = param.asInt();
}

BlynkTimer timer;

void sendSensor()
{
  //Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  hum = dht.readHumidity();

  // Read temperature as Celsius (the default)
  temp = dht.readTemperature();

  //Get the analog sensor value
  moisture_value= analogRead(sm_sensor);

  // Determine soil moisture percentage value
  soilMoisturePercent = map(moisture_value, DryValue, WetValue, 0, 100);

  // Keep values between 0 and 100
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

  // Check if any reads failed and exit early (to try again).
   if (isnan(hum) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, temp);
    Blynk.virtualWrite(V1, hum);
    Blynk.virtualWrite(V2, soilMoisturePercent);
    delay(500);
}
void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);

  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);

  // Initialize I2C display using 3.3 volts
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  // Initialize DHT11
  dht.begin();

  // Set Relay as Output
  pinMode(RELAY_OUT, OUTPUT);

  // Turn off relay
  digitalWrite(RELAY_OUT, LOW);

  //Set pump status as off
  Blynk.virtualWrite(V3, LOW);

}

void loop()
{
  Blynk.run();
  timer.run();

  // See if pump should be triggered
  // See if moisture is below or equal to threshold
  if (soilMoisturePercent <= pump_trigger) {
    // Turn pump on
    pumpOn();

  } else {
    // Turn pump off
    pumpOff();
  }

  // Cycle values on OLED Display
  // Pump Status
  printOLED(35, "PUMP", 40, pump_status_text, 2000);
  // Temperature
  printOLED(35, "TEMP", 10, String(temp) + "C", 2000);
  // Humidity
  printOLED(30, "HUMID", 10, String(hum) + "%", 2000);
  // Moisture
  printOLED(35, "Soil.M", 30, String(soilMoisturePercent) + "%", 2000);

}

 void pumpOn()
 {
   // Turn pump on
   digitalWrite(RELAY_OUT, LOW);
   pump_status_text = "ON";
   Blynk.virtualWrite(V3, HIGH);

 }


 void pumpOff()
 {
   // Turn pump off
   digitalWrite(RELAY_OUT, HIGH);
   pump_status_text = "OFF";
   Blynk.virtualWrite(V3, LOW);

 }

 void printOLED(int top_cursor, String top_text, int main_cursor, String main_text, int delay_time){
   // Prints to OLED and holds display for delay_time
   display.setCursor(top_cursor, 0);
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.println(top_text);

   display.setCursor(main_cursor, 40);
   display.setTextSize(3);
   display.setTextColor(WHITE);
   display.print(main_text);
   display.display();

   delay(delay_time);
   display.clearDisplay();

 }
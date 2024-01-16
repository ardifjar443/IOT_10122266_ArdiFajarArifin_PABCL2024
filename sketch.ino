
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include "ThingSpeak.h"
#include <NewPing.h>


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);


#define RELAY_PIN 33     // Pin Relay
#define RELAY_PIN2 27    // Pin Relay
#define DHT_PIN 14       // Pin sensor DHT22
#define LDR_PIN 34       // Pin analog LDR
#define LED_PIN 15       // Pin LEd

#define TRIG_PIN  0      // Pin HC-SR04
#define ECHO_PIN  2

const int myChannelNumber = 2401597;
const char* myApiKey = "FVVZYMIPMANLOY05";
const char* server = "api.thingspeak.com";

DHT dht(DHT_PIN, DHT22);
Servo servo;
WiFiClient client;

int maxReconnectAttempts = 5;
int reconnectAttempts = 0;

NewPing sonar(TRIG_PIN, ECHO_PIN);

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    if (reconnectAttempts < maxReconnectAttempts) {
      Serial.println("Attempting to reconnect to WiFi...");
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("reconnect to WiFi...");
      lcd.print("--------------------");

      WiFi.begin("Wokwi-GUEST", "",6);
      delay(5000);
      reconnectAttempts++;
    } else {
      Serial.println("Max reconnection attempts reached. Waiting for manual intervention.");
    }
  } else {
    reconnectAttempts = 0; 
  }
}


void setup() {
  Serial.begin(115200);
  

  lcd.init();

  lcd.backlight();

 


  lcd.setCursor(0,0);
  lcd.print("Connecting to WiFi");


  lcd.setCursor(0,1);
  WiFi.begin("Wokwi-GUEST", "",6);
  WiFi.setAutoReconnect(true);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    lcd.print(".");
  }
  lcd.setCursor(0,2);
  lcd.print("Connected!");
  ThingSpeak.begin(client);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("  GreenHouse  ctrl  ");
  lcd.setCursor(0,2);
  lcd.print("--------------------");
  delay(2000);

  dht.begin();
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
  digitalWrite(RELAY_PIN2, LOW); 
 

}

void loop() {
  reconnectWiFi();
  float temperature = dht.readTemperature();
  int ldrValue = analogRead(LDR_PIN);

  int distance = sonar.ping_cm();
  


  ThingSpeak.setField(1,temperature);

  ThingSpeak.setField(2,ldrValue);
  ThingSpeak.setField(3,distance);

  lcd.clear();


  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0,1);
  lcd.print("Light: ");
  lcd.print(ldrValue);
 
  if (temperature >= 32) {
    digitalWrite(RELAY_PIN, HIGH);
    lcd.setCursor(0, 2);
    lcd.print("Fan Status: ON");
  } 
  else {
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(0, 2);
    lcd.print("Fan Status: OFF");
  }

   if (ldrValue < 500) {
    digitalWrite(LED_PIN, HIGH);  
    lcd.setCursor(0, 3);
    lcd.print("Lamp Status: ON");
  } else {
    digitalWrite(LED_PIN, LOW);   
    lcd.setCursor(0, 3);
    lcd.print("Lamp Status: OFF");
  }

 

  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Water: ");
  lcd.print(distance);
  lcd.print(" CM");
  if (distance >= 300){
    digitalWrite(RELAY_PIN2, HIGH);
    lcd.setCursor(0,1);
    lcd.print("pump status: ON");
  }else if (distance <= 100) {
    digitalWrite(RELAY_PIN2, LOW);
    lcd.setCursor(0,1);
    lcd.print("pump status: OFF");
  }


  int x = ThingSpeak.writeFields(myChannelNumber,myApiKey);

   
  
  
 
  
  if(x == 200){
    Serial.println("Data pushed successfull");
    lcd.setCursor(0,2);
    lcd.print("____________________");
    lcd.setCursor(0,3);
    
    lcd.print("push: success");
  }else{
    Serial.println("Push error" + String(x));
    lcd.setCursor(0,2);
    lcd.print("____________________");
    lcd.setCursor(0,3);
    lcd.print("push: error "+String(x));
  }

  

  Serial.println("---");

  

  delay(5000); 
  
}

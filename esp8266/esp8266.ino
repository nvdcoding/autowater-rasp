#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleTimer.h>
#include <ArduinoJson.h>
SimpleTimer timer;
/* DHT22*/
#include "DHT.h"
// Wifi
#define ssid "AE 251"
#define password "hochocnuahocmai"
// mqtt
WiFiClient espClient;
PubSubClient client(espClient);

#define SOILPIN A0
#define ONE_WIRE_BUS 2
// config MQTT
#define mqtt_user "lkuno222"
#define mqtt_pass "0DC42C6DB41C43B6"
#define mqtt_broker "ngoinhaiot.com"
const int mqtt_port = 1111;


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int soilMoister = 0;
void getSoilMoisterData(void) {
  soilMoister = 0;
  delay (500);
  int N = 3;
  for (int i = 0; i < N; i++) // read sensor "N" times and get the average
  {
    soilMoister += analogRead(SOILPIN);
    delay(150);
  }
  soilMoister = soilMoister / N;

  soilMoister = 100 - map(soilMoister, 0, 1023, 0, 100);
  if(soilMoister < 30) {
    Serial.println("PumpON");
    timer.setTimeout(5000, stopPump);
  }
}
void stopPump() {
  Serial.println("PumpOff");
}
void startTimers(void) {
  timer.setInterval(10000, getSoilMoisterData); // ok
  timer.setInterval(1000, autoControl); // ok
}
void autoControl() {

}
//void pumpControl() {
//  turnOnPump();
//}
//void turnOnPump(){
//  pumpStatus = 1;
//  control();
//  timer.setTimeout(15 * 1000, turnOffPump);
//}
//void turnOffPump(){
//  pumpStatus = 0;
//  control();
//}
//void lampControl() {
//  turnOnLamp();
//}
//void turnOnLamp(){
//  lampStatus = 1;
//  control();
//  timer.setTimeout(5*60000, turnOffLamp);
//}
//void turnOffLamp(){
//  lampStatus = 0;
//  control();
//}

void setup() {
  Serial.begin(9600);

  pinMode(SOILPIN, OUTPUT);
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  setup_wifi();
  delay(10);
  sensors.begin();
  startTimers();
}

void setup_wifi() {
  delay(10);
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
}
unsigned long t;
void setup_mqtt() {
  if (!client.connected())// Kiểm tra kết nối
    reconnect();
  client.loop();
}
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    if (client.connect("ESP8266_id1", mqtt_user, mqtt_pass)) //kết nối vào broker
    {
      client.subscribe("lkuno222/plant");
    }
    else
    {
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  char buffer[256];
  String message;
  StaticJsonDocument<200> doc;
  for (int i = 0; i < length; i++) {
   message += (char)payload[i];
  }
  if(message == "UPDATE") {
    doc["soilMoister"] = soilMoister;
    serializeJson(doc, buffer);
    client.publish("lkuno222/update", buffer); 
  } else {
      if(message == "PumpOnAuto") {
        Serial.println("PumpON");
        timer.setTimeout(5000, stopPump);
      } else {
        Serial.println(message);
      } 
      
  }
}

void loop() {
  setup_mqtt();
  timer.run();
}

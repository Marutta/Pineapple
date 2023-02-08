#include <Wire.h>
#include <WiFi.h>
#include <MQTT.h>
#include <SPI.h>
#include "Adafruit_SHT31.h"

const char ssid[] = "Student";
const char pass[] = "Kristiania1914";
const char mqtt_username[] = "anka040";
const char mqtt_password[] = "klhj3Dkhgdfs89D";
const char mqtt_server[]   = "mqtt.toytronics.com";
String yourPersonalTopic;

WiFiClient networkClient;
MQTTClient mqttClient;

bool enableHeater = false;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  String clientId = "66454465"; // Create a random client ID
    clientId += String(random(0xffff), HEX);

  Serial.print("\nConnecting to Wifi...");
  while (!mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.print(".");
    delay(1000);
  }

  yourPersonalTopic = "pineapple-house/"; // Create a topic path based on your username
  yourPersonalTopic += "ph1";
  Serial.print("\nConnected to Wifi! Setting up Subscription to the topic: ");
  Serial.println( yourPersonalTopic );

  mqttClient.subscribe( yourPersonalTopic.c_str() );
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  /*
  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
  */

  WiFi.begin(ssid, pass);

  mqttClient.begin(mqtt_server, networkClient);

  connect();
}

void loop() {
  mqttClient.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else { 
    Serial.println("Failed to read humidity");
  }

  if (!mqttClient.connected()) {
    connect();
  }

  // publish a message every 5 second.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    float t = sht31.readTemperature();
    String tempAsString = String(t);
    mqttClient.publish(yourPersonalTopic, tempAsString);
  }
}
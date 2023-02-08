#include <Wire.h>
#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "ssid";
const char pass[] = "pass";
const char mqtt_username[] = "YourUserName";
const char mqtt_password[] = "YourPassword";
const char mqtt_server[]   = "mqtt_server_address";

WiFiClient networkClient;
MQTTClient mqttClient;

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
}

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.begin(ssid, pass);

  mqttClient.begin(mqtt_server, networkClient);

  connect();
}

void loop() {
  mqttClient.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient.connected()) {
    connect();
  }

  // publish a message every 5 second.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    


    mqttClient.publish("ananas-topic/ah1", temp);
    
  }
}
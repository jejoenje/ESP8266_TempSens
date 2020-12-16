
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "init.h"             // external constants

#define ONE_WIRE_BUS D1

#define mqtt_server "192.168.1.77"
#define mqtt_port 1883

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;

// Function to connect to WiFi:
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void getSendTemperatures() {
  sensors.requestTemperatures();
  
  float temp1 = sensors.getTempCByIndex(0);
  float temp2 = sensors.getTempCByIndex(1);

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temp1\":";payload += temp1; payload += ",";
  payload += "\"temp2\":";payload += temp2; payload += ",";
  
  payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  Serial.println( attributes );

  client.publish("sensors/esp8266/beta/temp1", String(temp1).c_str(), true);
  client.publish("sensors/esp8266/beta/temp2", String(temp2).c_str(), true);
  
  
}

void setup() {
  Serial.begin(9600);
  sensors.begin();
  setup_wifi();
  client.setClient(espClient);
  client.setServer(mqtt_server, mqtt_port);

  if ( !client.connected() ) 
  {
    reconnect();
  }

  getSendTemperatures();
  delay(1000);

  ESP.deepSleep(600e5);


}

void loop() {
//  if (!client.connected()) {
//      reconnect();
//    }
//    //client.loop();
//  
//  getSendTemperatures();
//  delay(1000);
}

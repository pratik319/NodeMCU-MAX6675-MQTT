#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <max6675.h>

// Replace with your Wi-Fi credentials
const char* ssid = "************";
const char* password = "*******";

// MQTT broker settings
const char* mqtt_server = "*********"; //cmd wlan adapter ip
const int mqtt_port = 1883;
const char* mqtt_username = "*******";
const char* mqtt_password = "*******";
const char* mqtt_topic = "temperature"; //type your own

const int MAX6675_CLK_PIN = D5;
const int MAX6675_CS_PIN = D8;
const int MAX6675_SO_PIN = D7;

MAX6675 thermocouple(MAX6675_CLK_PIN, MAX6675_CS_PIN, MAX6675_SO_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
 
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  double tempC = thermocouple.readCelsius();
  if (isnan(tempC)) {
    Serial.println("Error reading thermocouple.");
  } else {
    double tempF = (tempC * 9.0 / 5.0) + 32.0;

    Serial.println(tempC);
    char tempStr[10];
    dtostrf(tempC, 4, 2, tempStr);
    client.publish(mqtt_topic, tempStr);
  }

  delay(1000);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("NodeMCU_Client", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages if needed
}

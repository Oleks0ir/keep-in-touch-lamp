#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -------- CONFIG --------
const char* WIFI_SSID = "MagentaWLAN-9P8U";
const char* WIFI_PASS = "85244510944937551816";

const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;

const char* MQTT_CLIENT_ID = "esp8266_bracelet_B";
const char* PUB_TOPIC = "bracelet/A";

const int BUTTON_PIN = 14; //D5

bool currentState;
// ------------------------

WiFiClient espClient;
PubSubClient mqtt(espClient);

bool lastState = HIGH;

// -------- SETUP --------
void setup() {
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(115200);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  Serial.println("Connected to WIFI");
  // MQTT
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  while (!mqtt.connected()) {
    mqtt.connect(MQTT_CLIENT_ID);
    delay(100);
  }

  Serial.println("Connected to MQTT broker");

}

// -------- LOOP --------
void loop() {
  mqtt.loop();

  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {
    mqtt.publish(PUB_TOPIC, "TOUCH");
    Serial.println("TOUCH!");
  }
  lastState = currentState;

  if (currentState){Serial.print("-");}else  if(!currentState){Serial.print("+");}else{Serial.print("0");}
  delay(100);

}

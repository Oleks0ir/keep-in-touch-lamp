
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// -------- CONFIG --------
const char* WIFI_SSID = "MagentaWLAN-9P8U";
const char* WIFI_PASS = "85244510944937551816";

const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;

const char* MQTT_CLIENT_ID = "esp8266_bracelet_A";
const char* SUB_TOPIC = "bracelet/A";

// Example output pin
const int LED_PIN = LED_BUILTIN;
// ------------------------

WiFiClient espClient;
PubSubClient mqtt(espClient);

// -------- CALLBACK --------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  // Example action
  if (msg == "TOUCH") {
    digitalWrite(LED_PIN, LOW);   // LED ON (ESP8266 logic)
    delay(200);
    digitalWrite(LED_PIN, HIGH);  // LED OFF
  }
}

// -------- SETUP --------
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // MQTT
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  while (!mqtt.connected()) {
    mqtt.connect(MQTT_CLIENT_ID);
    delay(500);
  }

  mqtt.subscribe(SUB_TOPIC);

  Serial.println("Connected to MQTT broker");

}

// -------- LOOP --------
void loop() {
  if (!mqtt.connected()) {
    while (!mqtt.connected()) {
      mqtt.connect(MQTT_CLIENT_ID);
      delay(100);
    }
    mqtt.subscribe(SUB_TOPIC);
  }

  mqtt.loop();
}

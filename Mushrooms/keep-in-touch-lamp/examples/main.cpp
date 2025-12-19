#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ---------- CONFIG ----------
const char* WIFI_SSID = "MagentaWLAN-9P8U";
const char* WIFI_PASS = "85244510944937551816";

const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;

const char* MQTT_TOPIC = "bracelet/signal";

// 🔴🟢🔵 CHANGE ONLY THIS LINE
#define MY_COLOR 0x009900   // <-- change to 0x0000FF on the other ESP



// Hardware
#define BUTTON_PIN 14
#define LED_PIN    LED_BUILTIN
// ----------------------------

WiFiClient espClient;
PubSubClient mqtt(espClient);

bool lastButtonState = HIGH;
unsigned long lastPressTime = 0;
const unsigned long debounceMs = 250;

bool currentState;

// ---------- HELPERS ----------
String colorToString(uint32_t color) {
  char buf[7];
  sprintf(buf, "%06lX", color);
  return String(buf);
}

void reconnectMQTT() {
  while (!mqtt.connected()) {
    mqtt.connect(colorToString(MY_COLOR).c_str());
    delay(200);
  }
  mqtt.subscribe(MQTT_TOPIC);
}

// ---------- CALLBACK ----------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  uint32_t receivedColor = strtoul(msg.c_str(), nullptr, 16);

  // Ignore own messages
  if (receivedColor == MY_COLOR) return;

  // React to the other bracelet
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
}

// ---------- SETUP ----------
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  reconnectMQTT();

  Serial.println("Bracelet ready. Color: " + colorToString(MY_COLOR));
}

// ---------- LOOP ----------
void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();

  currentState = !digitalRead(BUTTON_PIN);
  //Serial.println(currentState);

  if (currentState == HIGH) {
    if (millis() - lastPressTime > debounceMs) {
      String payload = colorToString(MY_COLOR);
      mqtt.publish(MQTT_TOPIC, payload.c_str());
      Serial.println("Sent: " + payload);
      lastPressTime = millis();
    }
  }

  delay(10);

}

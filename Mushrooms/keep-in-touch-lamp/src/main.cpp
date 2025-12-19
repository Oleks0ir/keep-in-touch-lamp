#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

// ---------- CONFIG ----------
const char* WIFI_SSID = "MagentaWLAN-9P8U";
const char* WIFI_PASS = "85244510944937551816";

const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;

const char* MQTT_TOPIC = "bracelet/signal";

// CHANGE ONLY THIS LINE per ESP
#define MY_COLOR 0x00ff00  // ESP A (red), ESP B uses 0x0000FF (blue)

// Hardware
#define BUTTON_PIN D5
#define LED_PIN    D6
#define NUM_LEDS   5
#define COLOR_ORDER GRB
// ----------------------------

WiFiClient espClient;
PubSubClient mqtt(espClient);

CRGB leds[NUM_LEDS];

bool lastButtonState = HIGH;
unsigned long lastPressTime = 0;
const unsigned long debounceMs = 250;

CRGB currentColor = CRGB::Black;
uint8_t currentBrightness = 0;
unsigned long lastColorReceivedTime = 0;

const unsigned long fadeStepMs = 50;  // fade every 50 ms
const uint8_t fadeAmount = 5;          // amount to fade each step

unsigned long lastFadeTime = 0;


const unsigned long FADE_INTERVAL_MS = 25; // 2x faster fade
const float FADE_FACTOR = 0.8;              // exponential decay factor

float currentBrightnessF = 0.0;


// Helpers
String colorToString(uint32_t color) {
  char buf[7];
  sprintf(buf, "%06lX", color);
  return String(buf);
}

void setStripColor(CRGB color, uint8_t brightness) {
  CRGB c = color;
  c.nscale8_video(brightness);
  fill_solid(leds, NUM_LEDS, c);
  FastLED.show();
}

void clearStrip() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void reconnectMQTT() {
  while (!mqtt.connected()) {
    mqtt.connect(colorToString(MY_COLOR).c_str());
    delay(200);
  }
  mqtt.subscribe(MQTT_TOPIC);
}

// MQTT callback
void mqttCallback(char* topic, byte* payload, unsigned int length) {

 currentBrightnessF = 255.0;
 currentBrightness = 255;

  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  uint32_t receivedColor = strtoul(msg.c_str(), nullptr, 16);

  if (receivedColor == MY_COLOR) return; // ignore own messages

  currentColor = CRGB((receivedColor >> 16) & 0xFF, (receivedColor >> 8) & 0xFF, receivedColor & 0xFF);
  currentBrightness = 255;  // reset brightness to full on receiving new pulse
  lastColorReceivedTime = millis();

  setStripColor(currentColor, currentBrightness);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  clearStrip();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  reconnectMQTT();

  Serial.println("Bracelet ready. My color: " + colorToString(MY_COLOR));
}

void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();

  bool currentState = digitalRead(BUTTON_PIN);

  // Send pulse on button press
  if (lastButtonState == HIGH && currentState == LOW) {
    if (millis() - lastPressTime > debounceMs) {
      String payload = colorToString(MY_COLOR);
      mqtt.publish(MQTT_TOPIC, payload.c_str());
      Serial.println("Sent pulse color: " + payload);

      lastPressTime = millis();
    }
  }
  lastButtonState = currentState;

  // Fade LEDs smoothly
  if (currentBrightnessF > 1.0 && millis() - lastFadeTime > FADE_INTERVAL_MS) {
  lastFadeTime = millis();
  currentBrightnessF *= FADE_FACTOR;
  if (currentBrightnessF < 1.0) {
    currentBrightnessF = 0.0;
    clearStrip();
  } else {
    currentBrightness = (uint8_t)currentBrightnessF;
    setStripColor(currentColor, currentBrightness);
  }
}
}

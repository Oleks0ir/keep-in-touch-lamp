#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

// CONFIG
const char* WIFI_SSID = "MagentaWLAN-9P8U";
const char* WIFI_PASS = "85244510944937551816";

const char* MQTT_BROKER = "test.mosquitto.org";
const int MQTT_PORT = 1883;

const char* MQTT_TOPIC = "bracelet/signal";

unsigned long buttonPressStart = 0;
bool longPressHandled = false;
bool standByActive = false;


#define MY_COLOR 0xFF0000  // change per ESP

#define STANDY_COLOR 0xcc6600  // change per ESP

#define BUTTON_PIN 14
#define LED_PIN    12
#define NUM_LEDS   5
#define COLOR_ORDER GRB

WiFiClient espClient;
PubSubClient mqtt(espClient);

CRGB leds[NUM_LEDS];

bool lastButtonState = HIGH;
unsigned long lastPressTime = 0;
const unsigned long debounceMs = 250;

CRGB currentColor = CRGB::Black;
float currentBrightnessF = 0.0;
uint8_t currentBrightness = 0;

unsigned long lastColorReceivedTime = 0;

const unsigned long FADE_INTERVAL_MS = 55;
const float FADE_FACTOR = 0.8;

const float TARGET_BRIGHTNESS = 204.0; // e.g. 80%

unsigned long lastFadeTime = 0;

uint8_t current_mode = 0; //0 off   1 standby(process)   2 pulse 
uint8_t current_standby_brightness = 0;

const uint8_t max_standbt_brightness = 160;


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
    if (mqtt.connect(colorToString(MY_COLOR).c_str())) {
      mqtt.subscribe(MQTT_TOPIC);
    } else {
      delay(500);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  uint32_t receivedColor = strtoul(msg.c_str(), nullptr, 16);
  if (receivedColor == MY_COLOR) return;

  currentColor = CRGB((receivedColor >> 16) & 0xFF, (receivedColor >> 8) & 0xFF, receivedColor & 0xFF);
  currentBrightnessF = 255.0;
  currentBrightness = 255;
  lastColorReceivedTime = millis();

  Serial.println("Received color: " + colorToString(receivedColor));
  current_mode = 2;
  setStripColor(currentColor, currentBrightness);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  clearStrip();

  setStripColor(MY_COLOR, max_standbt_brightness);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("\n\nConnecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
  
  setStripColor(0x009933, max_standbt_brightness);
  delay(300);
  clearStrip();
  delay(300);
  setStripColor(0x009933, max_standbt_brightness);
  delay(300);
  clearStrip();
  delay(300);
  setStripColor(0x009933, max_standbt_brightness);
  delay(300);
  clearStrip();
  delay(300);

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  reconnectMQTT();

  clearStrip();

  Serial.println("Setup complete. My color: " + colorToString(MY_COLOR));

  clearStrip();
}

void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();

  bool currentState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentState == LOW) {
    if (millis() - lastPressTime > debounceMs) {
      String payload = colorToString(MY_COLOR);
      mqtt.publish(MQTT_TOPIC, payload.c_str());
      Serial.println("Sent pulse color: " + payload);
      lastPressTime = millis();
    }

  }
  lastButtonState = currentState;

  // Fade logic
  if (currentBrightnessF > 1.0 && millis() - lastFadeTime > FADE_INTERVAL_MS && current_mode == 2) {
    lastFadeTime = millis();
    currentBrightnessF *= FADE_FACTOR;
    if (currentBrightnessF < 1.0) {
      currentBrightnessF = 0.0;
      clearStrip();
      current_mode = 3;
    } else {
      
      currentBrightness = (uint8_t)currentBrightnessF;
      setStripColor(currentColor, currentBrightness);
    }
  }

  //FadeIn on standby
    if (millis() - lastFadeTime > FADE_INTERVAL_MS && current_mode == 1) {
      lastFadeTime = millis();

      currentBrightnessF += (TARGET_BRIGHTNESS - currentBrightnessF) * (1.0 - FADE_FACTOR);

      if (currentBrightnessF >= TARGET_BRIGHTNESS - 1.0) {
        currentBrightnessF = TARGET_BRIGHTNESS;
      }

      setStripColor(STANDY_COLOR, (uint8_t)currentBrightnessF);
    }

    Serial.println(current_mode);
    
    if(current_mode == 0){
        clearStrip();
    }

if (current_mode == 3){
    current_mode = (int) standByActive;
}

if (currentState == HIGH) {  // button held down (pressed)
  if (buttonPressStart == 0) {
    buttonPressStart = millis();
    longPressHandled = false;
  }

  if (!longPressHandled && millis() - buttonPressStart >= 3000) {
    standByActive = !standByActive;
    current_mode = 2-(int)standByActive;
    /*if(standByActive){
        current_mode = 1;
    }
    else{
        current_mode = 2;
    }*/
    longPressHandled = true;
    Serial.println("Hold initiated");
  }
} else {  // button released
  buttonPressStart = 0;
  longPressHandled = false;
}


}

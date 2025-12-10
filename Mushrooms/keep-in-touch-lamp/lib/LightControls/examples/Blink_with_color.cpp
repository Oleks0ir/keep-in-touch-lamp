#include <Arduino.h>
#include <LightControls.h>

LightControls lights(3, 2);  // 10 LEDs, pin D4 (GPIO2)


void setup() {
    lights.cycling = true;
    lights.begin();
}

void loop() {
  
    lights.setAllHexColorInArray(0, 4, 0xff0066); // sets an leds from 0 to 4 to purple
    delay(200);
    lights.setAllHexColor(0x000000);            //switches all defined leds on the stripe off
    delay(200);
}

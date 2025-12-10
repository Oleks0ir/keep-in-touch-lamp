#include <Arduino.h>
#include <LightControls.h>

LightControls lights(10, 2);  // 10 LEDs, pin D4 (2)


void setup() {
    lights.cycling = true;
    lights.begin();
}


void loop() {
  uint8 i;

  for (i = 1; i < 250; i++)     //clockwise
  {
    lights.setHexColor(i, 0xcccccc);
    lights.setHexColor(i-1, 0x000000);
    delay(100);
  }

  for (i = 255; i >0; i--)    //counterclockwise
  {
    lights.setHexColor(i+1, 0x000000);
    lights.setHexColor(i, 0xcccccc);
    delay(100);
  }
    
}
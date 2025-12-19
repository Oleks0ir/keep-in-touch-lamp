#include "LightControls.h"

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  120


LightControls::LightControls(uint16_t numLEDs, uint8_t dataPin) {
    _numLEDs = numLEDs;
    _dataPin = dataPin;
    leds = new CRGB[_numLEDs];
}

void LightControls::begin() {
    FastLED.addLeds<WS2812B, 2, GRB>(leds, _numLEDs);  // pin 2 -> D1 for example
    FastLED.setBrightness(BRIGHTNESS);
}



void LightControls::setHexColor(uint16_t index, uint32_t hexColor) {
    
    if(cycling){
        index = index%_numLEDs;
    }
    else if(index>=_numLEDs){
        return;
    }
        

    // Extract R, G, B components from 0xRRGGBB
    uint8_t r = (hexColor >> 16) & 0xFF;
    uint8_t g = (hexColor >> 8)  & 0xFF;
    uint8_t b = (hexColor)       & 0xFF;

    leds[index] = CRGB(r, g, b);
    FastLED.show();
}

void LightControls::setAllHexColorInArray(uint16_t start_index, uint16_t stop_index, uint32_t hexColor){
    
    if (stop_index>_numLEDs) stop_index=_numLEDs;
    
    for (start_index; start_index <= stop_index; start_index++)
    {
        setHexColor(start_index, hexColor);
    }
    
}

void LightControls::setAllHexColor(uint32_t hexColor){
    
    for (int i = 0; i < _numLEDs; i++)
    {
        setHexColor(i, hexColor);
    }
    
}
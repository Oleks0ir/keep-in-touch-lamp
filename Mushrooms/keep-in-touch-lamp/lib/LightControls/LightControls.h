#ifndef LIGHTCONTROLS_H
#define LIGHTCONTROLS_H

#include <Arduino.h>
#include <FastLED.h>

class LightControls {
public:
    LightControls(uint16_t numLEDs, uint8_t dataPin);

    void begin();
    void setHexColor(uint16_t index, uint32_t hexColor);
    void setAllHexColorInArray(uint16_t start_index, uint16_t stop_index, uint32_t hexColor);
    void setAllHexColor(uint32_t hexColor);
    bool cycling = false;


private:
    uint16_t _numLEDs;
    uint8_t _dataPin;
    CRGB* leds;

    uint16_t t = 0;  // time variable for fade
};


class Animation {
    public:
        Animation(LightControls lights);
    
    private:
        bool animationFlags[4]; // 0 - brightness reduction (fade); 1 -  Blurr; 2 - Rainbow; 3 - running lights;
};

#endif

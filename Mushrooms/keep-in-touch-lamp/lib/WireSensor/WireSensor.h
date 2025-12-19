#ifndef WIRESENSOR_H
#define WIRESENSOR_H

#include <Arduino.h>

class SensorButton {
public:
    SensorButton(uint8_t ButtonPin);

    bool touching;
    uint16_t t_disch;
    uint16_t t_norm;

    void init();
    void update();

private:
    uint8_t _ButtonPin;

    uint16_t _t_charge = 30;   // µs charge time
    uint16_t t_delta  = 5;    // µs threshold (tune this)

    uint8_t measurement_count;

    void charge();
};

#endif

#include "WireSensor.h"

SensorButton::SensorButton(uint8_t ButtonPin) {
    _ButtonPin = ButtonPin;
    touching  = false;
    t_disch   = 0;
    t_norm    = 0;
}

void SensorButton::charge() {
    pinMode(_ButtonPin, OUTPUT);
    digitalWrite(_ButtonPin, HIGH);
    delayMicroseconds(_t_charge);
    pinMode(_ButtonPin, INPUT);   // floating, discharged via 1 MΩ
}

void SensorButton::init() {
    t_norm = 0;
    touching = false;

    // Baseline calibration
    for (measurement_count = 0; measurement_count < 20; measurement_count++) {
        charge();

        uint32_t start = micros();
        while (digitalRead(_ButtonPin) != LOW &&
              (micros() - start) < 5000) {
            // wait for discharge or timeout
        }

        t_norm += (micros() - start);
        delay(5); // allow stabilization
    }

    t_norm /= 20;
}

void SensorButton::update() {
    charge();

    uint32_t start = micros();
    while (digitalRead(_ButtonPin) != LOW &&
          (micros() - start) < 5000) {
        // wait for discharge or timeout
    }

    t_disch = micros() - start;

    touching = (t_disch > (t_norm + t_delta));
}

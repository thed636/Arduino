#ifndef TTL_OUT_PIN_H_031220122303
#define TTL_OUT_PIN_H_031220122303

#include <Arduino.h>

class TtlOutPin {
public:
    TtlOutPin(byte pinNumber) : pin(pinNumber) {
        pinMode(pin, OUTPUT);
    }
    void state(bool high) {
        digitalWrite(number(), high ? HIGH : LOW);
    }
    inline void high() {
        state(true);
    }
    inline void low() {
        state(false);
    }
    inline byte number() const {
        return pin;
    }
private:
    const byte pin;
};

#endif //TTL_OUT_PIN_H_031220122303

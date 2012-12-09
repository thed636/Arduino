#ifndef TTL_IN_PIN_H_031220122303
#define TTL_IN_PIN_H_031220122303

#include <Arduino.h>

class TtlInPin {
public:
    TtlInPin(byte pinNumber) : pin(pinNumber) {
        pinMode(pin, INPUT);
    }
    inline bool high() const {
        return digitalRead(number()) == HIGH;
    }
    inline bool low() const {
        return !high();
    }
    inline byte number() const {
        return pin;
    }
private:
    const byte pin;
};

class Trigger: public TtlInPin {
public:
    Trigger(byte pinNumber) : TtlInPin(pinNumber) {
        reset();
    }

    bool changed() const {
        return state != high();
    }

    void reset() {
        state = high();
    }

private:
    bool state;
};

class EdgeCounter {
public:
    EdgeCounter(Trigger & trigger) : trigger(trigger), count_(0) {
    }

    void reset() {
        count_ = 0;
    }

    uint32_t count() const {
        return count_;
    }

    uint32_t update() {
        if (trigger.changed()) {
            trigger.reset();
            ++count_;
        }
        return count();
    }
private:
    Trigger & trigger;
    uint32_t count_;
};
#endif //TTL_IN_PIN_H_031220122303

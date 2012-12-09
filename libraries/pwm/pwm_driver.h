#ifndef PWM_DRIVER_H_031220122303
#define PWM_DRIVER_H_031220122303

#include <Arduino.h>
#include "pwm_pin.h"
#include "ttlout_pin.h"

class PwmDriver {
public:
    PwmDriver(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber)
    : cw(cwPinNumber), ccw(ccwPinNumber), pwm(pwmPinNumber) {
        stop();
    }

    static const int maxOutValue;
    static const int minOutValue;

    inline void stop() {
        out(0);
    }

    inline int out() const {
        return out_;
    }

    void out(int v);

protected:
    void updateOutput();

private:
    TtlOutPin cw;
    TtlOutPin ccw;
    PwmPin pwm;
    int out_;
};
#endif

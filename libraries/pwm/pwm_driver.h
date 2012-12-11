#ifndef PWM_DRIVER_H_031220122303
#define PWM_DRIVER_H_031220122303

#include <Arduino.h>
#include "pwm_pin.h"
#include "ttlout_pin.h"

class PwmDriver {
public:
    PwmDriver(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte deadZone = 50)
    : cw(cwPinNumber), ccw(ccwPinNumber), pwm(pwmPinNumber), deadZone_(deadZone) {
        stop();
    }

    const int maxOutValue() const {
        return maxPwmValue - deadZone();
    }
    const int minOutValue() const {
        return -maxOutValue();
    }
    static const int maxPwmValue;

    inline void stop() {
        out(0);
    }

    inline int out() const {
        return out_;
    }

    void out(int v);

    int deadZone() const {
        return deadZone_;
    }

    void deadZone( int v ) {
        deadZone_ = v;
    }

protected:
    void updateOutput();

private:
    TtlOutPin cw;
    TtlOutPin ccw;
    PwmPin pwm;
    int out_;
    int deadZone_;
};
#endif

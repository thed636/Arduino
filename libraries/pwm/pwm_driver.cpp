#include <pwm_driver.h>

const int PwmDriver::maxPwmValue = 255;

void PwmDriver::out(int v) {
    const int newValue = constrain(v, minOutValue(), maxOutValue());
    if (newValue != out()) {
        out_ = newValue;
        updateOutput();
    }
}

void PwmDriver::updateOutput() {
    if (out()) {
        const int pwmValue = abs(out()) + deadZone();
        pwm.value(pwmValue);
        const bool ccwDirection = out() > 0;
        ccw.state(ccwDirection);
        cw.state(!ccwDirection);
    } else {
        pwm.value(255);
        cw.low();
        ccw.low();
    }
}

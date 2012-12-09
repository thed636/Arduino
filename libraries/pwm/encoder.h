#ifndef ENCODER_H_165506122012
#define ENCODER_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>

class SoftEncoder {
public:
    SoftEncoder(byte pulsePinNumber)
    : trigger(pulsePinNumber), x(0), t_1(millis()), dt(dtMax), direction(0) {
    }

    enum { timeNumerator = 1000, dtMax };

    int position() const {
        return x;
    }

    int update();

    int velocity() const {
        return timeNumerator / dt * direction;
    }

    void reset() {
        x = 0;
        trigger.reset();
    }

    void setDirection(int v) {
        direction = v;
    }

private:
    Trigger trigger;
    int x;
    unsigned long t_1;
    unsigned long dt;
    int direction;
};

#endif

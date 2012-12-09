#include <encoder.h>

int SoftEncoder::update() {
    const unsigned long t = millis();
    const unsigned long timeout = t - t_1;

    if (trigger.changed()) {
        trigger.reset();
        x += direction;
        dt = timeout;
        t_1 = t;
    } else if (dt < dtMax && timeout > dt) {
        dt = timeout;
    }

    return x;
}

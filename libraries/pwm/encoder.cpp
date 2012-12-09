#include <encoder.h>

bool SoftEncoder::checkTriggerRaised() {
    if (trigger.changed()) {
        trigger.reset();
        return trigger.high();
    }
    return false;
}

int SoftEncoder::update() {
    const unsigned long t = millis();
    const unsigned long timeout = t - t_1;

    if (checkTriggerRaised()) {
        x += direction;
        dt = timeout;
        t_1 = t;
    } else if (dt < dtMax && timeout > dt) {
        dt = timeout;
    }

    return x;
}

#include <encoder.h>

bool SoftEncoder::checkTriggerRaised() {
    if (trigger.changed()) {
        trigger.reset();
        return trigger.high();
    }
    return false;
}

bool SoftEncoder::update() {
    if(tick_<timeNumerator) {
        ++tick_;
    }
    const int v(calculateVelocity(++tick_));
    if (checkTriggerRaised()) {
        increment();
        dx = v;
        tick_ = 0;
        return true;
    }
    if ( v < velocity() ) {
        dx = v;
        return velocity() == 0;
    }

    return false;
}

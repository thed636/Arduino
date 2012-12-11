#include <encoder.h>
#include <sign.h>
bool SoftEncoder::checkTriggerRaised() {
    if (trigger.changed()) {
        trigger.reset();
        return true;//trigger.high();
    }
    return false;
}

bool SoftEncoder::update() {
    if( dx ) {
        tick_++;
    }
    if (checkTriggerRaised()) {
        increment();
        dx_1 = dx;
        dx = calculateVelocity(tick_);
        dt_1 = tick_;
        tick_ = 0;
        return true;
    }
    if ( tick_ > dt_1 && dx ) {
        dx = max(0, abs(dx - max(1,abs( dx - dx_1)/tick_)))*sign(dx);
        return true;
    }

    return false;
}

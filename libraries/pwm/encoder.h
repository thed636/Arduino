#ifndef ENCODER_H_165506122012
#define ENCODER_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>

class SoftEncoder {
public:
    SoftEncoder(byte pulsePinNumber)
    : trigger(pulsePinNumber), x(0), dx(0), dx_1(0), dt_1(0), tick_(maxTick), direction(0) {
    }

    enum { timeNumerator = 1000, maxTick };

    typedef unsigned int tick_type;

    int position() const {
        return x;
    }

    int velocity() const {
        return dx;
    }

    bool update();

    void reset() {
        x = dx = 0;
        trigger.reset();
    }

    void setDirection(int v) {
        direction = v;
    }

    tick_type tick() const {
        return tick_;
    }

    int calculateVelocity( tick_type phase ) {
        return timeNumerator / phase * direction;
    }

protected:
    bool checkTriggerRaised();

    void increment() {
        x += direction;
    }
private:
    Trigger trigger;
    int x;
    int dx;
    int dx_1;
    tick_type dt_1;
    tick_type tick_;
    int direction;
};

//class PseudoEncoder {
//public:
//    PseudoEncoder()
//    : x(0), dx(0), tick_(dtMax), direction(0) {
//    }
//
//    enum { timeNumerator = 1000, dtMax };
//
//    typedef unsigned int tick_type;
//
//    int position() const {
//        return x;
//    }
//
//    bool update() {
//        ++tick;
//    }
//
//    void reset() {
//        x = dx = 0;
//    }
//
//    void setDirection(int v) {
//        direction = v;
//    }
//
//    tick_type tick() const {
//        return tick_;
//    }
//
//    static int calculatePhase( int velocity ) {
//        return velocity ? timeNumerator / abs(velocity) : dtMax;
//    }
//protected:
//    bool checkTriggerRaised();
//
//    void increment() {
//        x += direction;
//    }
//private:
//    int x;
//    int dx;
//    tick_type tick_;
//    int direction;
//};

#endif

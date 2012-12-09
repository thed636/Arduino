#ifndef DRIVES_H_165506122012
#define DRIVES_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>
#include <encoder.h>
#include <descrete.h>
#include <regulator.h>

#define sign(v) (abs(v)/v)

typedef PidRegulator SpeedRegulator;

typedef PRegulator PositionRegulator;

class ServoDrive {
public:
    ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber,
            const DescreteSample & descrete) :
            encoder_(pulsePinNumber), driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
            maxX(0), minX(0),
            speedRegulator_(10.0, 2.5, 1.0, 255., -255., descrete.T()),
            positionRegulator_(2.0),
            speedError_(0){
        reset();
    }

    SoftEncoder & encoder() {
        return encoder_;
    }

    const SoftEncoder & encoder() const {
        return encoder_;
    }

    int position() const {
        return encoder().position();
    }

    int target() const {
        return u;
    }

    void target(int val) {
        u = val;
    }

    int error() const {
        return target() - position();
    }

    int update() {
        encoder().update();
        control();
        return error();
    }

    int speed() const {
        return encoder().velocity();
    }

    int speedError() const {
        return speedError_;
    }

    void setMaxSpeed(byte v) {
        //positionRegulator.maximum(v);
    }

    void calibrate() {
        TtlInPin pin(30);
        int x0 = position();
        setSpeed(150);
        while (pin.high()) {
            x0 = encoder().update();
        }
        setSpeed(0);
        while (speed())
            encoder().update();
//    target(x0);
//    while(speed()||error())
//      update();
        reset();
    }

    void reset() {
        speedError_ = u = 0;
        encoder().reset();
    }

    SpeedRegulator & speedRegulator() {
        return speedRegulator_;
    }
    PositionRegulator & positionRegulator() {
        return positionRegulator_;
    }
    int out() const {
        return driver.out();
    }
protected:
    void setSpeed(int v) {
        speedError_ = v - speed();
        if (!v && abs(speedError()) < 15 ) {
            driver.stop();
            speedRegulator().reset();
        } else {
            const int out = speedRegulator().control(speedError());
            driver.out(out);
        }
        if (v) {
            encoder().setDirection(sign(v));
        }
    }

    void control() {
        setSpeed(positionRegulator().control(error()));
    }

private:
    SoftEncoder encoder_;
    PwmDriver driver;
    int u;
    int maxX;
    int minX;
    SpeedRegulator speedRegulator_;
    PositionRegulator positionRegulator_;
    int speedError_;
};

#endif // DRIVES_H_165506122012

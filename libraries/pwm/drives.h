#ifndef DRIVES_H_165506122012
#define DRIVES_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>
#include <encoder.h>
#include <descrete.h>
#include <regulator.h>

typedef PidRegulator VelocityRegulator;

typedef PRegulator PositionRegulator;

class ServoDrive {
public:
    enum ControlKind { controlPosition, controlVelocity };
    ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber,
            const DescreteSample & descrete) :
            encoder_(pulsePinNumber), driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
            maxPos(0), minPos(0),
            velocityRegulator_(10.0, 2.5, 1.0, 255., -255., descrete.T()),
            positionRegulator_(2.0, 80., -80.),
            targetSpeed_(0){
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

    int targetPosition() const {
        return teargetPos_;
    }

    void targetPosition(int val) {
        teargetPos_ = val;
        controlKind = controlPosition;
    }

    int positionError() const {
        return targetPosition() - position();
    }

    int speed() const {
        return encoder().velocity();
    }

    int targetSpeed() const {
        return targetSpeed_;
    }

    void targetSpeed( int v ) {
        targetSpeed_ = v;
        controlKind = controlVelocity;
    }

    int speedError() const {
        return targetSpeed_ - speed();
    }

    void setMaxSpeed(byte v) {
        //positionRegulator.maximum(v);
    }

    void calibrate() {
        TtlInPin pin(30);
        int x0 = position();
        targetSpeed(150);
        while (pin.high()) {
            control();
        }
        targetSpeed(0);
        while (speed())
            encoder().update();
//    targetPosition(x0);
//    while(speed()||error())
//      update();
        reset();
    }

    void reset() {
        targetSpeed_ = teargetPos_ = 0;
        encoder().reset();
    }

    VelocityRegulator & velocityRegulator() {
        return velocityRegulator_;
    }
    PositionRegulator & positionRegulator() {
        return positionRegulator_;
    }
    int out() const {
        return driver.out();
    }

    void control() {
        encoder().update();
        if( controlKind == controlPosition ) {
            handlePositionError();
        }
        handleSpeedError();
        updateEncoderDirection();
    }
protected:
    void updateEncoderDirection() {
        if (out()) {
            encoder().setDirection(sign(out()));
        }
    }

    void handleSpeedError() {
        if (targetSpeed()==0 && abs(speedError()) < 15 ) {
            driver.stop();
            velocityRegulator().reset();
        } else {
            const int ctl = velocityRegulator().control(speedError());
            const bool sameDirection = sign(ctl) == sign(speed());
            driver.out( !speed() || sameDirection ? ctl : 0 );
        }
    }

    void handlePositionError() {
        targetSpeed_ = positionRegulator().control(positionError());
    }
private:
    SoftEncoder encoder_;
    PwmDriver driver;
    int teargetPos_;
    int maxPos;
    int minPos;
    VelocityRegulator velocityRegulator_;
    PositionRegulator positionRegulator_;
    int targetSpeed_;
    ControlKind controlKind;
};

#endif // DRIVES_H_165506122012

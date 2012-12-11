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
    enum ControlKind { controlPosition, controlVelocity, controlSeek };
    ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber,
            const DescreteSample & descrete) :
            encoder_(pulsePinNumber), driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
            maxPos(0), minPos(0),
            velocityRegulator_(10.0, 2.5, 1.0, 255., -255., descrete.T()),
            positionRegulator_(2.0, 140., -140.),
            targetSpeed_(0),
            basePin(30) {
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
        return spd;// + (spd_1 + spd_2)/10;
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

    void seek(int speed) {
        targetSpeed_ = speed;
        controlKind = controlSeek;
    }

    void reset() {
        targetSpeed_ = teargetPos_ = 0;
        spd = spd_1 = spd_2 = 0;
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
    void deadZone( int v ) {
        driver.deadZone(v);
    }

    void control() {
        const int newSpd = encoder().update() ? encoder().velocity() : speed();
        spd_2 = spd_1;
        spd_1 = spd;
        spd = newSpd;
        if( controlKind == controlSeek ) {
            handleSeekError();
        }
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
        const bool positionReached = controlKind == controlPosition && !positionError();

        if( positionReached ) {
            velocityRegulator().reset();
            driver.stop();
        } else {
            int ctl = velocityRegulator().control(speedError());
            const bool sameDirection = sign(ctl) == sign(speed());
            if(speed() && !sameDirection) {
                ctl = min( abs(speed())/2, abs(ctl) ) * sign(ctl);
            }
            driver.out( ctl );
        }
    }

    void handlePositionError() {
        targetSpeed_ = positionRegulator().control(positionError());
    }

    void handleSeekError() {
        if(basePin.low()) {
            targetPosition(position());
        }
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
    TtlInPin basePin;
    int spd;
    int spd_1;
    int spd_2;
};

#endif // DRIVES_H_165506122012

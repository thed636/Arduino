#ifndef DRIVES_H_165506122012
#define DRIVES_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>
#include <encoder.h>
#include <descrete.h>
#include <regulator.h>

typedef PrecisePidRegulator VelocityRegulator;

typedef PRegulator PositionRegulator;

class ServoDriveSensors {
public:
    ServoDriveSensors(byte pulsePinNumber, const DescreteSample & descrete) :
            encoder_(pulsePinNumber), descrete_(descrete) {
        reset();
    }

    int position() const {
        return encoder().position();
    }

    int speed() const {
        return encoder().velocity() / descrete().dt();
    }

    const DescreteSample & descrete() const {
        return descrete_;
    }

    const SoftEncoder & encoder() const {
        return encoder_;
    }

protected:
    void reset() {
        encoder().reset();
    }

    SoftEncoder & encoder() {
        return encoder_;
    }

private:
    SoftEncoder encoder_;
    const DescreteSample & descrete_;
};

class ServoDriveControl  {
public:
    enum ControlKind { controlPosition, controlVelocity, controlSeek };
    ServoDriveControl(const ServoDriveSensors & sensors) :
        sensors(sensors),maxPos(0), minPos(0),
        velocityRegulator_(10.0, 2.5, 1.0, 255., -255., sensors.descrete()),
        positionRegulator_(2.0, 140., -140.),
        targetSpeed_(0),
        basePin(42) {
        reset();
    }

    int targetPosition() const {
        return targetPos_;
    }

    void targetPosition(int val) {
        targetPos_ = val;
        controlKind = controlPosition;
    }

    int positionError() const {
        switch(controlKind) {
            case controlPosition :
                return targetPosition() - sensors.position();
            case controlSeek :
                return 1;
        }
        return 0;
    }

    int targetSpeed() const {
        return targetSpeed_;
    }

    void targetSpeed( int v ) {
        targetSpeed_ = v;
        controlKind = controlVelocity;
    }

    int speedError() const {
        return targetSpeed_ - sensors.speed();
    }

    void setMaxSpeed(byte v) {
        //positionRegulator.maximum(v);
    }

    void seek(int speed) {
        targetSpeed_ = speed;
        controlKind = controlSeek;
    }

    void reset() {
        targetSpeed_ = targetPos_ = 0;
    }

    VelocityRegulator & velocityRegulator() {
        return velocityRegulator_;
    }
    PositionRegulator & positionRegulator() {
        return positionRegulator_;
    }

    int control() {
        switch(controlKind) {
            case controlSeek :
                handleSeekError();
            break;
            case controlPosition :
                handlePositionError();
            break;
        }
        return handleSpeedError();
    }
protected:

    int handleSpeedError() {
        const bool positionReached = controlKind == controlPosition && !positionError();
        const int speed = sensors.speed();
        const SoftEncoder & encoder = sensors.encoder();
        int ctl(0);
        if( positionReached ) {
            velocityRegulator().reset();
        } else {
            const int targetPhase = encoder.calculatePhase(speed)/sensors.descrete().dt();
            const int phaseError = targetPhase<encoder.tick() ? 1 : 0;
            ctl = velocityRegulator().control(speedError(), phaseError);
            const bool sameDirection = sign(ctl) == sign(speed);
            if(speed && !sameDirection) {
                ctl = 0;
            }
        }
        return ctl;
    }

    void handlePositionError() {
        targetSpeed_ = positionRegulator().control(positionError());
    }

    void handleSeekError() {
        if(basePin.low()) {
            targetPosition(sensors.position());
        }
    }
private:
    const ServoDriveSensors & sensors;
    int targetPos_;
    int maxPos;
    int minPos;
    VelocityRegulator velocityRegulator_;
    PositionRegulator positionRegulator_;
    int targetSpeed_;
    ControlKind controlKind;
    TtlInPin basePin;
};

class ServoDrive : public ServoDriveSensors  {
public:
    enum ControlKind { controlPosition, controlVelocity, controlSeek };
    ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber,
            const DescreteSample & descrete) :
            ServoDriveSensors( pulsePinNumber, descrete),
            controlSystem(*this),
            driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
            basePin(42) {
        reset();
    }

    int targetPosition() const {
        return controlSystem.targetPosition();
    }

    void targetPosition(int val) {
        controlSystem.targetPosition(val);
        controlKind = controlPosition;
    }

    int positionError() const {
        return controlSystem.positionError();
    }

    int targetSpeed() const {
        return controlSystem.targetSpeed();
    }

    void targetSpeed( int v ) {
        controlSystem.targetSpeed(v);
        controlKind = controlVelocity;
    }

    int speedError() const {
        return controlSystem.speedError();
    }

    void setMaxSpeed(byte v) {
        controlSystem.setMaxSpeed(v);
    }

    void seek(int speed) {
        controlSystem.seek(speed);
        controlKind = controlSeek;
    }

    void reset() {
        controlSystem.reset();
        ServoDriveSensors::reset();
    }

    VelocityRegulator & velocityRegulator() {
        return controlSystem.velocityRegulator();
    }
    PositionRegulator & positionRegulator() {
        return controlSystem.positionRegulator();
    }
    int out() const {
        return driver.out();
    }
    void deadZone( int v ) {
        driver.deadZone(v);
    }

    void control() {
        encoder().update();
        driver.out( controlSystem.control() );
        updateEncoderDirection();
    }
protected:
    void updateEncoderDirection() {
        if (out()) {
            encoder().setDirection(sign(out()));
        }
    }
private:
    PwmDriver driver;
    ServoDriveControl controlSystem;
    ControlKind controlKind;
    TtlInPin basePin;
};

#endif // DRIVES_H_165506122012

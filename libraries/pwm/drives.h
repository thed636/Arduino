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
  ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber)
  : encoder_(pulsePinNumber), 
    driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
    maxX(0), minX(0),
    speedRegulator(10.0, 2.5, 1.0),
    positionRegulator(2.0){
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

  void target( int val ) {
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

  void setMaxSpeed( byte v ) {
    //positionRegulator.maximum(v);
  }

  void calibrate() {
    TtlInPin pin(30);
    int x0 = position();
    setSpeed(150);
    while(pin.high()) {
      x0 = encoder().update();
    }
    setSpeed( 0 );
    while(speed())
      encoder().update();
//    target(x0);
//    while(speed()||error())
//      update();
    reset();
  }

  void reset() {
    u = 0;
    encoder().reset();
  }
protected:
  void setSpeed( int v ) {
    const int e = v - speed();
    if( !v && !e ) {
      driver.stop();
      speedRegulator.reset();
    } else {
      const int out = speedRegulator.control(e);
      driver.out(out);
    }
    if(v) {
      encoder().setDirection(sign(v));
    }
  }

  void control() {
    setSpeed(positionRegulator.control(error()));
  }

private:
  SoftEncoder encoder_;
  PwmDriver driver;
  int u;
  int maxX;
  int minX;
  SpeedRegulator speedRegulator;
  PositionRegulator positionRegulator;
};


#endif // DRIVES_H_165506122012
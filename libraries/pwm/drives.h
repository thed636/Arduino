#ifndef DRIVES_H_165506122012
#define DRIVES_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>

inline int sign(int v) {
  return v<0 ? -1 : 1;
}

class ServoDrive {
public:
  ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber)
  : trigger(pulsePinNumber), 
    driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
    maxX(0), minX(0), maxSpeed(255), minSpeed(70){
    reset();
  }

  int position() const {
    return x;
  }

  void reset() {
    x = 0;
    u = 0;
    stop();
    trigger.reset();
    time = millis();
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
    if( trigger.changed() ) {
      trigger.reset();
      x+=driver.direction();
      updateTime();
    }
    control();
    return error();
  }

  int speed() const {
    return 1000/dt;
  }

  void setMaxSpeed( byte v ) {
    maxSpeed = max(v, minSpeed);
  }
protected:
  void stop() {
    driver.stop();
    dt = 10000;
  }

  virtual void control() {
    if( error() ) {
      const int speed = min(abs(error())+minSpeed, maxSpeed) * sign(error());
      driver.speed(speed);
    } else {
      stop();
    }
  }

  void updateTime() {
    unsigned long t = millis();
    dt = t - time;
    time = t;
  }
private:
  Trigger trigger;
  PwmDriver driver;
  int u;
  int x;
  int maxX;
  int minX;
  byte maxSpeed;
  byte minSpeed;
  unsigned long time;
  unsigned long dt;
};


#endif // DRIVES_H_165506122012
#ifndef DRIVES_H_165506122012
#define DRIVES_H_165506122012

#include <pwm_driver.h>
#include <ttlin_pin.h>

#define sign(v) (v<0 ? -1 : 1)

class DescreteSample {
public:
  enum {dtDefault = 2};
  DescreteSample(unsigned long dt = dtDefault) : dt_(dt), time_(millis()) {}
  bool tick() {
    unsigned long t = millis();
    if(t-time_ < dt_) {
      return false;
    }
    time_ = t;
    return true;
  }
  
  unsigned long dt() const {
     return dt_;
  }
  
  unsigned long time() const {
    return time_;
  }
private:
  unsigned long time_;
  unsigned long dt_;
};

class SoftEncoder {
public:
  SoftEncoder(byte pulsePinNumber)
  : trigger(pulsePinNumber), lastEdgeTime(millis()), dt(dtMax), direction(0){
  }

  enum { timeNumerator = 1000, dtMax };

  int position() const {
    return x;
  }

  int update() {
    if( trigger.changed() ) {
      trigger.reset();
      x+=direction;
      resetPulseTime();
    } else {
      updatePulseTime();
    }
    return x;
  }

  int velocity() const {
    return timeNumerator/dt * direction;
  }

  void reset() {
    x = 0;
    trigger.reset();
  }
  
  void setDirection( byte v ) {
    direction = v;
  }

protected:
  void resetPulseTime() {
    unsigned long t = millis();
    dt = t - lastEdgeTime;
    lastEdgeTime = t;
  }

  void updatePulseTime() {
    if(dt<dtMax) {
      dt = max(dt, millis() - lastEdgeTime);
    }
  }

private:
  Trigger trigger;
  int x;
  unsigned long lastEdgeTime;
  unsigned long dt;
  byte direction;
};

class PidRegulator {
public:
  enum { scaleFactor = 1000 };
  PidRegulator( float Kp = 1., float Kip = 1., float Kdp = 0., float T = float(DescreteSample::dtDefault)/1000. ) 
  : e_1(0), Kp(Kp*scaleFactor), Kdi(Kp*Kip*T*scaleFactor), Kdd(Kp*Kdp/T*scaleFactor) {}

  int control( int error ) {
    const long e = error;
    long u = u_1 + Kp*(e - e_1) + Kdi*e + Kdd*(e - 2*e_1 + e_2);
    u_1 = u;
    e_2 = e_1;
    e_1 = e;
    return u/scaleFactor;
  }

  void reset() {
    e_1 = e_2 = 0;
    u_1 = 0;
  }
private:
  int e_1;
  int e_2;
  int Kp;
  int Kdi;
  int Kdd;
  long u_1;
};


typedef PidRegulator SpeedRegulator;

class PositionRegulator {
public:
  enum { scaleFactor = SpeedRegulator::scaleFactor };
  PositionRegulator( float k = 1. ) : k(k*scaleFactor), maximum_(255) {}
  int maximum() const {
    return maximum_;
  }
  void maximum( int v ) {
    maximum_ = v;
  }
  int control( int error ) const {
    const long e = error;
    const long speed = min(abs(e) * k, long(maximum())) * sign(e);
    return speed/scaleFactor;
  }
private:
  int k;
  int maximum_;
};

class ServoDrive {
public:
  ServoDrive(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber, byte pulsePinNumber)
  : encoder_(pulsePinNumber), 
    driver(cwPinNumber, ccwPinNumber, pwmPinNumber),
    maxX(0), minX(0){
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
    positionRegulator.maximum(v);
  }

  void calibrate() {
    TtlInPin pin(30);
    int x0 = position();
    setSpeed(50);
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
protected:
  void reset() {
    u = 0;
    encoder().reset();
  }

  void setSpeed( int v ) {
    const int out = speedRegulator.control(speed() - v);
    driver.out(out);
    if(out) {
      encoder().setDirection(sign(out));
    }
  }

  virtual void control() {
    //if( error() ) {
      setSpeed(positionRegulator.control(error()));
    //} else {
    //  setSpeed(0);
    //}
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
#ifndef PWM_DRIVER_H_031220122303
  #define PWM_DRIVER_H_031220122303

  #include <Arduino.h> 
  #include "pwm_pin.h"
  #include "ttlout_pin.h"

class PwmDriver {
public:
  PwmDriver(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber)
   : cw(cwPinNumber), ccw(ccwPinNumber), pwm(pwmPinNumber) {
    stop();
  }

  inline void stop() {
    speed(0);
  }

  int direction() const {
    return speed() ? (speed() > 0 ? 1 : -1) : 0;
  }
  
  int speed() const {
    return speed_;
  }

  void speed( int v ) {
    if( v!= speed() ) {
      speed_ = v;
      if( direction() ) {
        setCcw( direction() == 1 );
        pwm.value( direction() * speed() );
      } else {
        pwm.value(0);
        cw.high();
        ccw.high();
        delay(10);
        cw.low();
        ccw.low();
      }
    }
  }

protected:
  void setCcw( bool v ) {
    cw.state(!v);
    ccw.state(v);
  }
private:
  TtlOutPin cw;
  TtlOutPin ccw;
  PwmPin pwm;
  int speed_;
};
#endif
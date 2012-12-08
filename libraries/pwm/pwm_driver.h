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
  enum{ maxOutValue=255, minOutValue=-255  };
  inline void stop() {
    out(0);
  }

  int out() const {
    return out_;
  }

  void out( int v ) {
    v = max(min(v,maxOutValue), minOutValue);
    if( v!= out() ) {
      out_ = v;
      if( out_ ) {
        setCcw( out_>0 );
        pwm.value( abs(out_) );
      } else {
        pwm.value(0);
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
  int out_;
};
#endif
#ifndef PWM_DRIVER_H_031220122303
  #define PWM_DRIVER_H_031220122303

  #include <Arduino.h> 
  #include "pwm_pin.h"
  #include "ttlout_pin.h"

class PwmDriver {
public:
  PwmDriver(byte cwPinNumber, byte ccwPinNumber, byte pwmPinNumber)
   : cwP(cwPinNumber), ccw(ccwPinNumber), pwm(pwmPinNumber) {
    stop();
  }
  inline void stop() {
    cw.low();
    ccw.low();
  }
  inline void start( byte speed, bool ccwDirection ) {
    pwm.value(speed);
    cw.state(!ccwDirection);
    ccw.state(ccwDirection);
  }
private:
  TtlOutPin cw;
  TtlOutPin ccw;
  PwmPin pwm;
}
#endif
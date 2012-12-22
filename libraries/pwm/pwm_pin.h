#ifndef PWM_PIN_H_031220122303
  #define PWM_PIN_H_031220122303
#include <Arduino.h> 

class PwmPin {
public:
  PwmPin( byte pinNumber ) : pin(pinNumber) {
    pinMode(pin, OUTPUT);
  }
  inline void value(byte v) {
    analogWrite(number(), v);
  }
  inline byte number() const {
    return pin;
  }
  inline void reset() {
    digitalWrite(number(),0);
  }
  static const int maxValue;
private:
  const byte pin;
};

#endif //PWM_PIN_H_031220122303

#ifndef TTL_IN_PIN_H_031220122303
  #define TTL_IN_PIN_H_031220122303

  #include <Arduino.h> 

class TtlInPin {
public:
  TtlInPin( byte pinNumber ) : pin(pinNumber) {
    pinMode(pin, INPUT);
  }
  inline bool high() const {
    return digitalRead(number()) == HIGH;
  }
  inline bool low() const {
    return !high();
  }
  inline byte number() const {
    return pin;
  }
private:
  const byte pin;
};

#endif //TTL_IN_PIN_H_031220122303
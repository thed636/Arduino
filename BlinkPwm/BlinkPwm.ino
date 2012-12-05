#include <pwm_pin.h>

PwmPin ledPin(2);



#pragma pack(1)
const uint32_t packetSize = 16;

struct ArduinoMessage {
    uint8_t type;
    ArduinoMessage( char type ) : type(type) {}
};

struct PwmOut : public ArduinoMessage {
    enum {messageType = 0x1, freeSize = packetSize - sizeof(ArduinoMessage) - 3};
    uint8_t pin;
    uint8_t value;
    uint8_t zero;
    uint8_t dummy[freeSize];
    PwmOut( uint8_t pin, uint8_t value ) : ArduinoMessage(messageType), pin(pin), value(value), zero(0) {}
};

struct MoveDriveTo : public ArduinoMessage {
    enum {messageType = 0x2, freeSize = packetSize - sizeof(ArduinoMessage) - 7};
    uint8_t drive;
    uint8_t speed;
    int32_t x;
    uint8_t zero;
    uint8_t dummy[freeSize];
    MoveDriveTo(uint8_t drive, uint8_t speed, int32_t x)
    : ArduinoMessage(messageType), drive(drive), speed(speed), x(x), zero(0) {}
};

struct ArduinoResponse {
    uint8_t result;
    ArduinoResponse(uint8_t result = 0) : result(result) {}
};

#pragma pack()

char inBuf[packetSize];
uint8_t outBuf[packetSize];

void lightOn() {
  for( int i=0; i!=255; ++i ) {
    ledPin.value(i);
    delay(1);
  }
}

void lightOff(){
  for( int i=255; i!=0; --i ) {
    ledPin.value(i);
    delay(1);
  }
}
// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);
  Serial.flush();
  while( Serial.available() )
    Serial.read();
  
  for( int i=0; i!=packetSize; ++i ) {
    inBuf[i]=0x1;
    outBuf[i]=0x1;
  }
  lightOn();
  lightOff();
  lightOn();
  lightOff();
  Serial.write(0x22);
}


// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available() >= packetSize) {  //если есть доступные данные
    // считываем байт
    for( int i = 0; i!= packetSize; ++i )
      inBuf[i]= Serial.read();
      //if( inBuf[0] == 0xF ) {
    switch( ((ArduinoMessage*)inBuf)->type ) {
      case PwmOut::messageType:
      {
        const PwmOut & msg(*((PwmOut*)inBuf));
        PwmPin pin(msg.pin);
        pin.value(msg.value);
        ArduinoResponse & response(*((ArduinoResponse*)outBuf));
        response.result = 0;
      }
      default:;
    }
    //ledPin.value(inBuf[1]);
    Serial.flush();
      //lightOn();
    Serial.write((uint8_t*)outBuf, packetSize);
      //lightOff();
  }
}

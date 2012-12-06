#include <pwm_driver.h>
#include "D:\workspace\arductl\src\messages.h"
PwmDriver drive1(22,23,2);


// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);
  Serial.flush();
  while( Serial.available() )
    Serial.read();

  Serial.write(0x22);
}

//Here can be just Serial.peek()
#define MESSAGE_HANDLERS_BEGIN switch( Serial.peek() ) {

#define MESSAGE_HANDLERS_END default:;}

#define HANDLE_MESSAGE_BEGIN(Request,Response)\
  case Request::messageType : {\
    typedef Response __Resp; \
    if(Serial.available()>=sizeof(Request)) { \
      Request request;\
      char * inBuf((char*)(&request));\
      for( int i = 0; i!= sizeof(Request); ++i ) \
        inBuf[i]= Serial.read(); \
      Response response;
         
#define HANDLE_MESSAGE_END Serial.flush(); Serial.write((uint8_t*)(&response), sizeof(__Resp));}} break

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available()) {  //если есть доступные данные
    MESSAGE_HANDLERS_BEGIN
      HANDLE_MESSAGE_BEGIN(PwmOut,ArduinoResponse)
        PwmPin pin(request.pin);
        pin.value(request.value);
        response.result = 0;
      HANDLE_MESSAGE_END;
      HANDLE_MESSAGE_BEGIN(MoveDriveTo,ArduinoResponse)
        drive1.start(request.speed,request.x>0);
        response.result = 0;
      HANDLE_MESSAGE_END;
      HANDLE_MESSAGE_BEGIN(StopDrive,ArduinoResponse)
        drive1.stop();
        response.result = 0;
      HANDLE_MESSAGE_END;
    MESSAGE_HANDLERS_END;
  }
}

//// the loop routine runs over and over again forever:
//void loop() {
//  if (Serial.available() >= packetSize) {  //если есть доступные данные
//    // считываем байт
//    for( int i = 0; i!= packetSize; ++i )
//      inBuf[i]= Serial.read();
//      //if( inBuf[0] == 0xF ) {
//    MESSAGE_HANDLERS_BEGIN
//      HANDLE_MESSAGE_BEGIN(PwmOut,ArduinoResponse)
//        PwmPin pin(request.pin);
//        pin.value(request.value);
//        response.result = 0;
//      HANDLE_MESSAGE_END;
//      HANDLE_MESSAGE_BEGIN(MoveDriveTo,ArduinoResponse)
//        drive1.start(request.speed,request.x>0);
//        response.result = 0;
//      HANDLE_MESSAGE_END;
//      HANDLE_MESSAGE_BEGIN(StopDrive,ArduinoResponse)
//        drive1.stop();
//        response.result = 0;
//      HANDLE_MESSAGE_END;
//    MESSAGE_HANDLERS_END;
//    //ledPin.value(inBuf[1]);
//    Serial.flush();
//      //lightOn();
//    Serial.write((uint8_t*)outBuf, packetSize);
//      //lightOff();
//  }
//}

#include <pwm_driver.h>
#include <drives.h>
#include "D:\workspace\arductl\src\messages.h"
PwmDriver drive1(22,23,2);
ServoDrive baseDrive(22,23,2,24);


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
        baseDrive.setMaxSpeed(request.speed);
        baseDrive.target(request.x);
        response.result = 0;
      HANDLE_MESSAGE_END;
      HANDLE_MESSAGE_BEGIN(GetDrivePosition,DrivePosition)
        response.pos = baseDrive.position();
      HANDLE_MESSAGE_END;
      HANDLE_MESSAGE_BEGIN(GetDriveSpeed,DriveSpeed)
        response.speed = baseDrive.speed();
      HANDLE_MESSAGE_END;
      HANDLE_MESSAGE_BEGIN(GetDrivePositionError,DrivePositionError)
        response.error = baseDrive.error();
      HANDLE_MESSAGE_END;
      HANDLE_MESSAGE_BEGIN(StopDrive,ArduinoResponse)
        drive1.stop();
        response.result = 0;
      HANDLE_MESSAGE_END;
    MESSAGE_HANDLERS_END;
  }
  baseDrive.update();
}


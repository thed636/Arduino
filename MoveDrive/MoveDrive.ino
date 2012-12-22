#include <pwm_driver.h>
#include <drives.h>
#include "D:\workspace\arductl\src\messages.h"

DescreteSample descrete;

const unsigned drivesCount = 5;

ServoDrive drives[drivesCount] = { 
  ServoDrive(22,24,2,44,descrete), 
  ServoDrive(26,28,3,46,descrete),
  ServoDrive(30,32,4,48,descrete),
  ServoDrive(34,36,5,50,descrete),
  ServoDrive(38,40,6,52,descrete) };

ServoDrive & baseDrive(drives[0]);
ServoDrive & shoulderDrive(drives[1]);
ServoDrive & elbowDrive(drives[2]);
ServoDrive & tiltDrive(drives[3]);
ServoDrive & gripDrive(drives[4]);

// the setup routine runs once when you press reset:
void setup() {                
  Serial.begin(115200);
  Serial.flush();
  while( Serial.available() )
    Serial.read();
  Serial.write(0x22);
}

void control() {
  if( descrete.tick() ) {
    for(byte i(0); i!=drivesCount; ++i) {
      drives[i].control();
    }
  }
}

void readFromSerial( char * buf, uint8_t size ) {
  for( uint8_t i = 0; i!= size; ++i ) {
    buf[i]= Serial.read();
    control();
  }
}

void writeToSerial( const uint8_t * buf, uint8_t size ) {
  for( uint8_t i = 0; i!= size; ++i ) {
    Serial.write(buf[i]);
    control();
  }
}

//Here can be just Serial.peek()
#define MESSAGE_HANDLERS_BEGIN switch( Serial.peek() ) {

#define MESSAGE_HANDLERS_END default:;}

#define HANDLER_BEGIN(Message)\
  case Message::Request::messageType : {\
    typedef Message::Request Request;\
    typedef Message::Response Response; \
    if(Serial.available()>=sizeof(Request)) { \
      Request request;\
      readFromSerial((char*)(&request), sizeof(Request));\
      Response response;
         
#define HANDLER_END Serial.flush(); writeToSerial((uint8_t*)(&response), sizeof(Response));}} break

ServoDrive & getGrive(const DriveRequest & request) {
  return drives[request.drive-1];
}
// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available()) {  //если есть доступные данные
    MESSAGE_HANDLERS_BEGIN
      HANDLER_BEGIN(PwmOut)
        PwmPin pin(request.pin);
        pin.value(request.value);
        response.result = 0;
      HANDLER_END;

      HANDLER_BEGIN(MoveTo)
        ServoDrive & drive(getGrive(request));
        drive.targetPosition(request.x);
        response.result = 0;
      HANDLER_END;

      HANDLER_BEGIN(MoveSpeed)
        ServoDrive & drive(getGrive(request));
        drive.targetSpeed(request.speed);
      HANDLER_END;

      HANDLER_BEGIN(SeekDriveSpeed)
        ServoDrive & drive(getGrive(request));
        drive.seek(request.speed);
      HANDLER_END;

      HANDLER_BEGIN(ResetDrive)
        ServoDrive & drive(getGrive(request));
        drive.reset();
      HANDLER_END;

      HANDLER_BEGIN(ConfigureDrive)
        ServoDrive & drive(getGrive(request));
        drive.reset();
        drive.velocityRegulator().configure(request.speed.Kp(), request.speed.Kip(), request.speed.Kdp());
        drive.positionRegulator().configure(request.pos.Kp());
        drive.deadZone(request.pwm.deadZone);
      HANDLER_END;

      HANDLER_BEGIN(GetDriveState)
        ServoDrive & drive(getGrive(request));
        response.state.pos = drive.position();
        response.state.error = drive.positionError();
        response.state.speed = drive.speed();
        response.state.speedError = drive.speedError();
        response.state.out = drive.out();
      HANDLER_END;
    MESSAGE_HANDLERS_END;
  }
  control();
}


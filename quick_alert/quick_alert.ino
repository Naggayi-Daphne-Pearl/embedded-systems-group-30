#include <Sim800L.h>
#include <SoftwareSerial.h>

Sim800L Sim800L(16, 17); 

void setup(){
 Sim800L.begin(4800);    
 //Sim800L.sendSms("+256702439337","Hello from ESP");
}

void loop(){


 //do nothing
if (Sim800L){
  Serial.println('Available');
} else{

Serial.println('Not available');

}
}
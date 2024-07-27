#include <Sim800L.h>
#include <SoftwareSerial.h>

#include <DHT11.h>
//#include <Buzzer.h>



const int buzzer = 0;
const int redLed = 2;
const int greenLed = 15;
const int smokeSensor = 12;
const int maxTemp=27;
const int maxGasLevel=35;
const int phoneCallLength=15000;
message=" ";

const char* contacts[] = {"+256702439337"};
int numContacts = sizeof(contacts) / sizeof(contacts[0]);
//Buzzer buzzer(buzzerPin);
Sim800L Sim800L(16, 17);

DHT11 dhtSensor(4);


void setup() {
  Serial.begin(9600); 
  Sim800L.begin(9600);
  Serial.println("Setting up");
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  
  Serial.println("GSM Module Communication Starting...");

}

void GasSmokeLevel() {
  int value = analogRead(smokeSensor);
  // delay(500);
  int newvalue = map(value, 0, 4095, 0, 100);
  // delay(500);
  Serial.print("GasLevel: ");
  Serial.println(newvalue);

  if (newvalue > maxGasLevel) {
      //digitalWrite(buzzer, HIGH);
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);// Increment the index
    //sendSms();
    delay(10000);
    readSms();//
  }
   else {
    //digitalWrite(buzzer, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    delay(2000);
  }
}

void readSms() {
  int index=AT+CMTI;// returns index of incoming message Cellular Message Timeline
  while(index>0){
    String msg=Sim800L.print("AT+CMGR");
    Serial.print(msg);
    index=index+1;
    message=msg;
    msg=" ";
  }
}

void reply(){
  
if(message=="Temperature"){


}
void call(){
  int i;
  for(i=0;i<numContacts;i++){
  Sim800L.print("ATD"); // Send the ATD command
  Sim800L.print((char*) contacts[i]); // Send the phone number
  Sim800L.println(";"); // End the command with a semicolon and newline
  Serial.print("Calling: ");
  Serial.println((char*) contacts[i]);

  delay(phoneCallLength); // Wait for 10 seconds (simulate an active call)

  // Hang up the call
  Sim800L.println("ATH"); // Send the ATH command to hang up
  Serial.println("Call ended.");
  delay(5000);

  }
 
  

}

void sendSms(String message){

int i;

 for(i=0;i<numContacts;i++){
Sim800L.println("AT+CMGF=1");     // Set SMS mode to text
 delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read()); // Print the response to the serial monitor
  }

Sim800L.print("AT+CMGS=\"");
 Sim800L.print((char*) contacts[i]);
Sim800L.println("\"");
 delay(1000);
   while (Sim800L.available()) {
    Serial.write(Sim800L.read()); // Print the response to the serial monitor
  }
 Sim800L.print(message);
   delay(1000);
Sim800L.write(26);
delay(5000);
}



}




void loop() {

  call();
 //GasSmokeLevel();
  int temperature = dhtSensor.readTemperature();
  //readSms();

  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    delay(1000);  

    if (temperature > maxTemp) {
      // sending sms to multiple contacts
      //call();
      
      //Sim800L.callNumber("+256702439337");
      digitalWrite(redLed, HIGH);  // Turn on red LED
      digitalWrite(buzzer, HIGH);
      digitalWrite(greenLed, LOW);
      // Turn off green LED
      //Sim800L.callNumber("+256785796401")
    }
  
    //delay(3000);
    else {
      digitalWrite(redLed, LOW);  // Turn off red LED
      digitalWrite(greenLed, HIGH);
      digitalWrite(buzzer, LOW);  // Turn on green LED
    } 
  }

   else {
    // Print error message based on the error code.
    Serial.println("Cannot Read Temperature"); 
  }
  
}
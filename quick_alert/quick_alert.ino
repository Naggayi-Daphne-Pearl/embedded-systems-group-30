#include <Sim800L.h>
#include <SoftwareSerial.h>
#include <DHT11.h>
#include <Buzzer.h>

const int buzzer = 0;
const int redLed = 2;
const int greenLed = 15;
const int smokeSensor = 12;
 
const char* contacts[] = {"+256702439337"}; //"+256785796401", "+256755643774"};
int numContacts = sizeof(contacts) / sizeof(contacts[0]);
//Buzzer buzzer(buzzerPin);
Sim800L Sim800L(16, 17);

DHT11 dhtSensor(4);


void setup() {
  Serial.begin(9600);
  Sim800L.begin(4800);
Serial.println("Setting up");
  //Sim800L.sendSms("+256785796401","Hello from Quick Alert System");
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void GasSmokeLevel() {
  int value = analogRead(smokeSensor);
  // delay(500);
  int newvalue = map(value, 0, 4095, 0, 100);
  // delay(500);
  Serial.print("GasLevel: ");
  Serial.println(newvalue);
  if (newvalue > 50) {
    //digitalWrite(buzzer, HIGH);
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
  } else {
    //digitalWrite(buzzer, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
  }
}

// void readSms(){
//   char buffer[256];
//   if (Sim800L.available()) {
//     Serial.println("Incoming SMS detected");
//     // int messageLength = Sim800L.getSms(buffer, sizeof(buffer) - 1);
//     // if (messageLength > 0) {
//     //   buffer[messageLength] = '\0';  // Null-terminate the buffer
//     //   Serial.print("Received SMS: ");
//     //   Serial.println(buffer);
//     // } 
//   }
// }
void loop() {
  int temperature = dhtSensor.readTemperature();
  //readSms();
  //delay(3000);
  // sending sms to multiple contacts
  int i;
  //for(i=0;i<numContacts;++i){
    //bool result = Sim800L.sendSms((char*)contacts[i], "Hello from Quick Alert System");
    //Serial.print("SMS send result: ");
    //Serial.println(result ? "Success" : "Failure");
    //delay(200);
    
 // }
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  GasSmokeLevel();
  Serial.println(sizeof(contacts));
  Serial.println(sizeof(contacts[0]));
  Serial.println(numContacts);

  if (temperature > 28) {
    Sim800L.callNumber("+256785796401");
    // Sim800L.sendSms("+256702439337", "Hello from Quick Alert System");
    delay(1000);
    digitalWrite(redLed, HIGH);  // Turn on red LED
    digitalWrite(buzzer, HIGH);
    digitalWrite(greenLed, LOW);  // Turn off green LED

    //Sim800L.callNumber("+256785796401");

  }
  //delay(3000);
  else if (temperature < 28) {
    digitalWrite(redLed, LOW);  // Turn off red LED
    digitalWrite(greenLed, HIGH);
    digitalWrite(buzzer, LOW);  // Turn on green LED

  } else if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  } else {
    // Print error message based on the error code.
    Serial.println("Cannot Read Temperature"); 
  }
  delay(5000);  // Adjust delay as needed for your application
}
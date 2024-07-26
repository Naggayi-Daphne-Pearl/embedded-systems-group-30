#include <Sim800L.h>
#include <SoftwareSerial.h>
#include <DHT11.h>
//#include <Buzzer.h>

const int buzzer = 0;
const int redLed = 2;
const int greenLed = 15;
const int smokeSensor = 12;
const int maxSmokeLevel=21;
const int phoneCallLength=10000;

 
const char* contacts[] = {"+256773367078"};//
int numContacts = sizeof(contacts) / sizeof(contacts[0]);
//Buzzer buzzer(buzzerPin);
Sim800L Sim800L(16, 17);

DHT11 dhtSensor(4);


void setup() {
  Serial.begin(9600);
  Sim800L.begin(4800);
  Serial.println("Setting up");
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
  if (newvalue > maxSmokeLevel) {
   // Sim800L.sendSms("+256755643774","The Gas Level is ");
   Serial.println((char*)contacts[0]);
   Sim800L.callNumber((char*)contacts[0]);
 delay(10000);
 Sim800L.hangoffCall();
   
int i=0;
while(i<6){

Serial.println("Here......");
  i++;
  delay(5000);
}

    //digitalWrite(buzzer, HIGH);
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);

  } else {
    //digitalWrite(buzzer, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    delay(2000);
  }
}

void readSms() {
  String smsCountStr = Sim800L.getNumberSms(1);
  int smsCount = smsCountStr.toInt(); // Check the number of SMS messages in the inbox
  if (smsCount > 0) {  
    for (int i = 1; i <= smsCount; i++) {
      String message = Sim800L.readSms(i); // Read the SMS at index i
      if (message.length() > 0) {
        Serial.print("Received SMS: ");
        Serial.println(message);

        //Sim800L.deleteSms(i); // Optionally, delete the SMS after reading
      }
    }
  }
  
}
void loop() {
  int temperature = dhtSensor.readTemperature();
  
  GasSmokeLevel();
  readSms();
  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    delay(1000);  

    if (temperature > 28) {
      // sending sms to multiple contacts
    int i;
    for(i=0;i<numContacts;++i){
      bool result = Sim800L.sendSms((char*)contacts[i], "Hello from Quick Alert System");
      Serial.print("SMS send result: ");
      Serial.println(result ? "Success" : "Failure");
      delay(200);
    }

      Sim800L.callNumber("+256702439337");
      //digitalWrite(redLed, HIGH);  // Turn on red LED
      //digitalWrite(buzzer, HIGH);
      //digitalWrite(greenLed, LOW);  // Turn off green LED
      //Sim800L.callNumber("+256785796401")
    }
  
    //delay(3000);
    else {
      //digitalWrite(redLed, LOW);  // Turn off red LED
      //digitalWrite(greenLed, HIGH);
      //digitalWrite(buzzer, LOW);  // Turn on green LED
    } 
  }

   else {
    // Print error message based on the error code.
    Serial.println("Cannot Read Temperature"); 
  }
  
}
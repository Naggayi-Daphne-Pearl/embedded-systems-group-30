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

  MySerial.begin(9600, SERIAL_8N1, 16, 17); // UART2, TX=16, RX=17
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
  String smsCountStr = Sim800L.getNumberSms(1);
  Serial.print("SMS Count String: ");
  Serial.println(smsCountStr);

  int smsCount = smsCountStr.toInt(); // Convert the number of SMS messages to an integer
  Serial.print("Number of sms: ");
  Serial.println(smsCount);

  if (smsCount > 0) {
    for (int i = 1; i <= smsCount; i++) {
      String message = Sim800L.readSms(i); // Read the SMS at index i
      if (message.length() > 0) {
        Serial.print("Received SMS: ");
        Serial.println(message);

        // Sim800L.deleteSms(i); // Optionally, delete the SMS after reading
      } else {
        Serial.print("Failed to read SMS at index: ");
        Serial.println(i);
      }
    }
  } else {
    Serial.println("No SMS messages found.");
  }
}


void call(String phoneNumber,int phoneCallLength){
  // String phoneNumber = "+256755643774"; // Replace with the actual phone number
  Sim800L.print("ATD"); // Send the ATD command
  Sim800L.print(phoneNumber); // Send the phone number
Sim800L.println(";"); // End the command with a semicolon and newline
  Serial.println("Calling " + phoneNumber);

  delay(phoneCallLength); // Wait for 10 seconds (simulate an active call)

  // Hang up the call
  Sim800L.println("ATH"); // Send the ATH command to hang up
  Serial.println("Call ended.");
  delay(5000);


}


void sendSms(){

   int i;
    for(i=0;i<numContacts;++i){
    bool result = Sim800L.sendSms((char*)contacts[i], "Hello from Quick Alert System");
    Serial.print(result ? "Sent to " : "Failed to send to ");
    Serial.println((char*)contacts[i]);
    delay(3000);
    }
  delay(15000);
}
void loop() {


 GasSmokeLevel();
  int temperature = dhtSensor.readTemperature();
  readSms();

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
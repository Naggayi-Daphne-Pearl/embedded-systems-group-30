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
String message=" ";

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
    delay(1000);
    //readSms();//
  }
   else {
    //digitalWrite(buzzer, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    delay(2000);
  }
}

String readMessage(int index) {
  Sim800L.print("AT+CMGR="); //reads content of message stored on a simcard
  Sim800L.println(index);
  delay(2000); // Wait for the response to ensure it's processed

  String response = "";
  while (Sim800L.available()) {
    response += (char)Sim800L.read();
  }

  // Print the entire response for debugging
  Serial.print("Read Message Response: ");
  Serial.println(response);

  int messageStart = response.indexOf("\r\n", response.indexOf("\r\n") + 2) + 2;
  //locates the actual position of the message response.indexOf("\r\n"): Finds the first line break.response.indexOf("\r\n") + 2: Moves past the first line break.
  //response.indexOf("\r\n", response.indexOf("\r\n") + 2): Finds the next line break.+ 2: Moves past the second line break to the start of the message content.

  int messageEnd = response.indexOf("\r\n", messageStart);
  if (messageStart != -1 && messageEnd != -1) {
    return response.substring(messageStart, messageEnd);
  }

  return "Error: Unable to extract message.";
}

void readSms() {
  // Request to check for new message indices
  Sim800L.println("AT+CMTI=\"SM\""); // Assuming "SM" for SIM storage; adjust as needed
  delay(2000); // Wait for the response to ensure it's processed

  // Read and process the response to get the index of the new message
  String response = "";
  while (Sim800L.available()) {
    response += (char)Sim800L.read();
  }

  // Print the entire response for debugging
  Serial.print("Response: ");
  Serial.println(response);

  // Check if the response contains "+CMTI: "
  int indexStart = response.indexOf("+CMTI: \"SM\",");
  if (indexStart != -1) {
    indexStart += 13; // Move past "+CMTI: \"SM\","
    int indexEnd = response.indexOf("\r\n", indexStart);
    if (indexEnd == -1) indexEnd = response.length(); // Handle case where "\r\n" is not found

    String indexString = response.substring(indexStart, indexEnd);
    int messageIndex = indexString.toInt();
    
    if (messageIndex > 0) {
      Serial.print("Message Index: ");
      Serial.println(messageIndex);
      
      // Proceed to read the message using the index
      String message = readMessage(messageIndex);
      Serial.print("Message: ");
      Serial.println(message);
    } else {
      Serial.println("Invalid message index.");
    }
  } else {
    Serial.println("No new messages found or response format is incorrect.");
  }
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
  //delay(5000);
  while (Sim800L.available()) {
      Serial.read(); // Read and discard old data
    }
  delay(2000);
  
}

}

void loop() {
 
  sendSms("The temperature is :",temperature);
  //delay(10000);
 // readSms();
  //call();
 //GasSmokeLevel();
  int temperature = dhtSensor.readTemperature();
  //readSms();

  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    delay(1000);  

    if (temperature > maxTemp) {
      
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
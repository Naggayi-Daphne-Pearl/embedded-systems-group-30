#include <Sim800L.h>
#include <SoftwareSerial.h>

#include <DHT11.h>
//#include <Buzzer.h>



const int buzzer = 0;
const int redLed = 2;
const int yellowLed = 15;
const int smokeSensor = 12;
const int maxTemp=26;
const int maxGasLevel=19;
const int phoneCallLength=15000;

const char* contacts[] = {"+256702439337"};
int numContacts = sizeof(contacts) / sizeof(contacts[0]);
//Buzzer buzzer(buzzerPin);
Sim800L Sim800L(16, 17);

DHT11 dhtSensor(4);


void setup() {
  Serial.begin(9600); 
  Sim800L.begin(4800);
  Serial.println("Setting up");
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Sim800L.print("AT+CMGF=1\r");
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  // Check and print current SMS storage settings
  Sim800L.print("AT+CPMS?\r");
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  // Set SMS storage to SIM card
  Sim800L.print("AT+CPMS=\"SM\",\"SM\",\"SM\"\r");
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  // Check network registration
  Sim800L.print("AT+CREG?\r");
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  // Check signal strength
  Sim800L.print("AT+CSQ\r");
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }
}

void GasSmokeLevel() {
  int value = analogRead(smokeSensor);
  // delay(500);
  int newvalue = map(value, 0, 4095, 0, 100);
  // delay(500);
  Serial.print("GasLevel: ");
  Serial.println(newvalue);

  if (newvalue > maxGasLevel) {
   
    digitalWrite(buzzer, HIGH);
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);// Increment the index
    call();
  }
   else {
    digitalWrite(buzzer, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    delay(2000);
  }
}

void readSms() {
  // Clear the serial buffer before sending a new command
  while (Sim800L.available()) {
    Sim800L.read();
  }

  // List all SMS messages to get the total count
  Sim800L.print("AT+CMGL=\"ALL\"\r");
  delay(2000);  // Wait for the module to respond

  // Read the response from the module
  String response = "";
  while (Sim800L.available()) {
    char c = Sim800L.read();
    response += c;
  }

  // Print the raw response for debugging
  Serial.print("Raw response: ");
  Serial.println(response);

  // Extract SMS messages from the response
  int smsCount = extractSmsCount(response);
  Serial.print("Parsed SMS Count: ");
  Serial.println(smsCount);

  if (smsCount > 0) {
    for (int i = 1; i <= smsCount; i++) {
      String message = readSmsByIndex(i); // Read the SMS at index i
      if (message.length() > 0) {
        Serial.print("Received SMS: ");
        Serial.println(message);

          // Extract sender's phone number
        int startIdx = response.indexOf("+CMGL:", i) + 7; // Adjust to extract the sender's number
        int endIdx = response.indexOf(",", startIdx);
        String sender = response.substring(startIdx, endIdx);
        // Optionally, delete the SMS after reading
        Serial.print(sender);
        readSms();
        
        //processSms(message, sender);
        delay(3000);
        String x=deleteSms(i);
        Serial.print("Deleted message:");
        Serial.println(x);

        
      } else {
        Serial.print("Failed to read SMS at index: ");
        Serial.println(i);
      }
      
    }
  } 
  else {
    Serial.println("No SMS messages found.");
  }
}

// Function to extract SMS count from the response
int extractSmsCount(String response) {
  int count = 0;
  int index = response.indexOf("+CMGL:");
  while (index != -1) {
    count++;
    index = response.indexOf("+CMGL:", index + 1);
  }
  return count;
}

// Function to read SMS by index
String readSmsByIndex(int index) {
  // Clear the serial buffer before sending a new command
  while (Sim800L.available()) {
    Sim800L.read();
  }

  // Send the command to read the SMS at the given index
  Sim800L.print("AT+CMGR=");
  Sim800L.print(index);
  Sim800L.print("\r");
  delay(2000); // Wait for the module to respond

  // Read the response from the module
  String response = "";
  while (Sim800L.available()) {
    char c = Sim800L.read();
    response += c;
  }

  // Print the raw response for debugging
  Serial.print("Raw CMGR response: ");
  Serial.println(response);

  // Extract the message body from the response
  int startIdx = response.indexOf("\r\n") + 2; // Start of the message
  int endIdx = response.lastIndexOf("\r\n"); // End of the message

  if (startIdx != -1 && endIdx != -1) {
    return response.substring(startIdx, endIdx);
  } else {
    return ""; // Failed to read the message
  }
}

void processSms(String message, String sender) {
  Serial.print("Processing SMS from: ");
  Serial.println(sender);
  Serial.print("Message: ");
  Serial.println(message);

  // Send a specific message based on the reply
  sendReplyBasedOnResponse(sender, message);
}

void sendReplyBasedOnResponse(String sender, String response) {
  // Define different responses based on the received message
  String replyMessage = "";

  if (response=="Temperature") {
    replyMessage = "Thank you for your confirmation!";
  // } else if (response==GasLevel) {
  //   replyMessage = "Sorry to hear that. Can you provide more details?";
  } else if (response.indexOf("Help") != -1) {
    replyMessage = "How can we assist you further?";
  } else {
    replyMessage = "Thank you for your response!";
  }

  // Send the reply message
  sendSms(replyMessage.c_str());
}

String deleteSms(int index) {
  // Clear the serial buffer before sending a new command
  while (Sim800L.available()) {
    Sim800L.read();
  }

  // Send the command to delete the SMS at the given index
  Sim800L.print("AT+CMGD=");
  Sim800L.print(index);
  Sim800L.print("\r");
  delay(1000); // Wait for the module to respond

  // Read the response from the module
  String response = "";
  while (Sim800L.available()) {
    char c = Sim800L.read();
    response += c;
  }

  // Print the response for debugging
  Serial.print("Delete SMS response: ");
  Serial.println(response);

  return response;
}

void call(){
  int i;
  for(i=0;i<numContacts;i++){
    Sim800L.print("ATD"); // Send the ATD command
    Sim800L.print((char*)contacts[i]); // Send the phone number
    Sim800L.println(";"); // End the command with a semicolon and newline
    Serial.print("Calling :" );
    Serial.println((char*)contacts[i]);
    delay(phoneCallLength); // Wait for 10 seconds (simulate an active call)

    // Hang up the call
    Sim800L.println("ATH"); // Send the ATH command to hang up
    Serial.println("Call ended.");
    delay(2000);
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
  //readSms();

}
 
  
}

void loop(){
  //readSms();
  //sendSms("The temperature is");
  //delay(1000);
  //deleteSms(1);
  //readSms();
  GasSmokeLevel();
  int temperature = dhtSensor.readTemperature();
  
  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    delay(1000);  

    if (temperature > maxTemp) {
      // sending sms to multiple contacts      call();
      //sendSms();
      //Sim800L.callNumber("+256702439337");
      digitalWrite(redLed, HIGH);  // Turn on red LED
      digitalWrite(buzzer, HIGH);
      digitalWrite(yellowLed, LOW);  // Turn off green LED
      //Sim800L.callNumber("+256785796401")
    }
  
    //delay(3000);
    else {
      digitalWrite(redLed, LOW);  // Turn off red LED
      digitalWrite(yellowLed, HIGH);
      digitalWrite(buzzer, LOW);  // Turn on green LED
    } 
  }

   else {
    // Print error message based on the error code.
    Serial.println("Cannot Read Temperature"); 
  }
  
}
#include <Sim800L.h>
#include <SoftwareSerial.h>

#include <DHT11.h>

//#include <Buzzer.h>


const int buzzer = 0;
const int redLed = 2;
const int yellowLed = 15;
const int smokeSensor = 12;
const int maxTemp=30;
const int maxGasLevel=35;
const int phoneCallLength=15000;

const char* contacts[] = {"+256750000783","+256702439337"};
int numContacts = sizeof(contacts) / sizeof(contacts[0]);

//Buzzer buzzer(buzzerPin);
Sim800L Sim800L(16, 17);

DHT11 dhtSensor(4);


void setup() {
  Serial.begin(9600); 
  Sim800L.begin(9600);//initiating the SIM800L
  Serial.println("Setting up");
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Sim800L.print("AT+CMGF=1\r");//command for setting message format to text
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());//reading from the SIM800L and writing to the serial monitor
  }


  Sim800L.print("AT+CPMS?\r");  // Check and print current SMS storage settings
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  
  Sim800L.print("AT+CPMS=\"SM\",\"SM\",\"SM\"\r");// Set SMS storage to SIM card
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  
  Sim800L.print("AT+CREG?\r");// Check network registration 0 not registered , 1 registered home network ,
  //2 not registered , 3 registration denied,4 unknown , 5 registered
  delay(1000);
  while (Sim800L.available()) {
    Serial.write(Sim800L.read());
  }

  
  Sim800L.print("AT+CSQ\r");// Check signal strength ranging from 0-31
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
    //String message = "Warning: Gas level is high. Current level: " + String(newvalue);
    //sendSms(message);

    digitalWrite(buzzer, HIGH);
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);// Increment the index
    //call();
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

  // Send command to list all unread SMS messages
  Sim800L.println("AT+CMGL=\"REC UNREAD\"");
  delay(2000);  // Allow time for the module to respond

  // Read the response from the module
  String response = "";
  unsigned long startTime = millis(); //millis() is a built-in Arduino function that returns the number of milliseconds since the Arduino board began running the current program.
  while (millis() - startTime < 5000) {  // Wait up to 5 seconds
    if (Sim800L.available()) {
      char c = Sim800L.read();
      response += c;
    }
  }

  // Print the raw response for debugging
  Serial.print("Raw response: ");
  Serial.println(response);

  // Parsing response
  int startIndex = 0;


  while (true) {
    int cmglIndex = response.indexOf("+CMGL:", startIndex); //CMGL lists messages getting the start index of the response
    if (cmglIndex == -1) break;  // No more SMS entries

    int endOfEntry = response.indexOf("\r\n", cmglIndex);
    if (endOfEntry == -1) endOfEntry = response.length();  // End of response if not found

    String entry = response.substring(cmglIndex, endOfEntry);
    Serial.print("SMS Entry: ");
    Serial.println(entry);

    int indexStart = entry.indexOf("+CMGL: ") + 7;
    int indexEnd = entry.indexOf(",", indexStart);//getting index of the first comma
    String indexStr = entry.substring(indexStart, indexEnd);
    int smsIndex = indexStr.toInt();

    Serial.print("SMS Index: ");
    Serial.println(smsIndex);

    //extracting the sender's contact
    int senderStart = entry.indexOf("\",\"", indexEnd) + 3;
    int senderEnd = entry.indexOf("\",\"", senderStart);
    String sender = entry.substring(senderStart, senderEnd);

    Serial.print("Sender: ");
    Serial.println(sender);

    //getting index of the message to extract the actual message
    int contentStart = entry.indexOf("\r\n", senderEnd) + 2;
    int contentEnd = entry.indexOf("\r\n", contentStart);
    if (contentEnd == -1) contentEnd = entry.length();
    String message = entry.substring(contentStart, contentEnd);
    message.trim();//removes any whitespace 

    Serial.print("Message: ");
    Serial.println(message);

    if (message.length() > 0) {
      Serial.print("Received SMS: ");
      Serial.println(message);

      // Process the SMS message
      processSms(message, sender);

      // Optionally, delete the SMS after processing
      String deleteResponse = deleteSms(smsIndex);
      Serial.print("Delete response: ");
      Serial.println(deleteResponse);
    } else {
      Serial.print("Failed to read SMS content at index: ");
      Serial.println(smsIndex);
    }

    startIndex = endOfEntry + 2; // Move past the current entry
  }
}


String deleteSms(int index) {
  // Clear the serial buffer before sending a new command
  while (Sim800L.available()) {
    Sim800L.read();
  }

  // Send command to delete SMS at the specified index
  Sim800L.print("AT+CMGD=");
  Sim800L.print(index);
  Sim800L.print("\r");
  delay(5000);  // Wait for the module to respond

  // Read the response from the module
  String response = "";
  while (Sim800L.available()) {
    char c = Sim800L.read();
    response += c;
  }

  // Print the raw response for debugging
  Serial.print("Delete response: ");
  Serial.println(response);

  return response;
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
 
 // Read the current temperature and gas level
  int temperature = dhtSensor.readTemperature();
  int gasLevel = analogRead(smokeSensor);
  gasLevel = map(gasLevel, 0, 4095, 0, 100);

  // Send a specific message based on the reply
  sendReplyBasedOnResponse(sender, message, temperature, gasLevel);


 
}

void sendReplyBasedOnResponse(String sender, String response,int temperature,int gasLevel) {
  // Define different responses based on the received message
  String replyMessage = "";
  response.trim(); // Trim any extra spaces

  if (response.equalsIgnoreCase("Temperature")) {  // Use equalsIgnoreCase to handle case variations
    replyMessage = "The current temperature is " + String(temperature) + " °C.";
  } else if (response.equalsIgnoreCase("GasLevel")) {  // Use equalsIgnoreCase to handle case variations
    replyMessage = "The current gas level is " + String(gasLevel) + ".";
  } else if (response.indexOf("Help") != -1) {
    replyMessage = "How can we assist you further?";
  } else {
    // replyMessage = "The current temperature is " + String(temperature) + " °C.";
    // replyMessage = "The current gas level is " + String(gasLevel) + ".";
     replyMessage = "Thank you for your response.";
    
  }

  // Send the reply message
  sendSms(replyMessage.c_str());
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


    String response = "";
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) { // Wait up to 5 seconds for a response
      if (Sim800L.available()) {
        char c = Sim800L.read();
        response += c;
      }
    }
    Serial.print("Call response: ");
    Serial.println(response);

    // Check for OK or CONNECT responses to verify the call was made
    if (response.indexOf("OK") != -1 || response.indexOf("CONNECT") != -1) {
      delay(phoneCallLength); // Wait for the duration of the simulated call
    } else {
      Serial.println("Failed to initiate the call.");
    }

    // Hang up the call
    Sim800L.println("ATH"); // Send the ATH command to hang up
    Serial.println("Call ended.");
    delay(2000); //
    
  }

}

void sendSms(String message) {
  for (int i = 0; i < numContacts; i++) {
    // Clear the serial buffer before sending a new command
    while (Sim800L.available()) {
      Sim800L.read();
    }

    // Set SMS mode to text
    Sim800L.println("AT+CMGF=1");//command for sending sms with the format as text
    delay(1000);  // Wait for the module to respond
    while (Sim800L.available()) {
      Serial.write(Sim800L.read()); // Print the response to the serial monitor
    }

    // Set the recipient and start the SMS sending process
    Sim800L.print("AT+CMGS=\"");
    Sim800L.print(contacts[i]);
    Sim800L.println("\"");
    delay(1000);  // Wait for the command to be processed

    // Print the message to the serial monitor
    Serial.print("Sending message: ");
    Serial.println(message);

    // Send the message content
    Sim800L.print(message);
    delay(1000);  // Allow time for the message to be processed

    // End the message with CTRL+Z (ASCII 26)
    Sim800L.write(26); // CTRL+Z
    delay(5000);  // Wait for the module to process the message

    // Read and print the response from the module
    String response = "";
    while (Sim800L.available()) {
      char c = Sim800L.read();
      response += c;
    }

    Serial.print("SMS send response: ");
    Serial.println(response);

    // Optional: Clear any leftover data in the buffer
    while (Sim800L.available()) {
      Sim800L.read();
    }
  }
}




void loop(){
 // Sim800L.callNumber("+256702439337");
  call();
  //readSms();
  //sendSms("The temperature is");
  //delay(1000);
  //deleteSms(1);
  //readSms();
  //GasSmokeLevel();
  int temperature = dhtSensor.readTemperature();
  
  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    delay(1000);  

    if (temperature > maxTemp) {
      String message = "Warning: Temperature is high. Current level: " + String(temperature);
      sendSms(message);
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
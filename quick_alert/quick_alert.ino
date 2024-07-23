#include <Sim800L.h>
#include <SoftwareSerial.h>
#include <DHT11.h>
#include <Buzzer.h>

const int buzzer = 0;
const int redLed = 2;
const int greenLed = 15;
const int smokeSensor = 12;

//Buzzer buzzer(buzzerPin);
Sim800L Sim800L(16, 17);
DHT11 dhtSensor(4);


void setup() {
  Serial.begin(9600);
  Sim800L.begin(4800);
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
void loop() {
  int temperature = dhtSensor.readTemperature();
  //delay(3000);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  GasSmokeLevel();



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
    digitalWrite(buzzer, LOW);  // Turn oo green LED
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
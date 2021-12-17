#include <Wire.h>
#include "SparkFunBME280.h"
BME280 mySensor;
#define DEBUG true

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  sendData("AT+RST\r\n", 2000, false); // reset module
  sendData("AT+CWMODE=2\r\n", 1000, false); // configure as
  //access point
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  sendData("AT+CWSAP?\r\n", 2000, DEBUG); // get SSID info
  //(network name)
  sendData("AT+CIPMUX=1\r\n", 1000, false); // configure for
  //multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, false); // turn on
  //server on port 80

 
  Wire.begin();
  Wire.setClock(400000); //Increase to fast I2C speed!
  mySensor.beginI2C();
  mySensor.setReferencePressure(101200); //Adjust the sea level pressure used for altitude calculations

}
void loop()
{
  if (Serial1.available()) {
    if (Serial1.find("+IPD,")) {
      delay(500);
      int connectionId = Serial1.read() - 48; // read()
      //function returns
      // ASCII decimal value and 0 (the first decimal
      
      String webpage = getWebpageContent();
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
      sendData(cipSend, 100, DEBUG);
      sendData(webpage, 150, DEBUG);
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
      sendData(closeCommand, 300, DEBUG);
    }
  }
}

String getWebpageContent(){
  String str = "<h1>Welcome to the weather station!</h1>";
  str += " <p> Humidity: " + String(mySensor.readFloatHumidity()) + " </p>";
  str += " <p> Pressure: " + String(mySensor.readFloatAltitudeMeters()) + " </p> ";
  str += " <p> Temperature " + String(mySensor.readTempC()) + " </p> ";
  return str;
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial1.print(command); // send command to the esp8266
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      char c = Serial1.read(); // read next char
      response += c;
    }
  }
  if (response.indexOf("/l0") != -1) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (response.indexOf("/l1") != -1) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (debug) {
    Serial.print(response);
  }
  return response;
}

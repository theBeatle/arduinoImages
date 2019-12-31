#include "ThingSpeak.h"
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//BMP280 setup
#define BME280_I2C_ADDR 118 // 0x76 after I2C scanner
Adafruit_BME280 bme; // I2C GPIO 21 & 22

//WiFi setup
char ssid[] = "SS-GUEST"; // your network SSID (name)
char pass[] = "Welcome!"; // your network password
int keyIndex = 0; // your network key Index number (needed only for WEP)
WiFiClient client;

//Thingspeak setup
unsigned long myChannelNumber = 941107; //update
const char * myWriteAPIKey = "KOJF97K1HQ5C1M2X"; //update


void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));

  if (!bme.begin(118)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }

   //
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialize ThingSpeak
}

void loop() {
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    //Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  //read sensor data
  float bmeTemperature = bme.readTemperature();
  float bmePressure = bme.readPressure() / 133.322; // gets in mm HG
  float bmeAltitude = bme.readAltitude(1015.92); // defvalue is 1024, current is for Rivne
  float bmeHumidity = bme.readHumidity();

  // Write to ThingSpeak.
  ThingSpeak.setField(1, bmeTemperature);
  ThingSpeak.setField(2, bmeHumidity);
  ThingSpeak.setField(3, bmePressure);
  ThingSpeak.setField(4, bmeAltitude);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200)
  {
    Serial.println("Channel update successful.");

    Serial.print(F("Temperature = "));
    Serial.print(bmeTemperature);
    Serial.println(" *C");

    Serial.print(F("Humidity = "));
    Serial.print(bmeHumidity);
    Serial.println(" %");

    Serial.print(F("Pressure = "));
    Serial.print(bmePressure  );
    Serial.println(" mm Hg");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmeAltitude); /* Adjusted to local forecast! */
    Serial.println(" m");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  Serial.println();
  delay(60000);
}

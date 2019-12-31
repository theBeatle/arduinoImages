#include "ThingSpeak.h"
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//BMP280 setup
#define BMP280_I2C_ADDR 118 // 0x76 after I2C scanner
Adafruit_BMP280 bmp; // I2C GPIO 21 & 22

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
  Serial.println(F("BMP280 test"));

  if (!bmp.begin(118)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

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
  float bmpTemperature = bmp.readTemperature();
  float bmpPressure = bmp.readPressure() / 133.322; // gets in mm HG
  float bmpAltitude = bmp.readAltitude(1015.92); // defvalue is 1024, current is for Rivne

  // Write to ThingSpeak.
  ThingSpeak.setField(1, bmpPressure);
  ThingSpeak.setField(2, bmpTemperature);
  ThingSpeak.setField(3, bmpAltitude);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200)
  {
    Serial.println("Channel update successful.");

    Serial.print(F("Temperature = "));
    Serial.print(bmpTemperature);
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmpPressure  );
    Serial.println(" mm Hg");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmpAltitude); /* Adjusted to local forecast! */
    Serial.println(" m");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  Serial.println();
  delay(20000);
}

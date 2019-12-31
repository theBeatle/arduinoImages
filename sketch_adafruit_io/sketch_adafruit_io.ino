
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "AdafruitIO_WiFi.h"

//BMP280 setup
#define BME280_I2C_ADDR 118 // 0x76 after I2C scanner
Adafruit_BME280 bme; // I2C GPIO 21 & 22

/******************************* WIFI-BEGIN **************************************/
#define WIFI_SSID   "SS-GUEST"
#define WIFI_PASS   "Welcome!"
/******************************* WIFI-END **************************************/

/************************ Adafruit IO Config *******************************/

// Adafruit IO key.
#define IO_USERNAME  "oleh_kniaz"
#define IO_KEY       "aio_vgLw36m2a4BMCWIQJBT03XuK10Xm"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

AdafruitIO_Feed *temperature = io.feed("weather-station.temperature");
AdafruitIO_Feed *humidity = io.feed("weather-station.humidity");
AdafruitIO_Feed *pressure = io.feed("weather-station.pressure");

/************************ Adafruit IO Config-END *******************************/


void setup() {
  Serial.begin(115200);
  // wait for serial monitor to open
  while(! Serial);
  
  Serial.println(F("BME280 test"));

  if (!bme.begin(BME280_I2C_ADDR)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  //read sensor data
  float bmeTemperature = bme.readTemperature();
  float bmePressure = bme.readPressure() / 133.322; // gets in mm HG
  float bmeAltitude = bme.readAltitude(1015.92); // defvalue is 1024, current is for Rivne
  float bmeHumidity = bme.readHumidity();

  // save Temperature
  Serial.println("sending -> ");
  temperature->save(bmeTemperature);
  humidity->save(bmeHumidity);
  pressure->save(bmePressure);

  
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
   
  Serial.println();
  delay(10000);
}

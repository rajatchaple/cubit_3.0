/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   8
#define MAXCS   9
#define MAXCLK  10

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);


// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);

// Example creating a thermocouple instance with hardware SPI
// on SPI1 using specified CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS, SPI1);

void setup() {
  Serial.begin(9600);

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  // OPTIONAL: Can configure fault checks as desired (default is ALL)
  // Multiple checks can be logically OR'd together.
  // thermocouple.setFaultChecks(MAX31855_FAULT_OPEN | MAX31855_FAULT_SHORT_VCC);  // short to GND fault is ignored

  Serial.println("DONE.");
}

void loop() {
  unsigned long startTime = millis(); // Record the start time
  double c_readings[10]; // Array to store the Celsius readings
  double f_readings[10]; // Array to store the Fahrenheit readings
  
  // Read temperatures
  for (int i = 0; i < 10; i++) {
    c_readings[i] = thermocouple.readCelsius(); // Read temperature in Celsius
    f_readings[i] = thermocouple.readFahrenheit(); // Read temperature in Fahrenheit
    delay(100); // Delay 100 milliseconds between readings
  }

  // Sort the Celsius readings array
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (c_readings[i] > c_readings[j]) {
        double temp = c_readings[i];
        c_readings[i] = c_readings[j];
        c_readings[j] = temp;
      }
    }
  }

  // Sort the Fahrenheit readings array
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (f_readings[i] > f_readings[j]) {
        double temp = f_readings[i];
        f_readings[i] = f_readings[j];
        f_readings[j] = temp;
      }
    }
  }

  // Calculate median Celsius
  double c_median;
  if (10 % 2 == 0) {
    c_median = (c_readings[4] + c_readings[5]) / 2.0;
  } else {
    c_median = c_readings[5];
  }

  // Calculate median Fahrenheit
  double f_median;
  if (10 % 2 == 0) {
    f_median = (f_readings[4] + f_readings[5]) / 2.0;
  } else {
    f_median = f_readings[5];
  }

//  Serial.print("Median Temperature (Celsius) = ");
//  Serial.println(c_median);

  Serial.print("TrunOffMidRef(:");
  Serial.print(130);  //130
  Serial.print(",");
  Serial.print("MidRef:");
  Serial.print(150);  //150
  Serial.print(",");
  Serial.print("MeltTempRef_50sec:");
  Serial.print(175);  //175
  Serial.print(",");
  Serial.print("TurnOffPeakRef:");
  Serial.print(230);  //219
  Serial.print(",");
  Serial.print("PeakRef:");
  Serial.print(245);  //220
  Serial.print(",");
  
  Serial.print("TemperatureC: ");
  Serial.println(c_median);
  
  // Calculate the time elapsed
  unsigned long elapsedTime = millis() - startTime;

  // Delay to ensure total time for serial reading is 1 second
  if (elapsedTime < 1000) {
    delay(1000 - elapsedTime);
  }
  
}

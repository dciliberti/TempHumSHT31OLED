/************************************************************************
   Display temperature and humidity. Use an Adafruit SHT30 sensor to 
   measure ambient humidity and temperature. Display the values on a 
   0.96" OLED screen and through serial communication. Uses a toggle
   switch to show temperature or humidity data on the OLED display,
   including actual value, min and max values, elapsed time.

    Copyright (C) 2023  Danilo Ciliberti 
    danilo.ciliberti@smartup-engineering.com
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>
*************************************************************************/

// Libraries
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Define the SHT sensor object
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Variables
double temp, hum, timeMil, timeSec; // temperature, relative humidity, time

// Variables initialization
double minTemp = 99;  // minimum temperature
double minHum = 99;   // minimum humidity
double maxTemp = -99; // maximum temperature
double maxHum = 0;    // maximum humidity
int togglePin = 5;    // toggle switch digital pin


void setup() {
  pinMode(togglePin, INPUT_PULLUP); //configure pin as an input and enable the internal pull-up resistor
  
  Serial.begin(9600);

  Serial.println("SHT30 self-test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  
  Serial.println("Time\t Hum\t Temp");
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // Show image buffer on the display hardware
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen
  display.display();
  while (millis() % 1000 != 0) {
    // wait until timer is with zero decimal
  }

  // Clear the buffer
  display.clearDisplay();

}

void loop() {
  // Display time passed since the program started
  timeMil = millis();
  timeSec = timeMil/1000;
  Serial.print(timeSec,1);
  Serial.print("s\t");

  // Read and display humidity
  hum = sht31.readHumidity();
  Serial.print(hum,1);
  Serial.print("%\t");

  // Read and display temperature
  temp = sht31.readTemperature();
  Serial.print(temp,1);
  Serial.println("°C");

  // Store minimum and maximum values
  if (temp < minTemp) {
    minTemp = temp;
  }
  if (temp > maxTemp) {
    maxTemp = temp;
  }
  if (hum < minHum) {
    minHum = hum;
  }
  if (hum > maxHum) {
    maxHum = hum;
  }

  // Display values on screen
  display.clearDisplay();       // avoid "noise" on the display due to serial print commands (unknown cause)
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Display humidity or temperature according to toggle switch status
  int toggleStatus = digitalRead(togglePin);
  
  if (toggleStatus == 1){
   display.setCursor(0, 0);
   display.print("Temp.(C)    Min / Max");

   display.setTextSize(3);
   display.setCursor(0, 18);
   display.print(temp,1);

   display.setTextSize(2);
   display.setCursor(80, 15);
   display.print(minTemp,1);
   display.setCursor(80, 30);
   display.print(maxTemp,1); 
  } else {
   display.setCursor(0, 0);
   display.print("Humidity(%) Min / Max");

   display.setTextSize(3);
   display.setCursor(0, 18);
   display.print(hum,1);

   display.setTextSize(2);
   display.setCursor(80, 15);
   display.print(minHum,1);
   display.setCursor(80, 30);
   display.print(maxHum,1);
  }

  // Display elapsed time anyway below
  display.setCursor(0, 50);
  display.print("Time:");
  display.print(timeSec,1);
  display.print("s");

  display.display();
  // Use millis() instead of delay() to keep accurate timing
  while (millis() < timeMil + 1000) {
    // wait a time fraction between the start of the loop and this line
  }
  display.clearDisplay();

}

// PartialUpdateExample : example for Waveshare 1.54", 2.31" and 2.9" e-Paper and the same e-papers from Dalian Good Display Inc.
//
// Created by Jean-Marc Zingg based on demo code from Good Display for GDEP015OC1.
//
// The e-paper displays are available from:
//
// https://www.aliexpress.com/store/product/Wholesale-1-54inch-E-Ink-display-module-with-embedded-controller-200x200-Communicate-via-SPI-interface-Supports/216233_32824535312.html
//
// http://www.buy-lcd.com/index.php?route=product/product&path=2897_8363&product_id=35120
// or https://www.aliexpress.com/store/product/E001-1-54-inch-partial-refresh-Small-size-dot-matrix-e-paper-display/600281_32815089163.html
//

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Dispay ePaper for Arduino : https://forum.arduino.cc/index.php?topic=436411.0

// mapping suggestion from Waveshare 2.9inch e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping suggestion from Waveshare 2.9inch e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// mapping suggestion for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>

#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

//#include GxEPD_BitmapExamples

// generic/common.h
//static const uint8_t SS    = 15; // D8
//static const uint8_t MOSI  = 13; // D7
//static const uint8_t MISO  = 12; // D6
//static const uint8_t SCK   = 14; // D5

GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io, 2, 12 /*RST=D4*/ /*BUSY=D2*/); // default selection of D4(=2), D2(=4)

#if defined(_GxGDEP015OC1_H_)
const uint32_t partial_update_period_s = 1;
const uint32_t full_update_period_s = 6 * 60 * 60;
#elif defined(_GxGDE0213B1_H_) || defined(_GxGDEH029A1_H_) || defined(_GxGDEW042T2_H_)
const uint32_t partial_update_period_s = 2;
const uint32_t full_update_period_s = 1 * 60 * 60;
#endif

#include "BitmapGraphics.h"

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

// heart sensor
/*
  #include <Wire.h>
  #include "MAX30105.h"
  
  #include "heartRate.h"
  
  MAX30105 particleSensor;
  
  const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
  byte rates[RATE_SIZE]; //Array of heart rates
  byte rateSpot = 0;
  long lastBeat = 0; //Time at which the last beat occurred
  
  float beatsPerMinute;
  int beatAvg;
*/

// D0 on Heltec WiFi Kit 
int pinVibro = 16;

//measure speed
double speedSlowest = 18.4;
double speedOwn = 20.5;

int speedSensorPin = A0; 
double circumference = 0.00446860139 / 20;

int startTime = 0;
int endTime = 0;
int lastSample = 0;

int cnt = 0;

  
void setup(void)
{

  pinMode(pinVibro, OUTPUT);
  digitalWrite(pinVibro, LOW); 
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  display.init(115200); // enable diagnostic output on Serial
  //display.init(); // disable diagnostic output on Serial
  Serial.println("setup done");
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(0);
  // draw background


  // cope with code size limitation
  display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1));
  display.setFont(&FreeMonoBold9pt7b);

  // partial update to full screen to preset for partial update of box window
  // (this avoids strange background effects)
  display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1), GxEPD::bm_default | GxEPD::bm_partial_update);
  display.setRotation(1);

  // use GPIO pin 5
  mySwitch.enableReceive(5);  // Receiver on inerrupt 0 => that is pin #2

  // Transmitter is connected to Arduino Pin gpio #3
  mySwitch.enableTransmit(1);

  showPartialUpdateSlowest();
  showPartialUpdateSpeed();
  showPartialUpdateDebug();
  showPartialUpdateSlower();


  /*
   // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  */

  
  
}

void loop()
{
  
  //showPartialUpdate_AVRDebug();
  if (mySwitch.available()) {
    
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );
      Serial.println ( (long) mySwitch.getReceivedRawdata() );

      //speedOwn = ((double) random(150,280)) / 10;
      speedSlowest = ((double) random(110,220)) / 10;

      showPartialUpdateSlowest();
      showPartialUpdateSpeed();
      showPartialUpdateDebug();
      showPartialUpdateSlower();

      mySwitch.send(110, 24);

    }

    
    mySwitch.resetAvailable();
  }

  /*
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
  */

    // read the input on analog pin 0:
  int sensorValue = analogRead(speedSensorPin);

  if (sensorValue >= 1020) {
    if (abs(sensorValue - lastSample) >= 1000) {
      
      if (startTime != 0) {
        Serial.print("LAST: ");
        Serial.print(lastSample);
        Serial.print(", CURRENT: ");
        Serial.println(sensorValue);
        endTime = millis();
        int diff = endTime - startTime;
        Serial.print("DIFF: ");
        Serial.println(diff);
        double hours = (diff * 1.0) / 3600000;
        Serial.print("HOURS: ");
        Serial.println(hours);
        double km = circumference;
        Serial.print("KM: ");
        Serial.println(km);
        double v = km / hours;
        speedOwn = v;
        Serial.print("V: ");
        Serial.println(v);
        startTime = 0;
        endTime = 0;

        showPartialUpdateSpeed();
        showPartialUpdateSlower();
        
      } else {
        startTime = millis();
      }
    }
  }
  if (sensorValue > 55500) {
    if (abs(sensorValue - lastSample) >= 500) {
      cnt++;
      Serial.println(cnt);
    }
  }
  
  lastSample = sensorValue;
  // print out the value you read:
  //Serial.println(sensorValue);
  if (sensorValue >= 0) {
    //Serial.println(sensorValue);
  }
  delay(1);        // delay in between reads for stability

}

void drawCallbackSlowest()
{
  uint16_t box_x = 10;
  uint16_t box_y = 0;
  uint16_t box_w = 190;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print("Slowest: ");
  display.print(String(speedSlowest, 1));
  display.println("km/h");
}

void drawCallbackSpeed()
{
  uint16_t box_x = 80;
  uint16_t box_y = 110;
  uint16_t box_w = 110;
  uint16_t box_h = 70;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print("Speed: \n");
  display.setCursor(box_x, cursor_y +22);
  display.setFont(&FreeMonoBold12pt7b);
  
  display.print(String(speedOwn, 1));
  display.print("km/h \n");
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(box_x, cursor_y +47);
  //display.print("3 Members");

}

void drawCallbackSlower()
{
  uint16_t box_x = 80;
  uint16_t box_y = 60;
  uint16_t box_w = 110;
  uint16_t box_h = 40;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);

  Serial.print("speedSlowest: ");
  Serial.println(speedSlowest);
  Serial.print("speedOwn: ");
  Serial.println(speedOwn);

  if((speedSlowest + 1) < speedOwn){
    display.print("! Slower !\n");

    // turn on vibro
    digitalWrite(pinVibro, HIGH);
    delay(50);
    digitalWrite(pinVibro, LOW); 
    delay(50); 
    digitalWrite(pinVibro, HIGH);    
    delay(80);
    digitalWrite(pinVibro, LOW); 
      
  }else{
    display.print("\n");
   }
  
}

void drawCallbackDebug()
{
  uint16_t box_x = 80;
  uint16_t box_y = 180;
  uint16_t box_w = 110;
  uint16_t box_h = 15;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print(mySwitch.getReceivedValue());
}

void showPartialUpdateSlowest()
{
  uint16_t box_x = 10;
  uint16_t box_y = 0;
  uint16_t box_w = 190;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallbackSlowest, box_x, box_y, box_w, box_h);
}

void showPartialUpdateSpeed()
{
  uint16_t box_x = 80;
  uint16_t box_y = 110;
  uint16_t box_w = 110;
  uint16_t box_h = 70;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallbackSpeed, box_x, box_y, box_w, box_h);
}

void showPartialUpdateSlower()
{
  uint16_t box_x = 80;
  uint16_t box_y = 60;
  uint16_t box_w = 110;
  uint16_t box_h = 40;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallbackSlower, box_x, box_y, box_w, box_h);
}

void showPartialUpdateDebug()
{
  uint16_t box_x = 80;
  uint16_t box_y = 180;
  uint16_t box_w = 110;
  uint16_t box_h = 15;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallbackDebug, box_x, box_y, box_w, box_h);
}



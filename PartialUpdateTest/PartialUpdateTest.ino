// PartialUpdateTest : example for Waveshare 1.54", 2.31" and 2.9" e-Paper and the same e-papers from Dalian Good Display Inc.
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

// mapping from Waveshare 2.9inch e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping example for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, C S-> 10, CLK -> 13, DIN -> 11

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

// pins_arduino.h, e.g. AVR
//#define PIN_SPI_SS    (10)
//#define PIN_SPI_MOSI  (11)
//#define PIN_SPI_MISO  (12)
//#define PIN_SPI_SCK   (13)

GxIO_Class io(SPI, SS, 8, 9);
GxEPD_Class display(io);


#if defined(_GxGDEP015OC1_H_)
const uint32_t partial_update_period_s = 1;
const uint32_t full_update_period_s = 6 * 60 * 60;
#elif defined(_GxGDE0213B1_H_) || defined(_GxGDEH029A1_H_) || defined(_GxGDEW042T2_H_)
const uint32_t partial_update_period_s = 2;
const uint32_t full_update_period_s = 1 * 60 * 60;
#endif

uint32_t start_time;
uint32_t next_time;
uint32_t previous_time;
uint32_t previous_full_update;

uint32_t total_seconds = 0;
uint32_t seconds, minutes, hours, days;

#include "BitmapGraphics.h"

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  //display.init(115200); // enable diagnostic output on Serial
  display.init(); // disable diagnostic output on Serial
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
  start_time = next_time = previous_time = previous_full_update = millis();
  display.setRotation(1);
}

void loop()
{
  uint32_t actual = millis();
  while (actual < next_time)
  {
    // the "BlinkWithoutDelay" method works also for overflowed millis
    if ((actual - previous_time) > (partial_update_period_s * 1000))
    {
      //Serial.print(actual - previous_time); Serial.print(" > "); Serial.println(partial_update_period_s * 1000);
      break;
    }
    delay(100);
    actual = millis();
  }
  //Serial.print("actual: "); Serial.print(actual); Serial.print(" previous: "); Serial.println(previous_time);
  if ((actual - previous_full_update) > full_update_period_s * 1000)
  {

    display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1));
    display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1), GxEPD::bm_default | GxEPD::bm_partial_update);

    previous_full_update = actual;
  }
  previous_time = actual;
  next_time += uint32_t(partial_update_period_s * 1000);
  total_seconds += partial_update_period_s;
  seconds = total_seconds % 60;
  minutes = (total_seconds / 60) % 60;
  hours = (total_seconds / 3600) % 24;
  days = (total_seconds / 3600) / 24;

  showPartialUpdate_AVR();
  showPartialUpdate_AVR2();
  showPartialUpdate_AVR3();
  showPartialUpdate_AVRDebug();

}

void print02d(uint32_t d)
{
  if (d < 10) display.print("0");
  display.print(d);
}

void drawCallback()
{
  uint16_t box_x = 10;
  uint16_t box_y = 0;
  uint16_t box_w = 190;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  //display.print("7"); display.print("d "); print02d(hours); display.print(":"); print02d(minutes); display.print(":"); print02d(seconds);
  display.print("Slowest: ");
  print02d(seconds);
  display.print(" km/h");
}

void drawCallback2()
{
  uint16_t box_x = 80;
  uint16_t box_y = 110;
  uint16_t box_w = 110;
  uint16_t box_h = 75;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print("Speed: \n");
  display.setCursor(box_x, cursor_y +22);
  display.setFont(&FreeMonoBold12pt7b);
  display.print("24 km/h \n");
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(box_x, cursor_y +47);
  display.print("3 Members");

}

void drawCallback3()
{
  uint16_t box_x = 80;
  uint16_t box_y = 60;
  uint16_t box_w = 110;
  uint16_t box_h = 40;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print("! Slower !\n");
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
  display.print("Rec: 45L");
}

void showPartialUpdate_AVR()
{
  uint16_t box_x = 10;
  uint16_t box_y = 0;
  uint16_t box_w = 190;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallback, box_x, box_y, box_w, box_h);
}

void showPartialUpdate_AVR2()
{
  uint16_t box_x = 80;
  uint16_t box_y = 110;
  uint16_t box_w = 110;
  uint16_t box_h = 75;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallback2, box_x, box_y, box_w, box_h);
}

void showPartialUpdate_AVR3()
{
  uint16_t box_x = 80;
  uint16_t box_y = 60;
  uint16_t box_w = 110;
  uint16_t box_h = 40;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallback3, box_x, box_y, box_w, box_h);
}

void showPartialUpdate_AVRDebug()
{
  uint16_t box_x = 80;
  uint16_t box_y = 180;
  uint16_t box_w = 110;
  uint16_t box_h = 15;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallbackDebug, box_x, box_y, box_w, box_h);
}






// GxEPD_SPI_TestExample : test example for e-Paper displays from Waveshare and from Dalian Good Display Inc.
//
// Created by Jean-Marc Zingg based on demo code from Good Display,
// available on http://www.good-display.com/download_list/downloadcategoryid=34&isMode=false.html
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

// mapping suggestion from Waveshare SPI e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

// mapping suggestion for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>    // 1.54" b/w
//#include <GxGDEW0154Z04/GxGDEW0154Z04.cpp>  // 1.54" b/w/r 200x200
//#include <GxGDEW0154Z17/GxGDEW0154Z17.cpp>  // 1.54" b/w/r 152x152
//#include <GxGDE0213B1/GxGDE0213B1.cpp>      // 2.13" b/w
//#include <GxGDEW0213Z16/GxGDEW0213Z16.cpp>  // 2.13" b/w/r
//#include <GxGDEH029A1/GxGDEH029A1.cpp>      // 2.9" b/w
//#include <GxGDEW029Z10/GxGDEW029Z10.cpp>    // 2.9" b/w/r
//#include <GxGDEW027C44/GxGDEW027C44.cpp>    // 2.7" b/w/r
//#include <GxGDEW027W3/GxGDEW027W3.cpp>      // 2.7" b/w
//#include <GxGDEW042T2/GxGDEW042T2.cpp>      // 4.2" b/w
//#include <GxGDEW042Z15/GxGDEW042Z15.cpp>    // 4.2" b/w/r
//#include <GxGDEW0583T7/GxGDEW0583T7.cpp>    // 5.83" b/w
//#include <GxGDEW075T8/GxGDEW075T8.cpp>      // 7.5" b/w
//#include <GxGDEW075Z09/GxGDEW075Z09.cpp>    // 7.5" b/w/r

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

#if defined(ESP8266)

// generic/common.h
//static const uint8_t SS    = 15; // D8
//static const uint8_t MOSI  = 13; // D7
//static const uint8_t MISO  = 12; // D6
//static const uint8_t SCK   = 14; // D5

GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io /*RST=D4*/ /*BUSY=D2*/); // default selection of D4(=2), D2(=4)
// Heltec E-Paper 1.54" b/w without BUSY
//GxEPD_Class display(io, /*RST=D4*/ 2, /*BUSY=D2*/ -1); // default selection of D4(=2), no BUSY

#elif defined(ESP32)

// pins_arduino.h, e.g. LOLIN32
//static const uint8_t SS    = 5;
//static const uint8_t MOSI  = 23;
//static const uint8_t MISO  = 19;
//static const uint8_t SCK   = 18;

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

#elif defined(ARDUINO_ARCH_SAMD)

// variant.h of MKR1000
//#define PIN_SPI_MISO  (10u)
//#define PIN_SPI_MOSI  (8u)
//#define PIN_SPI_SCK   (9u)
//#define PIN_SPI_SS    (24u) // should be 4?
// variant.h of MKRZERO
//#define PIN_SPI_MISO  (10u)
//#define PIN_SPI_MOSI  (8u)
//#define PIN_SPI_SCK   (9u)
//#define PIN_SPI_SS    (4u)

GxIO_Class io(SPI, /*CS=*/ 4, /*DC=*/ 7, /*RST=*/ 6);
GxEPD_Class display(io, /*RST=*/ 6, /*BUSY=*/ 5);

#elif defined(ARDUINO_GENERIC_STM32F103C) && defined(MCU_STM32F103C8)

// STM32 Boards(STM32duino.com) Generic STM32F103C series STM32F103C8
// aka BluePill
// board.h
//#define BOARD_SPI1_NSS_PIN        PA4
//#define BOARD_SPI1_MOSI_PIN       PA7
//#define BOARD_SPI1_MISO_PIN       PA6
//#define BOARD_SPI1_SCK_PIN        PA5
//enum {
//    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13,PA14,PA15,
//  PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13,PB14,PB15,
//  PC13, PC14,PC15
//};
// variant.h
//static const uint8_t SS   = BOARD_SPI1_NSS_PIN;
//static const uint8_t SS1  = BOARD_SPI2_NSS_PIN;
//static const uint8_t MOSI = BOARD_SPI1_MOSI_PIN;
//static const uint8_t MISO = BOARD_SPI1_MISO_PIN;
//static const uint8_t SCK  = BOARD_SPI1_SCK_PIN;

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 3, /*RST=*/ 2);
GxEPD_Class display(io, /*RST=*/ 2, /*BUSY=*/ 1);

#elif defined(ARDUINO_GENERIC_STM32F103V) && defined(MCU_STM32F103VB)

// board.h
//#define BOARD_SPI1_NSS_PIN        PA4
//#define BOARD_SPI1_MOSI_PIN       PA7
//#define BOARD_SPI1_MISO_PIN       PA6
//#define BOARD_SPI1_SCK_PIN        PA5

// STM32 Boards(STM32duino.com) Generic STM32F103V series STM32F103VB
// Good Display DESPI-M01
// note: needs jumper wires from SS=PA4->CS, SCK=PA5->SCK, MOSI=PA7->SDI

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ PE15, /*RST=*/ PE14); // DC, RST as wired by DESPI-M01
GxEPD_Class display(io, /*RST=*/ PE14, /*BUSY=*/ PE13); // RST, BUSY as wired by DESPI-M01

#else

// pins_arduino.h, e.g. AVR
//#define PIN_SPI_SS    (10)
//#define PIN_SPI_MOSI  (11)
//#define PIN_SPI_MISO  (12)
//#define PIN_SPI_SCK   (13)

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9); // arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io /*RST=9*/ /*BUSY=7*/); // default selection of (9), 7

#endif

// 433 Mhz Receiver
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

uint32_t start_time;
uint32_t next_time;
uint32_t previous_time;
uint32_t previous_full_update;

uint32_t total_seconds = 0;
uint32_t seconds, minutes, hours, days;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  display.init(115200); // enable diagnostic output on Serial

  //433 Mhz Rec
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2

  Serial.println("setup done");
}

void loop()
{
  //showBitmapExample();
  showPartialUpdate();
  if (mySwitch.available()) {
    
    //display.drawPaged(showPartialUpdate);

    //mySwitch.resetAvailable();
  }

  /*
#if !defined(__AVR)
  drawCornerTest();
  showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
  showFont("FreeMonoBold12pt7b", &FreeMonoBold12pt7b);
  //showFont("FreeMonoBold18pt7b", &FreeMonoBold18pt7b);
  //showFont("FreeMonoBold24pt7b", &FreeMonoBold24pt7b);
#else
  display.drawCornerTest();
  delay(2000);
  display.drawPaged(showFontCallback);
#endif
  delay(10000);
  */
}

#if defined(_GxGDEP015OC1_H_)
void showBitmapExample()
{
  display.drawExampleBitmap(BitmapExample1, sizeof(BitmapExample1));
  delay(2000);
  display.drawExampleBitmap(BitmapExample2, sizeof(BitmapExample2));
  delay(5000);
  display.fillScreen(GxEPD_WHITE);
  display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
  display.update();
  delay(5000);
  showBoat();
}
#endif





void showFont(const char name[], const GFXfont* f)
{
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
  display.setTextColor(GxEPD_RED);
#endif
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
  display.update();
  delay(5000);
}

void showFontCallback()
{
  const char* name = "FreeMonoBold9pt7b";
  const GFXfont* f = &FreeMonoBold9pt7b;
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0, 0);
  display.println();
  display.println(name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
#if defined(HAS_RED_COLOR)
  display.setTextColor(GxEPD_RED);
#endif
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
}

void showTest()
{

   const char* name = "FreeMonoBold9pt7b";
    const GFXfont* f = &FreeMonoBold9pt7b;
    //display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(f);
    display.setCursor(0, 10);
    display.println();
    //display.println(name);
    display.println("On 433 Mhz: ");
    
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
      display.println("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );

      display.print("Received ");
      display.print( mySwitch.getReceivedValue() );
      display.print(" / ");
      display.print( mySwitch.getReceivedBitlength() );
      display.print("bit ");
      display.print("Protocol: ");
      display.println( mySwitch.getReceivedProtocol() );

      display.println("");
      display.println("");
      display.println("Physical");
      display.println("Computing");
      display.println("Lab SoSe2018");
    }

  
}

void print02d(uint32_t d)
{
  if (d < 10) display.print("0");
  display.print(d);
}


void drawCallback()
{
  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 170;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 16;
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print(days); display.print("d "); print02d(hours); display.print(":"); print02d(minutes); display.print(":"); print02d(seconds);
}

void showPartialUpdate()
{
  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 170;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + 14;
  display.drawPagedToWindow(drawCallback, box_x, box_y, box_w, box_h);
}

void drawCornerTest()
{
  display.drawCornerTest();
  delay(5000);
  uint8_t rotation = display.getRotation();
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
    display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
    display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
    display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
    display.update();
    delay(5000);
  }
  display.setRotation(rotation); // restore
}

#if defined(_GxGDEP015OC1_H_) || defined(_GxGDE0213B1_H_) || defined(_GxGDEH029A1_H_)
#include "IMG_0001.h"
void showBoat()
{
  // thanks to bytecrusher: http://forum.arduino.cc/index.php?topic=487007.msg3367378#msg3367378
  uint16_t x = (display.width() - 64) / 2;
  uint16_t y = 5;
  display.fillScreen(GxEPD_WHITE);
  display.drawExampleBitmap(gImage_IMG_0001, x, y, 64, 180, GxEPD_BLACK);
  display.update();
  delay(500);
  uint16_t forward = GxEPD::bm_invert | GxEPD::bm_flip_x;
  uint16_t reverse = GxEPD::bm_invert | GxEPD::bm_flip_x | GxEPD::bm_flip_y;
  for (; y + 180 + 5 <= display.height(); y += 5)
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawExampleBitmap(gImage_IMG_0001, x, y, 64, 180, GxEPD_BLACK, forward);
    display.updateWindow(0, 0, display.width(), display.height());
    delay(500);
  }
  delay(1000);
  for (; y >= 5; y -= 5)
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawExampleBitmap(gImage_IMG_0001, x, y, 64, 180, GxEPD_BLACK, reverse);
    display.updateWindow(0, 0, display.width(), display.height());
    delay(1000);
  }
  display.update();
  delay(1000);
}
#endif



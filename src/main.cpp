#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#include "global.h"



#define BOUTON     D2

int lastState = HIGH;
long unsigned int nb = 0;
int dejaApp = 0;
bool endCal = false;

vector<Vevent> globalVevents;

unsigned long last500 = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Test TFT ST7735");

  pinMode(BOUTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BOUTON),frontDetectM, FALLING);

  initScreen();
  initWifi();

  tft.fillScreen(ST7735_BLACK);
  
  updateCalendar();
  
  tft.fillScreen(ST7735_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

  // tft.setCursor(0,9);
  // for(int i = 0; i <globalVevents.size(); i++){
  //   tft.println(globalVevents[i].name);
  // }
}

void loop() {
  if(millis() - last500 >= 500) {
    last500 = millis();
    header();
    // tft.fillRect(0, 0, 36, 24, ST7735_BLACK);
    // displayVevents(globalVevents);
    // delay(1000);
  }
  displayVevents(globalVevents);
  if(risingEdge()) {
    updateCalendar();
  }
}
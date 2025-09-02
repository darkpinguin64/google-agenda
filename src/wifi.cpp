#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "global.h"


#define ssid "Freebox_job"
#define password "abcd012345"

WiFiServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200);

struct tm *ptm;
// struct tm globalTime;
unsigned long lastUpdate = 0;
time_t lastEpoch;

void updateDate(bool forceUpdate) {
  
  if(WiFi.isConnected() && (forceUpdate || millis() - lastUpdate <= 60000)) {
    timeClient.update();
    
    time_t epochTime = timeClient.getEpochTime(); // timestamp UTC
    ptm = gmtime(&epochTime);
    lastEpoch = epochTime;
    lastUpdate = millis();
    // globaTime = *ptm;
  } else {
    time_t newEpoch = lastEpoch + millis()/1000 - lastUpdate/1000;
    ptm = gmtime(&newEpoch);
  }
}

void initWifi() {
  tft.fillScreen(ST77XX_BLACK);
  tft.println("connecting to");
  Serial.println("connecting to");
  WiFi.begin(ssid, password);
  tft.println(WiFi.SSID());
  tft.print("\n[");
  tft.setTextColor(ST77XX_GREEN);
  int i = 0;
  while(!WiFi.isConnected()) {
    i++;
    tft.write("#");
    delay(500);
  }
  tft.setTextColor(ST77XX_WHITE, ST7735_BLACK);
  tft.println("]\n");
  tft.print("connected in ");
  tft.print( i*0.5);
  tft.println("seconds");
  server.begin();
  tft.println("server started");
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  // tft.println("ip address :");
  // tft.print("http://");
  // tft.print(WiFi.localIP());
  // tft.println("/");

  timeClient.begin();
  timeClient.update();
  updateDate();
  
  // while(!risingEdge()) {
  //   yield();
  // }
}
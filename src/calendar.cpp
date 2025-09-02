#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
#include <set>
#include "global.h"

#define ical "https://calendar.google.com/calendar/ical/marcos.jobard.vargas%40gmail.com/private-b2278afb9e51d3d9a4273a6e57569f0b/basic.ics"

using namespace std;

// struct tm *ptm;



void updateCalendar(bool discret) {
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();
  http.begin(client, ical);
  int httpCode = http.GET();
  
  // int indexVevent = 0;
  int cursorY = 0;
  // bool school = false; //deja un evenement ecole
  // bool entreprise = false; //deja un evenement entreprise
  
  vector<Vevent> vevents;
  vector<Vevent> uniqueVevents;
  set<String> seenVevents;
  
  updateDate(true);
  
  Vevent vevent;
  
  if(httpCode == HTTP_CODE_OK) {
    WiFiClient* stream = http.getStreamPtr();
    String line;
    // time_t epoch = 0;
    
    if(!discret) tft.fillScreen(ST7735_BLACK);

    while(http.connected() && !line.startsWith("END:VCALENDAR")) {
      line = stream->readStringUntil('\n');
      line.trim();
      // epoch = 0;

      if(line.startsWith("BEGIN:VEVENT")) {
        vevent = Vevent();
        vevent.name = "noName";
        // vevent.start.tm_year = 0;
        // vevent.start.tm_mon = 0;
        // vevent.start.tm_mday = 0;
        // vevent.start.tm_hour = 0;
        // vevent.start.tm_min = 0;
        // vevent.end.tm_year = 0;
        // vevent.end.tm_mon = 0;
        // vevent.end.tm_mday = 0;
        // vevent.end.tm_hour = 0;
        // vevent.end.tm_min = 0;
        // vevent.end.tm_sec = 0;
        
        // indexVevent++;
        // tft.setTextColor(ST7735_BLACK, ST7735_YELLOW);
        // tft.setCursor(128 - String(indexVevent).length() * 6 - 1, 144);
        // tft.print(indexVevent);
      }
      if(line.startsWith("DTSTART;")) { //DTSTART;VALUE=DATE:YYYYMMDD
        vevent.start.tm_year = line.substring(19, 23).toInt() - 1900;
        vevent.start.tm_mon = line.substring(23, 25).toInt() - 1;
        vevent.start.tm_mday = line.substring(25, 27).toInt();
        vevent.start.tm_hour = 0;
        vevent.start.tm_min = 0;
        vevent.start.tm_sec = 0;
        // Serial.printf("-----\nstart %02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
      } else if(line.startsWith("DTSTART:")) { //DTSTART:YYYYMMDDTHHMMSSZ
        vevent.start.tm_year = line.substring(8,12).toInt() - 1900;
        vevent.start.tm_mon = line.substring(12,14).toInt() - 1;
        vevent.start.tm_mday = line.substring(14,16).toInt();
        vevent.start.tm_hour = line.substring(17,19).toInt() + 2;
        vevent.start.tm_min = line.substring(19,21).toInt();
        vevent.start.tm_sec = line.substring(21,23).toInt();
        
        // Serial.printf("-----\nstart av %02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
        // epoch = mktime(&vevent.start);
        // epoch += 7200;
        // vevent.start = *gmtime(&epoch);
        // Serial.printf("start apres %02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
      }
      if(line.startsWith("RRULE:")) {
        int i = line.indexOf("FREQ=") + 5;
        int y = line.indexOf(";");
        String freq = line.substring(i, y);
        if(freq == "YEARLY") { //  && line.indexOf("UNTIL") == - 1
          vevent.start.tm_year = ptm->tm_year;
          vevent.end.tm_year = ptm->tm_year;
        }
      }
      if(line.startsWith("DTEND;")) { //DTEND;VALUE=DATE:YYYYMMDD
        vevent.end.tm_year = line.substring(17, 21).toInt() - 1900;
        vevent.end.tm_mon = line.substring(21, 23).toInt() - 1;
        vevent.end.tm_mday = line.substring(23, 25).toInt();
        vevent.end.tm_hour = 0;
        vevent.end.tm_min = 0;
        vevent.end.tm_sec = 0;
        // Serial.printf("end %02d/%02d/%02d %02d:%02d:%02d\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
      } else if(line.startsWith("DTEND:")) { //DTEND:YYYYMMDDTHHMMSSZ
        vevent.end.tm_year = line.substring(6,10).toInt() - 1900;
        vevent.end.tm_mon = line.substring(10,12).toInt() - 1;
        vevent.end.tm_mday = line.substring(12,14).toInt();
        vevent.end.tm_hour = line.substring(15,17).toInt() + 2;
        vevent.end.tm_min = line.substring(17,19).toInt();
        vevent.end.tm_sec = line.substring(19,21).toInt();

        // Serial.printf("end av %02d/%02d/%02d %02d:%02d:%02d\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
        // epoch = mktime(&vevent.end);
        // epoch += 7200;
        // vevent.end = *gmtime(&epoch);
        // Serial.printf("end apres %02d/%02d/%02d %02d:%02d:%02d\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
      }
      if(line.startsWith("SUMMARY:")) {
        tft.setTextColor(ST7735_WHITE);
        String name = line.substring(8);

        // if(name == "Yup") Serial.println("yep");
        // Serial.println(name + "\n-----\n");

        if(name.length() > 15) {
          name = name.substring(0,15);
        } else {
          // name += '/'*(14-name.length());
        }

        vevent.name = name;
      }
      if(line.startsWith("END:VEVENT") && mktime(&vevent.end) > mktime(ptm)) {
        // if(vevent.name == "Vacances à Argences") {
        //   Serial.printf("%02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
        //   Serial.printf("%02d/%02d/%02d %02d:%02d:%02d\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
        // }
        // Serial.printf("-----\nstart %02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
        // Serial.printf("ptm %02d/%02d/%02d %02d:%02d:%02d", ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        // Serial.println(mktime(&vevent.end) > mktime(ptm));
        // Serial.println(vevent.name);
        // Serial.printf("end %02d/%02d/%02d %02d:%02d:%02d\n-----\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
        if(!(seenVevents.find(vevent.name) != seenVevents.end() && (vevent.name == "Ecole" || vevent.name == "Entreprise"))) {
          // uniqueVevents.push_back(v);
          seenVevents.insert(vevent.name);
          vevents.push_back(vevent);
        }

        if(!discret) {
          tft.setTextColor(ST7735_BLACK, ST7735_CYAN);
          tft.setCursor(128 - String(vevents.size()).length() * 6, 0);
          tft.print(vevents.size());
          tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
          tft.setTextWrap(false);
          tft.setCursor(0, cursorY);
          // tft.fillRect(0, cursorY, 128, 8, ST7735_ORANGE);
          tft.print(vevent.name);
          cursorY += 8;
          if(tft.getCursorY() > 153) {
            tft.setCursor(0,0);
            cursorY = 0;
          };
        } else {
          // tft.fillRect(0,0,128,10,ST7735_BLACK);
          // tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
          // tft.setCursor(128 - String(vevents.size()).length() * 6, 1);
          // tft.print(vevents.size());
          // tft.setCursor(1,1);
          // tft.print(vevent.name);
        }
      }
    }
  }
  
  Serial.println("l171");
  // for(Vevent v : vevents) {
    //   Serial.println(v.name);
    //   Serial.printf("start %02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
    //   Serial.printf("end %02d/%02d/%02d %02d:%02d:%02d\n\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
    // }
    sort(vevents.begin(), vevents.end(), [](const Vevent &a, const Vevent &b){
      return mktime(const_cast<tm*>(&a.start)) < mktime(const_cast<tm*>(&b.start));
    });
    
    
    Serial.println("l182");
    // for(Vevent v : vevents) {
    //   if(!(seenVevents.find(v.name) != seenVevents.end() && (v.name == "Ecole" || v.name == "Entreprise"))) {
    //     uniqueVevents.push_back(v);
    //     seenVevents.insert(v.name);
    //   }
    // }
    
    
    Serial.println("l191");
    String removeType;
    for(Vevent v : vevents) {
      if(v.name == "Ecole") {
        removeType = "Entreprise";
        break;
      }
      if(v.name == "Entreprise") {
        removeType = "Ecole";
        break;
      }
    }
    
    // vector<Vevent>().swap(vevents);
    
    Serial.println("l206");
  vector<Vevent> filtered;
  copy_if(vevents.begin(), vevents.end(), back_inserter(filtered), [&](const Vevent v){
    return v.name != removeType;
  });
  
  // vector<Vevent>().swap(uniqueVevents);
  // uniqueVevents.clear();
  
  
  // Serial.println("------------filtered-------------");
  // for(Vevent v : filtered) {
  //   Serial.println(v.name);
  //   Serial.printf("start %02d/%02d/%02d %02d:%02d:%02d\n", vevent.start.tm_mday, vevent.start.tm_mon + 1, vevent.start.tm_year + 1900, vevent.start.tm_hour, vevent.start.tm_min, vevent.start.tm_sec);
  //   Serial.printf("end %02d/%02d/%02d %02d:%02d:%02d\n\n", vevent.end.tm_mday, vevent.end.tm_mon + 1, vevent.end.tm_year + 1900, vevent.end.tm_hour, vevent.end.tm_min, vevent.end.tm_sec);
  // }
  
  if(!discret) {
    tft.fillScreen(ST7735_BLACK);
  }
  initHeader = false;
  initVevents = false;

  globalVevents = filtered;
}
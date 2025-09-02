
#include <string>
#include "global.h"
using namespace std;

#define TFT_CS     D8
#define TFT_RST    D4
#define TFT_DC     D3


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

bool initVevents = false;
bool initHeader = false;


void initScreen() {
  tft.initR(INITR_BLACKTAB);
  tft.setTextColor(ST77XX_WHITE);
  tft.invertDisplay(false);
  // tft.setRotation(2);
}

void header() {
  if(!initHeader) {
    tft.fillRect(0,0,128,10,ST7735_BLACK);
    initHeader = true;
  }
  
  String format = "yyyy/mm/jj hh:mm:ss";
  tft.setCursor(64-format.length()*6/2, 2);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.printf("%02d/%02d/%02d %02d:%02d:%02d", ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  updateDate();
}

void resetTxt() {
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
}

struct veventInfo {
  time_t addedEpoch;
  int lastFill;
  unsigned int lastRemainDays;
};

unordered_map<std::string,veventInfo> dateInit;
void displayVevents(vector<Vevent> vevents) {
  
  const int nbEvent = 8;
  // const int nbCaractere = 14;
  
  int lim = (vevents.size() < nbEvent) ? vevents.size() : nbEvent;
  bool eventEnded = false;
  
  for(int i = 0; i < lim; i++) {
    Vevent v = vevents[i];
    veventInfo info;
    int heightBox = 150/nbEvent;
    int diff = 150-heightBox*nbEvent;
    int cursorY = 10 + i*heightBox + diff/2;
    
    
    bool enCours = mktime(&v.start) < mktime(ptm);
    unsigned long remainTime;
    
    if(!enCours) {
      remainTime = mktime(&v.start) - mktime(ptm);
    } else {
      remainTime = mktime(&v.end) - mktime(ptm);
    }
    unsigned int remainTimeDay = remainTime/86400+1;
    
    if(dateInit.find(std::string(v.name.c_str())) == dateInit.end()) {
      info.addedEpoch = mktime(ptm);
      info.lastFill = 0;
      info.lastRemainDays = 0;
      dateInit.insert({std::string(v.name.c_str()), info});
    }
    info = dateInit[std::string(v.name.c_str())];
    
    unsigned long deltaTime;
    unsigned long progress;
    int fill;
    
    if(enCours) {
      deltaTime = mktime(&v.end) - mktime(&v.start);
      progress = mktime(ptm) - mktime(&v.start);
      fill = (progress * 87) / deltaTime;
    } else {
      deltaTime = mktime(&v.start) - info.addedEpoch;
      progress = mktime(ptm) - info.addedEpoch;
      fill = (progress * 89) / deltaTime;
    }
    
    if(!initVevents) {
      // tft.fillRoundRect(6, cursorY + (heightBox-12)/2, 89, 12, 2, ST7735_BLACK);
      tft.fillRect(0, cursorY, 128, heightBox, ST7735_BLACK);
      // tft.drawRect(0, cursorY, 128, heightBox, ST7735_WHITE);
      // tft.drawRoundRect(5, cursorY + (heightBox-14)/2, 91, 14, 3, ST7735_WHITE);
      if(enCours) tft.fillRect(6, cursorY + (heightBox-4)/2 + 4, 89, 4, ST7735_MAGENTA);
      tft.drawRoundRect(5, cursorY + (heightBox-6)/2 + 4, 91, 6, 2, ST7735_WHITE);
      // tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
      // tft.setCursor(7, cursorY + (heightBox - 7)/2);
      // tft.print(v.name);
    }
    bool asChange = fill != info.lastFill || remainTimeDay != info.lastRemainDays;
    if(asChange || !initVevents) {
      tft.setTextSize(2);
      tft.setCursor(101, cursorY + (heightBox-14)/2);
      tft.setTextColor(enCours? ST7735_CYAN : ST7735_YELLOW, ST7735_BLACK);
      tft.printf("%02d", remainTimeDay);
      resetTxt();
      
      // tft.fillRoundRect(6, cursorY + (heightBox-12)/2, fill, 12, 2, enCours? ST7735_MAGENTA : ST7735_BLUE);
      // tft.fillRect(6, cursorY + (heightBox-4)/2 + 4, fill, 4, enCours? ST7735_WHITE : ST7735_CYAN);
      if(enCours) {
        tft.fillRect(7, cursorY + (heightBox-2)/2 + 4, fill, 2, ST7735_WHITE);
      } else {
        tft.fillRect(6, cursorY + (heightBox-4)/2 + 4, fill, 4, ST7735_MAGENTA);
      }
      // if(fill < 4) tft.drawRoundRect(5, cursorY + (heightBox-14)/2, 91, 14, 3, ST7735_WHITE);
      
      // tft.setCursor(7, cursorY + (heightBox - 7)/2);
      tft.setCursor(5, cursorY + (heightBox-14)/2);
      tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
      tft.print(v.name);
      
      info.lastRemainDays = remainTimeDay;
      info.lastFill = fill;
      dateInit[std::string(v.name.c_str())] = info;
    }
    
    if(progress/deltaTime >= 1 && enCours) {
      eventEnded = true;
      dateInit.erase(std::string(v.name.c_str()));
      break;
    }
  }

  if(!initVevents) initVevents = true;
  if(eventEnded) {
    updateCalendar(true);
  }
};
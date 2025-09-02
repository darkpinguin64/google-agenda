#pragma once

#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <time.h>

using namespace std;

extern Adafruit_ST7735 tft;

struct Vevent {
  String name;
  struct tm start;
  struct tm end;
};

void initScreen();
void initWifi();
void frontDetectM();
bool risingEdge();
void updateCalendar(bool discret = false);
void updateDate(bool forceUpdate = false);
extern struct tm *ptm;
void displayVevents(vector<Vevent> vevent);
void header();

extern bool initVevents;
extern bool initHeader;

extern vector<Vevent> globalVevents;
// void risingEdge(std::function<void()> f);
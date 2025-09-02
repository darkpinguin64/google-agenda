#include <Arduino.h>
using namespace std;

volatile bool frontDetect = false;

void IRAM_ATTR frontDetectM() {
  frontDetect = true;
}

bool risingEdge() {
  if(frontDetect) {
    frontDetect = false;
    return true;
  }
  return false;
}
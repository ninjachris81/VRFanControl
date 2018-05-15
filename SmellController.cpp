#include "SmellController.h"

#include <LogHelper.h>

SmellController::SmellController() : AbstractTriggerTask() {
  
}

void SmellController::init() {
  pinMode(PIN_SMELL, OUTPUT);
  digitalWrite(PIN_SMELL, HIGH);

  // turn on
  /*
  for (uint8_t i=0;i<5;i++) {
    digitalWrite(PIN_SMELL, HIGH);
    delay(200);
    digitalWrite(PIN_SMELL, LOW);
    delay(200);
  }*/
}

void SmellController::update() {
  LOG_PRINTLN(F("Resetting smell"));
  digitalWrite(PIN_SMELL, HIGH);
}

void SmellController::releaseSmell(uint8_t intensity) {
  if (intensity<=0) return;
  
  LOG_PRINT(F("Setting smell for "));
  LOG_PRINTLN(intensity);
  
  digitalWrite(PIN_SMELL, LOW);
  triggerUpdateDelay(intensity * TIME_FACTOR);
}


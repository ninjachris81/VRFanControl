#include "SmellController.h"

#include <LogHelper.h>

SmellController::SmellController() : AbstractTriggerTask() {
  
}

void SmellController::init() {
  pinMode(PIN_SMELL, OUTPUT);
  digitalWrite(PIN_SMELL, LOW);
}

void SmellController::update() {
  LOG_PRINTLN(F("Resetting smell"));
  digitalWrite(PIN_SMELL, LOW);
}

void SmellController::releaseSmell(uint8_t intensity) {
  LOG_PRINT(F("Setting smell for "));
  LOG_PRINTLN(intensity);
  
  digitalWrite(PIN_SMELL, HIGH);
  triggerUpdateDelay(intensity * 1000);
}


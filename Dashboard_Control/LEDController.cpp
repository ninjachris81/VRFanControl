#include "LedController.h"

#include <LogHelper.h>

#include "CommController.h"
#include "TaskIDs.h"
#include "Protocol.h"
#include "GlobalConstants.h"

LedController::LedController() : AbstractIdleTask() {
  
}

void LedController::init() {
  for (uint8_t i=0;i<LED_STRIP_COUNT;i++) {
    switch(i) {
      case LED_LOCATION_DASHBOARD:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_DASHBOARD, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_FINS:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_FINS, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_CABLE_HOLDER:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_CABLE_HOLDER, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_HEADLIGHTS:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_HEADLIGHTS, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_HEADLIGHTS_AMB:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_HEADLIGHTS_AMB, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_MIDDLE_STRIP:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_MIDDLE_STRIP, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
    }
    
    currentColors[i].init(i, 0);
    currentColors[i].registerValueChangeListener(this);
  }
}

void LedController::update() {
}

void LedController::onInitialBroadcast() {
  for (uint8_t i=0;i<LED_STRIP_COUNT;i++) {
    taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_LED_COLOR_FB, i + '0', currentColors[i].getValue());
  }
}

void LedController::setColor(LED_LOCATION location, uint8_t colorIndex) {
  colorIndex = constrain(colorIndex, 0, COLOR_COUNT-1);
  location = constrain(location, 0, LED_STRIP_COUNT-1);
  currentColors[location].setValue(colorIndex);
}

void LedController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  LOG_PRINT(F("LED "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(newValue);

  CRGB color = colors[newValue];
  fill_solid(leds[id], ledCounts[id], color);
  controllers[id]->showLeds(brightnesses[id]);

  taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_LED_COLOR_FB, id + '0', newValue);
}

void LedController::setBrightness(LedController::LED_LOCATION location, uint8_t value) {
  brightnesses[location] = map(value, 0, 9, 0, 255);
  controllers[location]->showLeds(brightnesses[location]);
}

LedController::LED_LOCATION LedController::resolveLocation(uint8_t c) {
  switch(c) {
    case MOD_LED_DASHBOARD:
      return LED_LOCATION_DASHBOARD;
    case MOD_LED_FINS:
      return LED_LOCATION_FINS;
    case MOD_LED_CABLE_HOLDER:
      return LED_LOCATION_CABLE_HOLDER;
    case MOD_LED_HEADLIGHTS:
      return LED_LOCATION_HEADLIGHTS;
    case MOD_LED_HEADLIGHTS_AMB:
      return LED_LOCATION_HEADLIGHTS_AMB;
    case MOD_LED_MIDDLE_STRIP:
      return LED_LOCATION_MIDDLE_STRIP;
  }

  return LED_LOCATION_INVALID;
}

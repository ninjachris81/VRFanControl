#include "LedController.h"

#include <LogHelper.h>

#include "CommController.h"
#include "TaskIDs.h"
#include "Protocol.h"
#include "GlobalConstants.h"

LedController::LedController() : AbstractIdleTask() {
  
}

void LedController::init() {
  colors[0] = LED_COLOR_0;

  ledCounts[LED_LOCATION_DASHBOARD] = LED_COUNT_DASHBOARD;
  ledCounts[LED_LOCATION_FINS] = LED_COUNT_FINS;
  ledCounts[LED_LOCATION_CABLE_HOLDER] = LED_COUNT_CABLE_HOLDER;

  for (uint8_t i=0;i<LED_STRIP_COUNT;i++) {
    leds[i] = new CRGB[ledCounts[i]];
  }

  controllers[LED_LOCATION_DASHBOARD] = &FastLED.addLeds<WS2812B, PIN_LED_DASHBOARD, LED_COLOR_MODE>(leds[LED_LOCATION_DASHBOARD], LED_COUNT_DASHBOARD).setCorrection( LED_CORRECTION );
  controllers[LED_LOCATION_FINS] = &FastLED.addLeds<WS2812B, PIN_LED_FINS, LED_COLOR_MODE>(leds[LED_LOCATION_FINS], LED_COUNT_FINS).setCorrection( LED_CORRECTION );
  controllers[LED_LOCATION_CABLE_HOLDER] = &FastLED.addLeds<WS2812B, PIN_LED_CABLE_HOLDER, LED_COLOR_MODE>(leds[LED_LOCATION_CABLE_HOLDER], LED_COUNT_CABLE_HOLDER).setCorrection( LED_CORRECTION );

  for (uint8_t i=0;i<LED_STRIP_COUNT;i++) {
    brightnesses[i] = LED_DEFAULT_BRIGHTNESS;
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

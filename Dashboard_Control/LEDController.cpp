#include "LedController.h"

#include <LogHelper.h>

#include "CommController.h"
#include "TaskIDs.h"
#include "Protocol.h"
#include "GlobalConstants.h"

LedController::LedController() : AbstractIntervalTask(LED_BLEND_TRIGGER_INTERVAL) {
  
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
      case LED_LOCATION_HEADLIGHTS_LEFT:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_HEADLIGHTS_LEFT, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_HEADLIGHTS_RIGHT:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_HEADLIGHTS_RIGHT, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_HEADLIGHTS_AMB_LEFT:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_HEADLIGHTS_AMB_LEFT, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_HEADLIGHTS_AMB_RIGHT:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_HEADLIGHTS_AMB_RIGHT, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
      case LED_LOCATION_MIDDLE_STRIP:
        controllers[i] = &FastLED.addLeds<WS2812B, PIN_LED_MIDDLE_STRIP, LED_COLOR_MODE>(leds[i], ledCounts[i]).setCorrection( LED_CORRECTION );
        break;
    }
    
    currentColors[i].init(i, 255);
    currentColors[i].registerValueChangeListener(this);
    currentColors[i].setValue(ledDefaultColors[i]);
  }
}

void LedController::update() {
  for (uint8_t i=0;i<LED_STRIP_COUNT;i++) {
    if (blendIndexes[i]!=BLEND_OFF) {
      if (blendColor(i, blendIndexes[i])) {
        // continue blend
        blendIndexes[i]+=LED_BLEND_STEP;
      } else {
        // blend finished
        blendIndexes[i]=BLEND_OFF;
        sendColorUpdate(i);
      }
    }
  }  
}

void LedController::onInitialBroadcast() {
  for (uint8_t i=0;i<LED_STRIP_COUNT;i++) {
    sendColorUpdate(i);
  }
}

void LedController::setColor(LED_LOCATION location, uint8_t colorIndex) {
  colorIndex = constrain(colorIndex, 0, COLOR_COUNT-1);
  location = constrain(location, 0, LED_STRIP_COUNT-1);

  if (blendIndexes[location]==BLEND_OFF) {
    currentColors[location].setValue(colorIndex);
  } else {
    LOG_PRINTLN(F("Blending in progress"));
  }
}

void LedController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  LOG_PRINT(F("LED "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(newValue);

  startBlendColor(id, oldValue);
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
    case MOD_LED_MIDDLE_STRIP:
      return LED_LOCATION_MIDDLE_STRIP;
    case MOD_LED_HEADLIGHTS:
      return LED_LOCATION_HEADLIGHTS_LEFT;      // resolve to left
    case MOD_LED_HEADLIGHTS_AMB:
      return LED_LOCATION_HEADLIGHTS_AMB_LEFT;    // resolve to left
  }

  return LED_LOCATION_INVALID;
}

void LedController::sendColorUpdate(uint8_t index) {
  if (index!=LED_LOCATION_HEADLIGHTS_RIGHT && index!=LED_LOCATION_HEADLIGHTS_AMB_RIGHT) taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_LED_COLOR_FB, INDEX_TO_LED_MOD(index), currentColors[index].getValue());
}

void LedController::startBlendColor(uint8_t index, uint8_t startColor) {
  blendIndexes[index] = 0;
  blendStartColors[index] = startColor;
}

bool LedController::blendColor(uint8_t index, int blendStep) {
  if (blendStep<0 || blendStep>255) return false;

/*
  LOG_PRINT(F("Blending "));
  LOG_PRINT(index);
  LOG_PRINT(" ");
  LOG_PRINTLN(blendStep);
*/

  CRGB targetColor = colors[currentColors[index].getValue()];
  CRGB color = blend(colors[blendStartColors[index]], targetColor, blendStep);
  fill_solid(leds[index], ledCounts[index], color);
  controllers[index]->showLeds(brightnesses[index]);

/*
  LOG_PRINT("(");
  LOG_PRINT(color.r);
  LOG_PRINT(",");
  LOG_PRINT(color.g);
  LOG_PRINT(",");
  LOG_PRINT(color.b);
  LOG_PRINTLN(")");
*/

  return color!=targetColor;
}

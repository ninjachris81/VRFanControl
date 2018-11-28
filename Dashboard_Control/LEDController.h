#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <AbstractIntervalTask.h>
#include <FastLED.h>
#include <Property.h>

#include "Pins.h"
#include "GlobalConstants.h"
#include "InitialBroadcastSupport.h"

#define LED_COLOR_MODE GRB
#define LED_CORRECTION TypicalSMD5050

#define LED_BLEND_TRIGGER_INTERVAL 10
#define LED_BLEND_STEP 2;

class LedController : public AbstractIntervalTask, public Property<uint8_t>::ValueChangeListener, public InitialBroadcastSupport {
public:
  LedController();

  enum LED_LOCATION {
    LED_LOCATION_INVALID = -1,
    LED_LOCATION_DASHBOARD,
    LED_LOCATION_FINS,
    LED_LOCATION_CABLE_HOLDER,
    LED_LOCATION_HEADLIGHTS_LEFT,
    LED_LOCATION_HEADLIGHTS_RIGHT,
    LED_LOCATION_HEADLIGHTS_AMB,
    LED_LOCATION_MIDDLE_STRIP
  };

  void init();

  void update();

  void onInitialBroadcast();

  void setColor(LED_LOCATION location, uint8_t colorIndex);

  void setBrightness(LED_LOCATION location, uint8_t value);

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

  LED_LOCATION resolveLocation(uint8_t c);

private:
  const CRGB colors[COLOR_COUNT] = {LED_COLOR_0, LED_COLOR_1, LED_COLOR_2, LED_COLOR_3};

  uint8_t brightnesses[LED_STRIP_COUNT] = {LED_BRIGHTNESS_DASHBOARD, LED_BRIGHTNESS_FINS, LED_BRIGHTNESS_CABLE_HOLDER, LED_BRIGHTNESS_HEADLIGHTS_LEFT, LED_BRIGHTNESS_HEADLIGHTS_RIGHT, LED_BRIGHTNESS_HEADLIGHTS_AMB, LED_BRIGHTNESS_MIDDLE_STRIP};
  CRGB* leds[LED_STRIP_COUNT] = {new CRGB[LED_COUNT_DASHBOARD], new CRGB[LED_COUNT_FINS], new CRGB[LED_COUNT_CABLE_HOLDER], new CRGB[LED_COUNT_HEADLIGHTS_LEFT], new CRGB[LED_COUNT_HEADLIGHTS_RIGHT], new CRGB[LED_COUNT_HEADLIGHTS_AMB], new CRGB[LED_COUNT_MIDDLE_STRIP]};
  const uint8_t ledCounts[LED_STRIP_COUNT] = {LED_COUNT_DASHBOARD, LED_COUNT_FINS, LED_COUNT_CABLE_HOLDER, LED_COUNT_HEADLIGHTS_LEFT, LED_COUNT_HEADLIGHTS_RIGHT, LED_COUNT_HEADLIGHTS_AMB, LED_COUNT_MIDDLE_STRIP};
  uint8_t ledDefaultColors[LED_STRIP_COUNT] = {0, 0, 3, 3, 3, 0, 0};
  
  int blendIndexes[LED_STRIP_COUNT] = {BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF};
  uint8_t blendStartColors[LED_STRIP_COUNT] = {0, 0, 0, 0, 0, 0, 0};
  
  CLEDController *controllers[LED_STRIP_COUNT];
  Property<uint8_t> currentColors[LED_STRIP_COUNT];

  uint8_t mapIndexToMod(uint8_t index);

  void startBlendColor(uint8_t index, uint8_t startColor);

  bool blendColor(uint8_t index, int blendStep);

};


#endif    //LEDCONTROLLER_H

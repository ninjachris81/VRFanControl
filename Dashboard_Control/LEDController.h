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
    LED_LOCATION_MIDDLE_STRIP,
    LED_LOCATION_HEADLIGHTS_LEFT,
    LED_LOCATION_HEADLIGHTS_RIGHT,
    LED_LOCATION_HEADLIGHTS_AMB_LEFT,
    LED_LOCATION_HEADLIGHTS_AMB_RIGHT
  };

  void init();

  void update();

  void onInitialBroadcast();

  void setColor(LED_LOCATION location, uint8_t colorIndex);

  void setBrightness(LED_LOCATION location, uint8_t value);

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

  LED_LOCATION resolveLocation(uint8_t c);

private:
  const CRGB colors[COLOR_COUNT+1] PROGMEM = {0, LED_COLOR_0, LED_COLOR_1, LED_COLOR_2, LED_COLOR_3, LED_COLOR_4, LED_COLOR_5, LED_COLOR_6, LED_COLOR_7, LED_COLOR_8};

  uint8_t brightnesses[LED_STRIP_COUNT] = {LED_BRIGHTNESS_DASHBOARD, LED_BRIGHTNESS_FINS, LED_BRIGHTNESS_CABLE_HOLDER, LED_BRIGHTNESS_MIDDLE_STRIP, LED_BRIGHTNESS_HEADLIGHTS_LEFT, LED_BRIGHTNESS_HEADLIGHTS_RIGHT, LED_BRIGHTNESS_HEADLIGHTS_AMB_LEFT, LED_BRIGHTNESS_HEADLIGHTS_AMB_RIGHT };
  CRGB* leds[LED_STRIP_COUNT] = {new CRGB[LED_COUNT_DASHBOARD], new CRGB[LED_COUNT_FINS], new CRGB[LED_COUNT_CABLE_HOLDER], new CRGB[LED_COUNT_MIDDLE_STRIP], new CRGB[LED_COUNT_HEADLIGHTS_LEFT], new CRGB[LED_COUNT_HEADLIGHTS_RIGHT], new CRGB[LED_COUNT_HEADLIGHTS_AMB_LEFT], new CRGB[LED_COUNT_HEADLIGHTS_AMB_RIGHT] };
  const uint8_t ledCounts[LED_STRIP_COUNT] PROGMEM = {LED_COUNT_DASHBOARD, LED_COUNT_FINS, LED_COUNT_CABLE_HOLDER, LED_COUNT_MIDDLE_STRIP, LED_COUNT_HEADLIGHTS_LEFT, LED_COUNT_HEADLIGHTS_RIGHT, LED_COUNT_HEADLIGHTS_AMB_LEFT, LED_COUNT_HEADLIGHTS_AMB_RIGHT };
  const uint8_t ledDefaultColors[LED_STRIP_COUNT] PROGMEM = {1, 1, 1, 1, 4, 4, 4, 4};
  
  int blendIndexes[LED_STRIP_COUNT] = {BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF, BLEND_OFF};
  uint8_t blendStartColors[LED_STRIP_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0};
  
  CLEDController *controllers[LED_STRIP_COUNT];
  Property<uint8_t> currentColors[LED_STRIP_COUNT];

  void startBlendColor(uint8_t index, uint8_t startColor);

  bool blendColor(uint8_t index, int blendStep);

  void sendColorUpdate(uint8_t index);
  
  void sendBrightnessUpdate(uint8_t index);

  void showLeds(LED_LOCATION index);

};


#endif    //LEDCONTROLLER_H

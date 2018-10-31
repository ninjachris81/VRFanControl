#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <AbstractIdleTask.h>
#include <FastLED.h>
#include <Property.h>

#include "Pins.h"
#include "GlobalConstants.h"
#include "InitialBroadcastSupport.h"

#define LED_COLOR_MODE GRB
#define LED_CORRECTION TypicalSMD5050

class LedController : public AbstractIdleTask, public Property<uint8_t>::ValueChangeListener, public InitialBroadcastSupport {
public:
  LedController();

  enum LED_LOCATION {
    LED_LOCATION_INVALID = -1,
    LED_LOCATION_DASHBOARD,
    LED_LOCATION_FINS,
    LED_LOCATION_CABLE_HOLDER,
    LED_LOCATION_HEADLIGHTS,
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
  const CRGB colors[COLOR_COUNT] = {LED_COLOR_0, LED_COLOR_1};

  uint8_t brightnesses[LED_STRIP_COUNT] = {LED_BRIGHTNESS_DASHBOARD, LED_BRIGHTNESS_FINS, LED_BRIGHTNESS_CABLE_HOLDER, LED_BRIGHTNESS_HEADLIGHTS, LED_BRIGHTNESS_HEADLIGHTS_AMB, LED_BRIGHTNESS_MIDDLE_STRIP};
  CRGB* leds[LED_STRIP_COUNT] = {new CRGB[LED_COUNT_DASHBOARD], new CRGB[LED_COUNT_FINS], new CRGB[LED_COUNT_CABLE_HOLDER], new CRGB[LED_COUNT_HEADLIGHTS], new CRGB[LED_COUNT_HEADLIGHTS_AMB], new CRGB[LED_COUNT_MIDDLE_STRIP]};
  const uint8_t ledCounts[LED_STRIP_COUNT] = {LED_COUNT_DASHBOARD, LED_COUNT_FINS, LED_COUNT_CABLE_HOLDER, LED_COUNT_HEADLIGHTS, LED_COUNT_HEADLIGHTS_AMB, LED_COUNT_MIDDLE_STRIP};
  
  CLEDController *controllers[LED_STRIP_COUNT];
  Property<uint8_t> currentColors[LED_STRIP_COUNT];

};


#endif    //LEDCONTROLLER_H

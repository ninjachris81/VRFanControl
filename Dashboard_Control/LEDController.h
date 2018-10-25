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
    LED_LOCATION_CABLE_HOLDER
  };

  void init();

  void update();

  void onInitialBroadcast();

  void setColor(LED_LOCATION location, uint8_t colorIndex);

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

private:
  CRGB colors[COLOR_COUNT];

  uint8_t brightnesses[LED_STRIP_COUNT];
  CRGB* leds[LED_STRIP_COUNT];
  uint8_t ledCounts[LED_STRIP_COUNT];
  CLEDController *controllers[LED_STRIP_COUNT];
  Property<uint8_t> currentColors[LED_STRIP_COUNT];

};


#endif    //LEDCONTROLLER_H

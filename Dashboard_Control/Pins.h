#ifdef ARDUINO_AVR_UNO

  #define PIN_MOSFET_FAN 3
  
  #define PIN_MM_RELAY1 4
  #define PIN_MM_RELAY2 7
  #define PIN_MM_RELAY3 8
  
  #define PIN_MM_LEVEL1 A0
  #define PIN_MM_LEVEL2 A1
  #define PIN_MM_LEVEL3 A2
  
  #define PIN_COMM_RX A4
  #define PIN_COMM_TX A5
  
  #define PIN_LED_DASHBOARD 5
  #define PIN_LED_FINS 6
  #define PIN_LED_CABLE_HOLDER 9
  #define PIN_LED_HEADLIGHTS 9    // TBD
  #define PIN_LED_HEADLIGHTS_AMB 9  // TBD
  #define PIN_LED_MIDDLE_STRIP 9 // TBD
  
#elif ARDUINO_AVR_MEGA2560

  #define PIN_MOSFET_FAN 2
  
  #define PIN_MM_RELAY1 26
  #define PIN_MM_RELAY2 28
  #define PIN_MM_RELAY3 30
  
  #define PIN_MM_LEVEL1 A0
  #define PIN_MM_LEVEL2 A1
  #define PIN_MM_LEVEL3 A2
  
  #define PIN_COMM_RX 22
  #define PIN_COMM_TX 24
  
  #define PIN_LED_DASHBOARD 3
  #define PIN_LED_FINS 4
  #define PIN_LED_CABLE_HOLDER 5
  #define PIN_LED_HEADLIGHTS_LEFT 7
  #define PIN_LED_HEADLIGHTS_RIGHT 8
  #define PIN_LED_HEADLIGHTS_AMB 9    // TBC
  #define PIN_LED_MIDDLE_STRIP 44

#else 
  #error Unsupported board
#endif

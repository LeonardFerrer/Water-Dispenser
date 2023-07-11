
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

extern bool restartEsp;
extern unsigned long time2Restart;
extern bool onPump;
extern unsigned long onDispenserTime;

extern uint8_t BUMP_PIN;
extern uint8_t powerLed;
extern uint8_t waterPumpLed;


#endif
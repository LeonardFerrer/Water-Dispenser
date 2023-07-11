#include "Globals.h"


bool restartEsp = false;
unsigned long time2Restart = 0;
bool onPump = false;
unsigned long onDispenserTime = 0;
unsigned long timeButton = 0;

uint8_t BUMP_PIN = D2;
uint8_t powerLed = D0;
uint8_t waterPumpLed = D3;
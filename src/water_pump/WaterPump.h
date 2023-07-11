#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include <Arduino.h>
#include <globals/Globals.h>
#include <web_socket/WebSocket.h>

extern unsigned long debounceTimeOff;
extern unsigned long debounceTimeOn;
extern uint8_t buttonTurnOff;
extern uint8_t buttonTurnOn;
extern bool sttOff;
extern bool sttOn;
extern bool isContinuous;
extern float volume;
extern float ratePump;

extern unsigned long timeMeasuringVolume;
extern unsigned long clockWaterPump;
extern bool sttMeasure;

enum StatusWaterPump {
    USE_CONTINUOUS,
    USE_BY_VOLUME,
    MEASURING_VOLUME,
    DISABLED,
    WAIT
};

extern enum StatusWaterPump stateWaterPump;

void onWaterPumpMeasure();
void onWaterPumpByVolume();
void onWaterPumpContinues();
void turnOffPump();
void turnOnPump();
double measureVolume();
void activateWaterPump();
void deactivateWaterPump();

#endif
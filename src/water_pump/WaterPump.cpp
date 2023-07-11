#include "WaterPump.h"

uint8_t buttonTurnOff = D6;
uint8_t buttonTurnOn = D7;
bool sttOff = false;
bool sttOn = false;
unsigned long debounceTimeOff = 0;
unsigned long debounceTimeOn = 0;
unsigned long timeMeasuringVolume = 0;
bool isContinuous = false;
bool isMeasure = false;
unsigned long clockWaterPump = 0;
float volume = 0.0;
float ratePump = 52000.0;

enum StatusWaterPump stateWaterPump = StatusWaterPump::WAIT;

/// @brief Ligar a bomba para preencher um certo volume pre-definido
void onWaterPumpByVolume()
{
    activateWaterPump();
    if (millis() > clockWaterPump + volume * ratePump)
    {
        deactivateWaterPump();
    }
}

void onWaterPumpMeasure()
{
    if (onPump)
    {
        activateWaterPump();
    }
}

/// @brief Ligar a bomba d'água continuamente
void onWaterPumpContinues()
{
    if (onPump)
    {
        activateWaterPump();
    }
    else
    {
        stateWaterPump = DISABLED;
    }
}

/// @brief Função invocado quando o botão Cancelar é pressionado.
void turnOffPump()
{
    int reading = digitalRead(buttonTurnOff);
    if (reading == LOW)
    {
        debounceTimeOff = millis();
        sttOff = true;
    }

    if (millis() > debounceTimeOff + 200)
    {
        if (sttOff)
        {
            
            if (stateWaterPump == StatusWaterPump::MEASURING_VOLUME)
            {
                notifyCompletionVolumeMeasurement();
            }
            else
            {
                stateWaterPump = DISABLED;
                notifyStatusAllClients(StatusService::OFF);
            }
            sttOff = false;
        }
    }
}

/// @brief Função invocada quando é pressionado o botão ON, ligando continuamente a bomba
void turnOnPump()
{

    int readingOn = digitalRead(buttonTurnOn);
    if (readingOn == LOW)
    {
        debounceTimeOn = millis();
        sttOn = true;
    }

    if (millis() > debounceTimeOn + 200)
    {
        if (sttOn)
        {
            if (!onPump)
            {
                stateWaterPump = USE_CONTINUOUS;
                onPump = true;
                notifyStatusAllClients(StatusService::CONTINUOUS);
                userOnPump = 0;
                sttOn = false;
            }
        }
    }
}

double measureVolume()
{
    double vol = 0.0;
    deactivateWaterPump();
    int timeMeasuring = millis() - clockWaterPump;
    vol = timeMeasuring / ratePump;

    return vol;
}

/// @brief Ativar a bomba d'água
void activateWaterPump()
{
    digitalWrite(waterPumpLed, HIGH); // acender led
    digitalWrite(BUMP_PIN, HIGH);     // Ligar a bomba
}

/// @brief desativar a bomba d'água
void deactivateWaterPump()
{
    digitalWrite(waterPumpLed, LOW); // desligar a bomba
    digitalWrite(BUMP_PIN, LOW);     // desligar a bomba
    onPump = false;
    userOnPump = -1;
    volume = 0.0;
    notifyStatusAllClients(StatusService::OFF);
    stateWaterPump = StatusWaterPump::WAIT;
}
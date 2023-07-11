#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include "AsyncWebSocket.h"
#include "ArduinoJson.h"
#include <globals/Globals.h>
#include <water_pump/WaterPump.h>
#include <map>


extern AsyncWebSocket ws;
extern int userOnPump;

enum StatusService {
    OFF,
    USE,
    ON,
    CONTINUOUS
};

extern enum StatusService stateService;
extern std::map<int, String> stateServiceMap;

void initWebSocket(AsyncWebServer& server);
void notifyStatusAllClients(StatusService status);
void notifyStatusAllClientExcept(uint32_t idClient, StatusService status);
void notifyCompletionVolumeMeasurement();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t idClient);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

#endif
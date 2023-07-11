#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <file_custom/FileCustom.h>
#include <globals/Globals.h>

extern String bodyRequest;

void routers(AsyncWebServer& server);
void fileManager(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);
void configureNetworkSettings(AsyncWebServerRequest *request);
void onBotte(AsyncWebServerRequest *request);
void updateBottes(AsyncWebServerRequest *request);
void updateBottesBodyRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);


#endif

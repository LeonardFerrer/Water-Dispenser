#include "WebSocket.h"

AsyncWebSocket ws("/ws");
int userOnPump = -1;
enum StatusService stateService = StatusService::OFF;

std::map<int, String> stateServiceMap = {
    {StatusService::OFF, "off"},
    {StatusService::USE, "busy"},
    {StatusService::CONTINUOUS, "continuous"},
    {StatusService::ON, "on"},
};

void initWebSocket(AsyncWebServer &server)
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t idClient)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        String msg = String((char *)data);
        if (strcmp((char *)data, "getClients") == 0)
        {
            AsyncWebSocket::AsyncWebSocketClientLinkedList clients = ws.getClients();
            Serial.println("Lista Clientes");
            for (const auto &c : clients)
            {
                if (c->status() == WS_CONNECTED)
                {
                    Serial.print("Client #");
                    Serial.println(c->id());
                }
            }
            Serial.println("");
        }
        if (strcmp((char *)data, "onBotteContinuous") == 0)
        {
            Serial.println("continuo");
            stateWaterPump = StatusWaterPump::USE_CONTINUOUS;
            onPump = true;
            userOnPump = idClient;
            notifyStatusAllClientExcept(idClient, StatusService::USE);
        }

        else if (strcmp((char *)data, "measured_volume_on") == 0)
        {
            if (stateWaterPump == StatusWaterPump::WAIT)
            {
                stateWaterPump = StatusWaterPump::MEASURING_VOLUME;
                userOnPump = idClient;
                onPump = true;
                clockWaterPump = millis();
                volume = 0.0;
                notifyStatusAllClientExcept(idClient, StatusService::USE);
            }
        }
        else if (strcmp((char *)data, "measured_volume_off") == 0)
        {
            if (userOnPump == (int)idClient || userOnPump == 0)
            {
                notifyCompletionVolumeMeasurement();
            }
        }
        else if (msg.startsWith("onbotte"))
        {
            String json = msg.substring(7);
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, json);
            if (error)
                Serial.println(F("Erro ao na deserializacao do json OnBotte."));
            else
            {
                if (stateWaterPump == StatusWaterPump::WAIT)
                {
                    Serial.print("NOME: ");
                    Serial.println((const char *)doc["nome"]);
                    Serial.print("VOLUME: ");
                    Serial.println((double)doc["volume"]);
                    Serial.println("");
                    stateWaterPump = StatusWaterPump::USE_BY_VOLUME;
                    userOnPump = idClient;
                    clockWaterPump = millis();
                    volume = (float)doc["volume"];
                    onPump = true;
                    notifyStatusAllClientExcept(idClient, StatusService::USE);
                }
            }
        }
        else if (msg.startsWith("offbotte"))
        {
            if (userOnPump == 0 || (int)idClient == userOnPump) // Se a  bomba foi ligado por equipamento
            {
                stateWaterPump = StatusWaterPump::DISABLED;
                notifyStatusAllClients(StatusService::OFF);
            }
        }
        else
        {
            // Serial.println(msg);
        }
    }
}

void webSocketMessage(void *arg, uint8_t *data, size_t len, uint32_t idClient)
{
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        deactivateWaterPump();
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len, client->id());
        webSocketMessage(arg, data, len, client->id());
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void notifyStatusAllClients(StatusService status)
{
    ws.textAll("onbotte{\"status\":\"" + stateServiceMap[status] + "\"}");
}

void notifyStatusAllClientExcept(uint32_t idClient, StatusService status)
{
    AsyncWebSocket::AsyncWebSocketClientLinkedList clients = ws.getClients();
    for (const auto &c : clients)
    {
        if (c->status() == WS_CONNECTED and c->id() != idClient)
        {
            ws.text(c->id(), "onbotte{\"status\":\"" + stateServiceMap[status] + "\"}");
        }
    }
}

void notifyCompletionVolumeMeasurement()
{
    uint32_t client = userOnPump;
    double vol = measureVolume();
    String message = "completed_measurement{\"volume\":" + String(vol) + "}";
    Serial.printf("volume calculado: %f\n", vol);
    ws.text(client, message);
    notifyStatusAllClientExcept(client, StatusService::OFF);
}
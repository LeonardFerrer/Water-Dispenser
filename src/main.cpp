#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"

#include <file_custom/FileCustom.h>
#include <server/server.h>
#include <web_socket/WebSocket.h>
#include <globals/Globals.h>
#include <water_pump/WaterPump.h>

AsyncWebServer server(80);

bool modeStation();
void modeAccessPoint();

// ###################################################################

void setup()
{
    Serial.begin(9600);
    Serial.println("");

    pinMode(buttonTurnOff, INPUT_PULLUP);
    pinMode(buttonTurnOn, INPUT_PULLUP);
    pinMode(powerLed, OUTPUT);
    pinMode(waterPumpLed, OUTPUT);
    pinMode(BUMP_PIN, OUTPUT);

    digitalWrite(powerLed, LOW);
    digitalWrite(waterPumpLed, LOW);
    digitalWrite(BUMP_PIN, LOW);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    if (!modeStation())
        modeAccessPoint();

    initWebSocket(server);
    routers(server);

    server.begin();
}

// ###################################################################

void loop()
{
    ws.cleanupClients();
    if (restartEsp)
    {
        if (millis() > time2Restart + 5000)
        {
            Serial.println(F(("Reiniciando")));
            ESP.restart();
        }
    }

    switch (stateWaterPump)
    {
    case USE_CONTINUOUS:
        onWaterPumpContinues();
        break;
    case USE_BY_VOLUME:
        onWaterPumpByVolume();
        break;
    case MEASURING_VOLUME:
        onWaterPumpMeasure();
        break;
    case DISABLED:
        deactivateWaterPump();
        break;
    case WAIT:
        break;
    }

    turnOffPump();
    turnOnPump();
}

// ###################################################################

/// @brief realizar as configurações necessárias no ESP para funcionar em modo STATION.
/// @retval \c TRUE, caso consigar entrar em modo Station;
/// @retval \c FALSE, caso contrário.
bool modeStation()
{
    String json = readFile("/network_settings.json");
    Serial.println(json);
    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, json);

    if (error)
    {
        Serial.println(F("Erro ao carregar as configurações de rede."));
        return false;
    }

    if (!(doc["ip"] == "") && !(doc["gateway"] == "") && !(doc["subnet"] == ""))
    {
        IPAddress ip;
        IPAddress gateway;
        IPAddress subnet;
        ip.fromString((const char *)doc["ip"]);
        gateway.fromString((const char *)doc["gateway"]);
        subnet.fromString((const char *)doc["subnet"]);

        Serial.println(ip);
        Serial.println(gateway);
        Serial.println(subnet);

        if (!WiFi.config(ip, gateway, subnet))
        {
            Serial.println(F("\nErro na definição do IP."));
            return false;
        }
    }

    // doc.clear();

    WiFi.mode(WIFI_STA);
    WiFi.begin((const char *)doc["ssid"], (const char *)doc["password"]);

    int sttPowerLed = HIGH;

    for (int counter = 0; WiFi.status() != WL_CONNECTED && counter < 30; counter++)
    {
        Serial.print(F("."));
        digitalWrite(powerLed, sttPowerLed);
        sttPowerLed = !sttPowerLed;
        delay(400);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("\nfalha ao logar na rede WiFi: \"" + WiFi.SSID() + "\"");
        Serial.println(F("Senha ou rede incorretos"));
        return false;
    }

    Serial.println("\nREDE: " + WiFi.SSID());
    Serial.println("acesse o IP: " + WiFi.localIP().toString());
    digitalWrite(powerLed, HIGH);
    return true;
}

// ###################################################################

/// @brief configura o ESP em modo Access Point.
void modeAccessPoint()
{
    digitalWrite(powerLed, LOW);
    IPAddress ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, gateway, subnet);

    if (!WiFi.softAP("Water-Dispenser", NULL))
    {
        Serial.println(F("Falha ao entrar em Modo Access Point"));
        restartEsp = true;
        time2Restart = millis();
        return;
    }

    Serial.println(F("\nModo Access Point"));
    Serial.println("login na rede: " + WiFi.softAPSSID());
    Serial.println("acesse IP: " + WiFi.softAPIP().toString());
}

// {"ssid":"Brasilink-Lucas", "password":"deusnocomando", "ip":"192.168.1.20", "gateway":"192.168.1.1", "subnet":"255.255.255.0"}
// {"ssid":"Sitio_Organico", "password":"casa@D63", "ip":"192.168.3.115", "gateway":"192.168.3.1", "subnet":"255.255.255.0"}
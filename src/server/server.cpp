#include "server.h"

String bodyRequest = "";

void fileManager(AsyncWebServerRequest *request)
{
    String url;
    String extension;
    url = request->url() == "/" ? "/index.html" : request->url();

    if (url.endsWith("html"))
        extension = "text/html";
    else if (url.endsWith("css"))
        extension = "text/css";
    else if (url.endsWith("js"))
        extension = "text/javascrypt";
    else if (url.endsWith("png"))
        extension = "image/png";
    else if (url.endsWith("json"))
        extension = "application/json";
    else
        extension = "text/plain";

    request->send(LittleFS, url, extension);
}

/// @brief Define as rotas dentro do servidor que o cliente terá acesso
void routers(AsyncWebServer &server)
{
    server.on("/", fileManager);
    server.on("/index.html", fileManager);
    server.on("/index.css", fileManager);
    server.on("/index.js", fileManager);
    server.on("/menu.png", fileManager);
    server.on("/navbar.css", fileManager);
    server.on("/navbar.js", fileManager);
    server.on("/bottes.json", fileManager);
    server.on("/network_settings.json", fileManager);
    server.on("/network_settings.js", fileManager);
    server.on("/network_settings.css", fileManager);
    server.on("/editar.png", fileManager);
    server.on("/add.png", fileManager);
    server.on("/network_settings.html", HTTP_GET, fileManager);
    server.on("/network_settings.html", HTTP_POST, configureNetworkSettings);
    server.on("/updateBottes", HTTP_POST, updateBottes, updateBottesBodyRequest);
    server.on("/onBotte", HTTP_POST, onBotte);
    server.onNotFound(notFound);
}

/// @brief Handle invocado quando o cliente tentar acessar uma rota não disponivel pelo servidor, enviado ao cliente uma pagina padrão de código 404.
/// @param request requisição do cliente
void notFound(AsyncWebServerRequest *request)
{
    String response = "Nao encontrada URL: " + WiFi.localIP().toString() + request->url();
    request->send(404, "text/plain", response);
}

/// @brief Handle invocado para define as novas configurações de rede. Através dessa função é salva os novos valores de SSID, PASSWORD, IP, GATEWAY e SUBNET.
/// @param request requisição do cliente
void configureNetworkSettings(AsyncWebServerRequest *request)
{
    AsyncWebParameter *p;
    String settings = "{";
    String key[] = {"ssid", "password", "ip", "gateway", "subnet"};
    for (int i = 0; i < 5; i++)
    {
        if (request->hasParam(key[i], true))
        {
            p = request->getParam(key[i], true);
            settings += "\"" + key[i] + "\":\"" + p->value() + "\"";
        }
        else
            settings += "\"" + key[i] + "\": \"...\"";

        if (i != 4)
            settings += ",";
        else
            settings += "}";
    }
    p = request->getParam("ip", true);
    String ip = p->value();

    writeFile("/network_settings.json", settings.c_str());

    String htmlContent = ""; // Variável para armazenar o conteúdo do arquivo

    // Ler o arquivo restartESP.html
    File file = LittleFS.open("/restartESP.html", "r");
    if (file)
    {
        // Ler o conteúdo do arquivo
        while (file.available())
        {
            htmlContent += file.readString();
        }
        file.close();
        htmlContent.replace("{COUNTER_VALUE}", ("'http://"+ip+"/'"));
        request->send(200, "text/html",htmlContent);
    }
    time2Restart = millis();
    restartEsp = true;

    // request->send(LittleFS, "/restartESP.html", "text/html");
}

/// @brief Aciona a bomba d'água de acordo com o volume contido na request
/// @param request request do cliente
void onBotte(AsyncWebServerRequest *request)
{
    Serial.println("");
    if (request->hasParam("name", true))
    {
        AsyncWebParameter *p = request->getParam("name", true);
        Serial.printf("%s: %s\n", p->name().c_str(), p->value().c_str());
    }
    else
    {
        Serial.println("falhou name");
    }

    if (request->hasParam("volume", true))
    {
        AsyncWebParameter *p = request->getParam("volume", true);
        Serial.printf("%s: %s\n", p->name().c_str(), p->value().c_str());
    }
    else
    {
        Serial.println("falhou volume");
    }

    Serial.println("ligando....");
    onDispenserTime = millis();
    onPump = true;
    request->send(200, "text/plain", "ok");
}

/// @brief Atualiza a lista de garrafas no servidor
/// @param request
void updateBottes(AsyncWebServerRequest *request)
{
    writeFile("/bottes.json", bodyRequest.c_str());
    bodyRequest = "";
    request->send(200, "text/plain", "OK");
}

/// @brief Handler responsavél por manipular o json contido no corpo da request de atualização das bottes no servidor
/// @param request handler request
/// @param data dados contidos no body da request
/// @param len tamanho  do array cque contém os dados do body
/// @param index indica o inicio dos dados da request
/// @param total tamanho todal do body
void updateBottesBodyRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyTemp = String((const char *)data);
    bodyRequest += bodyTemp.substring(0, len);
}

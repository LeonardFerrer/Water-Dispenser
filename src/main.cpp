#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
# include "ArduinoJson.h"

AsyncWebServer server(80);
unsigned long stopwatchStart=0;
bool restartESP = false;

//###################################################################
//Declaração de funções

bool modeStation();
void modeAccessPoint();
void initFS();
String fileRead(const char* path);
void fileWrite(const char* path, const char* content);
void routers();
void serverFileManager(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);
void configureNetworkSettings(AsyncWebServerRequest *request);


//###################################################################


void setup(){
    Serial.begin(9600);
    initFS();   
    if(!modeStation())  modeAccessPoint();
    routers();
    server.begin();
}


//###################################################################

void loop(){
    if(restartESP){
        if(millis()>stopwatchStart+5000){
            Serial.println(F(("Reiniciando")));
            ESP.restart();
        }
    }
}

//###################################################################


/// @brief realizar as configurações necessárias no ESP para funcionar em modo STATION.
/// @retval \c TRUE, caso consigar entrar em modo Station;
/// @retval \c FALSE, caso contrário.
bool modeStation(){
    StaticJsonBuffer<200> jsonBuffer;
    String json = fileRead("/network_settings.json");
    JsonObject& root = jsonBuffer.parseObject(json);
    if(!root.success()){
        Serial.println(F("Ero ao carregar as configurações de rede."));
        return false;
    }

    if( !(root["ip"].asString() == "") && !(root["gateway"].asString() == "") && !(root["subnet"].asString() == "")){
        IPAddress ip;
        IPAddress gateway;
        IPAddress subnet;
        ip.fromString(root["ip"].asString());
        gateway.fromString(root["gateway"].asString());
        subnet.fromString(root["subnet"].asString());    

        if(!WiFi.config(ip, gateway, subnet)){
            Serial.println(F("\nErro na definição do IP."));
            return false;
        }
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(root["ssid"].asString(), root["password"].asString());

    for( int counter=0; WiFi.status()!=WL_CONNECTED && counter <30; counter++){
        Serial.print(F("."));
        delay(400);
    }

    if(WiFi.status() != WL_CONNECTED){
        Serial.println("\nfalha ao logar na rede WiFi: \""+WiFi.SSID()+"\"");
        Serial.println(F("Senha ou rede incorretos"));
        return false;
    }

    Serial.println("\nREDE: "+WiFi.SSID());
    Serial.println("acesse o IP: "+WiFi.localIP().toString());
    return true;
}

//###################################################################

/// @brief configura o ESP em modo Access Point.
void modeAccessPoint(){

    IPAddress ip(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, gateway, subnet);

    if(!WiFi.softAP("Water-Dispenser", NULL)){
        Serial.println(F("Falha ao entrar em Modo Access Point"));
        restartESP = true;
        stopwatchStart = millis();
        return;
    }

    Serial.println(F("\nModo Access Point"));
    Serial.println("login na rede: "+WiFi.softAPSSID());
    Serial.println("acesse IP: "+WiFi.softAPIP().toString());
}


//###################################################################


/// @brief Inicia o sistema de arquivos LittlesFS
void initFS(){
    if(LittleFS.begin())    Serial.println(F("\nLittesFS iniciado com sucesso!"));
    else                    Serial.println(F("\nFalha ao iniciar o LittleFS!"));
}


//###################################################################

/// @brief Ler um arquivo na memoria.
/// @param path caminho do arquivo iniciado pela raiz "/".
/// @return uma string contendo o contéudo do arquivo.
String fileRead(const char* path){
    Serial.printf("Lendo arquivo: \"%s\"\r\n",path);
    File file = LittleFS.open(path, "r");

    if(!file || file.isDirectory()){
        Serial.println(F("Erro na leitura do arquivo"));
        return String();
    }

    String content;
    while(file.available()){
        content = file.readStringUntil('\n');
        break;
    }

    file.close();
    return content;
}


//###################################################################

/// @brief Escreve na mémoria op arquivo desejado juntamente com seu conteudo
/// @param path nome e caminho do arquivo começado na raiz "\"
/// @param content conteudo do arquivo a ser gravado
void fileWrite(const char* path, const char* content){
    File file = LittleFS.open(path, "w");
    if(!file){
        Serial.printf("Error ao abrir o arquivo \"%s\" para escrutar",path);
        return ;
    }

    if(!file.print(content)) Serial.printf("Erro ao gravar conteudo em \"%s\"",path);
    
    file.close();
}

//###################################################################

/// @brief Define as rotas dentro do servidor que o cliente terá acesso
void routers(){
    server.on("/", serverFileManager);
    server.on("/index.html", serverFileManager);
    server.on("/index.css", serverFileManager);
    server.on("/index.js", serverFileManager);
    server.on("/menu.png", serverFileManager);
    server.on("/navbar.css", serverFileManager);
    server.on("/navbar.js", serverFileManager);
    server.on("/bottes.json", serverFileManager);
    server.on("/network_settings.json", serverFileManager);
    server.on("/network_settings.css" ,serverFileManager);
    server.on("/network_settings.html",HTTP_GET ,serverFileManager);
    server.on("/network_settings.html", HTTP_POST ,configureNetworkSettings);
    server.onNotFound(notFound);
}

//###################################################################

/// @brief Gerencia os arquivos que serão enviados ao cliente de acordo com as rotas estabelecidas dentro do servidor
/// @param request requisição do cliente
void serverFileManager(AsyncWebServerRequest *request){
    String url;
    String extension;
    url = request->url() == "/" ? "/index.html" : request->url();

    if( url.endsWith("html"))        extension="text/html";
    else if( url.endsWith("css"))    extension="text/css";
    else if( url.endsWith("js"))     extension="text/javascrypt";
    else if( url.endsWith("png"))    extension="image/png";
    else if( url.endsWith("json"))   extension="application/json";
    else                             extension="text/plain";

    request->send(LittleFS, url, extension);
}

//###################################################################

/// @brief Handle invocado quando o cliente tentar acessar uma rota não disponivel pelo servidor, enviado ao cliente uma pagina padrão de código 404.
/// @param request requisição do cliente
void notFound(AsyncWebServerRequest *request){
    String response = "Nao encontrada URL: "+WiFi.localIP().toString()+request->url();
    request->send(404,"text/plain", response);
}


//###################################################################


/// @brief Hande=le invocado para define as novas configurações de rede. Através dessa função é salva os novos valores de SSID, PASSWORD, IP, GATEWAY e SUBNET.
/// @param request requisição do cliente
void configureNetworkSettings(AsyncWebServerRequest *request){
    AsyncWebParameter* p;
    String settings = "{";
    String key[] = {"ssid", "password", "ip", "gateway", "subnet"};
    for( int i=0; i<5; i++){
        if(request->hasParam(key[i], true)){
            p = request->getParam(key[i], true);
            settings += "\""+key[i]+"\":\""+p->value()+"\"";
        } else settings += "\""+key[i]+"\": \"...\"";

        if(i!=4)    settings += ",";
        else        settings += "}";
    }

    fileWrite("/network_settings.json",settings.c_str());
    stopwatchStart = millis();
    restartESP = true;
    request->send(200, "text/plain", " network settings write success!");
}

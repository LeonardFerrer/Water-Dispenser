#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
# include "ArduinoJson.h"

AsyncWebServer server(80);


//###################################################################
//Declaração de funções

void initFS();
String fileRead(const char* path);
void fileWrite(const char* path, const char* content);
bool modeStation();
void modeAccessPoint();

//###################################################################


void setup(){
    Serial.begin(9600);
    initFS();   

    modeStation(); 
}


//###################################################################

void loop(){

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
        Serial.println("\nfalha ao logar em \""+WiFi.SSID()+"\"");
        return false;
    }

    Serial.println("\nREDE: "+WiFi.SSID());
    Serial.println("IP: "+WiFi.localIP().toString());
    return true;
}

//###################################################################

void modeAccessPoint(){

}
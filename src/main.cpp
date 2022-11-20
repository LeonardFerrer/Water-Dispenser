#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"

//###################################################################
//Declaração de funções

void initFS();
String fileRead(const char* path);
void fileWrite(const char* path, const char* content);
void json();


//###################################################################


void setup(){
    Serial.begin(9600);
    initFS();    
}


//###################################################################

void loop(){

}

//###################################################################

/// @brief Inicia o sistema de arquivos LittlesFS
void initFS(){
    if(LittleFS.begin())    Serial.println(F("LittesFS iniciado com sucesso!"));
    else                    Serial.println(F("Falha ao iniciar o LittleFS!"));
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


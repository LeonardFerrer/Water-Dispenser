#include "FileCustom.h"
#include "LittleFS.h"

/// @brief Ler um arquivo na memoria.
/// @param path caminho do arquivo iniciado pela raiz "/".
/// @return uma string contendo o contéudo do arquivo.
String readFile(const char *path)
{
    if (!LittleFS.begin())
    {
        Serial.printf("Falha ao inicializar System File");
        return String();
    }

    Serial.printf("Abrindo arquivo: \"%s\"\r\n", path);
    File file = LittleFS.open(path, "r");

    if (!file || file.isDirectory())
    {
        Serial.println(F("Erro na leitura do arquivo"));
        return String();
    }

    String content;
    while (file.available())
    {
        content = file.readStringUntil('\n');
        break;
    }

    file.close();
    return content;
}

/// @brief Escreve na mémoria o arquivo desejado juntamente com seu conteudo
/// @param path nome e caminho do arquivo começado na raiz "\"
/// @param content conteudo do arquivo a ser gravado
bool writeFile(const char *path, const char *content)
{
    if (!LittleFS.begin())
    {
        Serial.printf("Falha ao inicializar System File");
        return false;
    }

    File file = LittleFS.open(path, "w");
    if (!file)
    {
        Serial.printf("Error ao abrir o arquivo \"%s\" para escrutar", path);
        return false;
    }

    if (!file.print(content))
    {
        Serial.printf("Erro ao gravar conteudo em \"%s\"", path);
        file.close();
        return false;
    }

    file.close();
    return true;
}
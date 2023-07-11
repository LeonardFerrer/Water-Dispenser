# Water Dispenser WiFi

# SUMÁRIO
* [Projeto](#projeto)
    * [Objetivos](#objetivos)  
* [Referências]()


# Projeto
Um dispensador de água também conhecido como bebouro, funciona com uma bomba para retirar água de um recipente e levar para uma torneira pela qual sairá a água facilitando por exemplo a tranferencia de água de um galão de 20L para garrafas menores sem a necessidade de esforço fisico para virar o galão e despejar água diretamente sobre as garrafas.

## Objetivos
* Desenvolver um equipamento IoT com conexão WiFi;
* Desenvolver um esquipamento que torne mais prático levar aguá de um recipente para outro, afim de diminui o esforço.
* Contruir um WebServer com ESP8266 pra disponibilizar micro serviços;
* Usar desenvolvimento WEB para criar páginas atráves de HTML, CSS, JavaScrypt;
* Utilizar o FileSystem para manipular e gerenciar arquivos em mémoria;
* Utilizar uma alternativa a biblioteca WiFi Manager.

## Manipulando body de uma request
Na troca de mensagem clientes/servidor foi utilizado requisições com o método POST, na qual os dados são contidos no body da request. Quando se é feito essa troca de mensagens por formularios, a biblioteca ESPAsyncWebServer fornece callbacks para lidar com elas ao se obter os parametros da mesma. Contudo algumas das requisições é feito a troca de Arquivos do tipo Json contidos dentro do body. Para isso devemos fazer algumas alterações na propria lib ESPAsyncWebServer para implementação de um novo callback para lidar com essas situações.

no arquivo **ESPAsyncWebServer.h**, adicione uma nova sobrecarga da função **.on()**:
```C++
AsyncCallbackWebHandler& on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, ArBodyHandlerFunction onBody);
```
Essa nova sobrecarga irá permitir que seja determinado as ações a serem exercutados pelo servidor quando um cliente acessar deterninado URL. As funções Handler padrões conseguem lidar bem com os parametros dentro de um request contudo para se ler uma request contendo um arquivo ou json no seu body faz-se necessario a implementação de um Handler dedicado para maninupar o conteudo do body. Com isso em mente a lib **ESPAsyncWebServer.h** já disponibilizar overloads da função **.on()** para manipular as request contudo, dentre as sobrecargas da Handler, somente uma irá ter o paramentro para manipular o body mas para usa-la faz-se necessario a implementação de uma Handler de Upload e por nesse projeto não haver necessidade disso, iremos criar uma sobrecarga nova da função **.on()**.

### Overload da função **.on()**:

Dentro do arquivo  **ESPAsyncWebServer.h**, após a declaração das demais sobrecargas da função **.on()** adicione mais esta sobrecarga.
```C++
AsyncCallbackWebHandler& on(
        const char* uri,                    // URL que se deseja chegar
        WebRequestMethodComposite method,   // Método da requisição (POST,GET...)
        ArRequestHandlerFunction onRequest, // função de callback para lidar com  requisição padrões
        ArBodyHandlerFunction onBody);      // função de callback para se lidar com os dados contido no body da requisição
```

já dentro do arquivo **WebServer.cpp** implemente a nova sobrecarga criada no passo anterior.
```C++
AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, ArBodyHandlerFunction onBody){
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  handler->onBody(onBody);
  addHandler(handler);
  return *handler;
}
```

# Referências

[Documentação de código](https://doxygen.nl/manual/commands.html)

[spiffs armazenamento de arquivos do esp32](https://blog.eletrogate.com/spiffs-armazenamento-de-arquivos-do-esp32/)

[display images esp32 esp8266 web server](https://randomnerdtutorials.com/display-images-esp32-esp8266-web-server/)

[Content Type](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types)

[Criando elemento DOM](http://devfuria.com.br/javascript/dom-create-element/)

[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

[XMLHttpRequest: readyState](https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState)

[esp8266 nodemcu vs-code platformio littlefs](https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/)

[Decodificando e Codificando JSON com Arduino ou ESP8266](https://randomnerdtutorials.com/decoding-and-encoding-json-with-arduino-or-esp8266/)

[esp8266-web-server](https://randomnerdtutorials.com/esp8266-web-server/)

[Tutorial array em Javascrypt](https://www.freecodecamp.org/portuguese/news/tutorial-de-arrays-de-objetos-em-javascript-como-criar-atualizar-e-percorrer-objetos-em-lacos-usando-metodos-de-array-do-js/)

[ESP8266 Servidor NodeMCU WebSocket: saídas de controle (Arduino IDE)](https://randomnerdtutorials.com/esp8266-nodemcu-websocket-server-arduino/)
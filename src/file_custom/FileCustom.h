#ifndef FILE_CUSTOM_H
#define FILE_CUSTOM_H

#include <Arduino.h>

String readFile( const char *path);
bool writeFile( const char *path, const char *content);

#endif

#ifndef STORAGE_H
#define STORAGE_H

#include "parser.h"

void set_value(char* key, char* value);
char* get_value(char* key);

char* set_command(RespCommand* command);
char* get_command(RespCommand* command);

#endif //STORAGE_H

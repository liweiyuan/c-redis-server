#include "storage.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

#define MAX_KEYS 100

typedef struct {
    char* key;
    char* value;
} KeyValuePair;

KeyValuePair storage[MAX_KEYS];
int key_count = 0;

void set_value(char* key, char* value) {
    if (key == NULL || value == NULL) return;
    for (int i = 0; i < key_count; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            free(storage[i].value);
            storage[i].value = strdup(value);
            return;
        }
    }
    if (key_count < MAX_KEYS) {
        storage[key_count].key = strdup(key);
        storage[key_count].value = strdup(value);
        key_count++;
    }
}

char* get_value(char* key) {
    if (key == NULL) return NULL;
    for (int i = 0; i < key_count; i++) {
        if (strcmp(storage[i].key, key) == 0) {
            return storage[i].value;
        }
    }
    return NULL;
}

char* set_command(RespCommand* command) {
    char* response = malloc(1024);
    if (command->argc != 3) {
        strcpy(response, "-ERR wrong number of arguments for 'set' command\r\n");
    } else {
        set_value(command->argv[1], command->argv[2]);
        strcpy(response, "+OK\r\n");
    }
    return response;
}

char* get_command(RespCommand* command) {
    char* response = malloc(1024);
    if (command->argc != 2) {
        strcpy(response, "-ERR wrong number of arguments for 'get' command\r\n");
    } else {
        char* value = get_value(command->argv[1]);
        if (value) {
            sprintf(response, "$%ld\r\n%s\r\n", strlen(value), value);
        } else {
            strcpy(response, "$-1\r\n");
        }
    }
    return response;
}

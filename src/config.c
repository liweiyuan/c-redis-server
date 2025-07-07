#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

Config load_config(const char* filename) {
    Config config = {0, 0, 0}; // Initialize with default values
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening config file");
        // Set default values if file not found
        config.log_level = 1; // INFO
        config.max_clients = 10;
        config.port = 6379;
        return config;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key != NULL && value != NULL) {
            // Trim whitespace from key and value
            while (*key == ' ' || *key == '\t') key++;
            char *end = key + strlen(key) - 1;
            while (end > key && (*end == ' ' || *end == '\t')) end--;
            *(end + 1) = '\0';

            while (*value == ' ' || *value == '\t') value++;
            end = value + strlen(value) - 1;
            while (end > value && (*end == ' ' || *end == '\t')) end--;
            *(end + 1) = '\0';

            if (strcmp(key, "log_level") == 0) {
                config.log_level = atoi(value);
            } else if (strcmp(key, "max_clients") == 0) {
                config.max_clients = atoi(value);
            } else if (strcmp(key, "port") == 0) {
                config.port = atoi(value);
            } else if (strcmp(key, "log_file_path") == 0) {
                config.log_file_path = strdup(value);
            }
        }
    }

    fclose(file);
    return config;
}

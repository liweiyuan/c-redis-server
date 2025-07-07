#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int log_level;
    int max_clients;
    int port;
    char *log_file_path;
} Config;

// Function to load configuration from a file
Config load_config(const char* filename);

#endif // CONFIG_H

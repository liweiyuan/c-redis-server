#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    DEBUG,
    INFO,
    WARN,
    ERROR
} LogLevel;

void set_log_level_custom(LogLevel level);
void set_log_file(const char* filename);
void close_log_file();
void log_message(LogLevel level, const char* format, ...);

#define LOG_DEBUG(format, ...) log_message(DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) log_message(INFO, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) log_message(WARN, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log_message(ERROR, format, ##__VA_ARGS__)

#endif // LOGGER_H

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

static LogLevel current_log_level = INFO; // Default log level
static FILE* log_file = NULL; // Declare log_file here

void set_log_file(const char* filename) {
    printf("Attempting to open log file: %s\n", filename);
    if (log_file != NULL) {
        fclose(log_file);
    }
    log_file = fopen(filename, "a"); // Open in append mode
    if (log_file == NULL) {
        perror("Error opening log file");
    }
}

void close_log_file() {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
}

void set_log_level_custom(LogLevel level) {
    current_log_level = level;
}

void log_message(LogLevel level, const char* format, ...) {
    if (level < current_log_level) {
        return;
    }

    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    const char* level_str;
    switch (level) {
        case DEBUG:
            level_str = "DEBUG";
            break;
        case INFO:
            level_str = "INFO";
            break;
        case WARN:
            level_str = "WARN";
            break;
        case ERROR:
            level_str = "ERROR";
            break;
        default:
            level_str = "UNKNOWN";
            break;
    }

    printf("[%s] [%s] ", buffer, level_str);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");

    if (log_file != NULL) {
        fprintf(log_file, "[%s] [%s] ", buffer, level_str);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        //fflush(log_file); // Ensure log is written to file immediately
    }
}


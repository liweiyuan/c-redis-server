#include "logger.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static LogLevel current_log_level = INFO; // Default log level
static FILE *log_file = NULL;             // Declare log_file here
static pthread_t flush_thread;
static int flush_thread_running = 0;
static int stop_flush_thread = 0;
static pthread_mutex_t log_file_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *flush_thread_func(void *arg) {
  while (!stop_flush_thread) {
    sleep(1);
    pthread_mutex_lock(&log_file_mutex);
    if (log_file != NULL) {
      fflush(log_file);
    }
    pthread_mutex_unlock(&log_file_mutex);
  }
  return NULL;
}

void set_log_file(const char *filename) {
  printf("Attempting to open log file: %s\n", filename);
  pthread_mutex_lock(&log_file_mutex);
  if (log_file != NULL) {
    fclose(log_file);
    log_file = NULL;
  }
  log_file = fopen(filename, "a"); // Open in append mode
  pthread_mutex_unlock(&log_file_mutex);
  if (log_file == NULL) {
    perror("Error opening log file");
  } else if (!flush_thread_running) {
    stop_flush_thread = 0;
    if (pthread_create(&flush_thread, NULL, flush_thread_func, NULL) == 0) {
      flush_thread_running = 1;
    } else {
      perror("Failed to start log flush thread");
    }
  }
}

void close_log_file() {
  if (flush_thread_running) {
    stop_flush_thread = 1;
    pthread_join(flush_thread, NULL);
    flush_thread_running = 0;
  }
  pthread_mutex_lock(&log_file_mutex);
  if (log_file != NULL) {
    fclose(log_file);
    log_file = NULL;
  }
  pthread_mutex_unlock(&log_file_mutex);
}

void set_log_level_custom(LogLevel level) { current_log_level = level; }

void log_message(LogLevel level, const char *format, ...) {
  if (level < current_log_level) {
    return;
  }

  time_t timer;
  char buffer[26];
  struct tm *tm_info;

  time(&timer);
  tm_info = localtime(&timer);

  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  const char *level_str;
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

  pthread_mutex_lock(&log_file_mutex);
  if (log_file != NULL) {
    fprintf(log_file, "[%s] [%s] ", buffer, level_str);
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fprintf(log_file, "\n");
    // fflush(log_file); // 由后台线程定时刷新
  }
  pthread_mutex_unlock(&log_file_mutex);
}

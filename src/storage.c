#include "storage.h"
#include "logger.h"
#include "parser.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYS 100

typedef struct {
  char *key;
  char *value;
} KeyValuePair;

KeyValuePair storage[MAX_KEYS];
int key_count = 0;

// 读写锁保护存储，允许多个读操作并发，写操作独占
static pthread_rwlock_t storage_rwlock = PTHREAD_RWLOCK_INITIALIZER;

void set_value(char *key, char *value) {
  if (key == NULL || value == NULL)
    return;

  // 获取写锁，独占访问存储
  if (pthread_rwlock_wrlock(&storage_rwlock) != 0) {
    LOG_ERROR("Failed to acquire write lock for storage");
    return;
  }

  // 查找是否已存在该key
  for (int i = 0; i < key_count; i++) {
    if (strcmp(storage[i].key, key) == 0) {
      free(storage[i].value);
      storage[i].value = strdup(value);
      pthread_rwlock_unlock(&storage_rwlock);
      LOG_DEBUG("Updated existing key: %s", key);
      return;
    }
  }

  // 添加新的键值对
  if (key_count < MAX_KEYS) {
    storage[key_count].key = strdup(key);
    storage[key_count].value = strdup(value);
    key_count++;
    LOG_DEBUG("Added new key: %s (total keys: %d)", key, key_count);
  } else {
    LOG_WARN("Storage full, cannot add key: %s", key);
  }

  pthread_rwlock_unlock(&storage_rwlock);
}

char *get_value(char *key) {
  if (key == NULL)
    return NULL;

  // 获取读锁，允许多个读操作并发
  if (pthread_rwlock_rdlock(&storage_rwlock) != 0) {
    LOG_ERROR("Failed to acquire read lock for storage");
    return NULL;
  }

  char *result = NULL;
  for (int i = 0; i < key_count; i++) {
    if (strcmp(storage[i].key, key) == 0) {
      // 复制值以避免在释放锁后访问共享内存
      result = strdup(storage[i].value);
      LOG_DEBUG("Found key: %s", key);
      break;
    }
  }

  pthread_rwlock_unlock(&storage_rwlock);

  if (result == NULL) {
    LOG_DEBUG("Key not found: %s", key);
  }

  return result;
}

char *set_command(RespCommand *command) {
  // Allocate response buffer based on actual need rather than fixed size
  const char *err_msg = "-ERR wrong number of arguments for 'set' command\r\n";
  const char *ok_msg = "+OK\r\n";

  if (command->argc != 3) {
    char *response = malloc(strlen(err_msg) + 1);
    if (response == NULL) {
      LOG_ERROR("Failed to allocate memory for response");
      return strdup("-ERR internal server error\r\n");
    }
    strcpy(response, err_msg);
    return response;
  } else {
    set_value(command->argv[1], command->argv[2]);
    char *response = malloc(strlen(ok_msg) + 1);
    if (response == NULL) {
      LOG_ERROR("Failed to allocate memory for response");
      return strdup("-ERR internal server error\r\n");
    }
    strcpy(response, ok_msg);
    return response;
  }
}

char *get_command(RespCommand *command) {
  const char *err_msg = "-ERR wrong number of arguments for 'get' command\r\n";
  const char *nil_msg = "$-1\r\n";

  if (command->argc != 2) {
    char *response = malloc(strlen(err_msg) + 1);
    if (response == NULL) {
      LOG_ERROR("Failed to allocate memory for response");
      return strdup("-ERR internal server error\r\n");
    }
    strcpy(response, err_msg);
    return response;
  }

  char *value = get_value(command->argv[1]);
  if (value) {
    // 计算响应所需的确切大小: $长度\r\n值\r\n
    size_t value_len = strlen(value);
    size_t len_digits = snprintf(NULL, 0, "%zu", value_len);
    size_t response_size =
        1 + len_digits + 2 + value_len + 2 + 1; // $, 长度, \r\n, 值, \r\n, \0

    char *response = malloc(response_size);
    if (response == NULL) {
      LOG_ERROR("Failed to allocate memory for response");
      free(value);
      return strdup("-ERR internal server error\r\n");
    }

    snprintf(response, response_size, "$%zu\r\n%s\r\n", value_len, value);
    free(value); // 释放get_value返回的副本
    return response;
  } else {
    char *response = malloc(strlen(nil_msg) + 1);
    if (response == NULL) {
      LOG_ERROR("Failed to allocate memory for response");
      return strdup("-ERR internal server error\r\n");
    }
    strcpy(response, nil_msg);
    return response;
  }
}

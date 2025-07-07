#include "parser.h"
#include "logger.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_COMMANDS 10

CommandEntry command_table[MAX_COMMANDS];
int command_table_size = 0;

// Helper function to parse a bulk string from the buffer
char *parse_bulk_string(char **buffer) {
  if (**buffer != '$')
    return NULL;
  (*buffer)++; // Skip '$'
  char *end_ptr;
  long length = strtol(*buffer, &end_ptr, 10);
  if (length == -1) {
    *buffer = end_ptr + 2; // Skip "-1\r\n"
    return NULL;           // Null bulk string
  }
  *buffer = end_ptr + 2; // Skip "\r\n"
  char *value = malloc(length + 1);
  strncpy(value, *buffer, length);
  value[length] = '\0';
  *buffer += length + 2; // Skip value and "\r\n"
  return value;
}

RespCommand parse_command(char *buffer) {
  RespCommand cmd = {0, NULL};
  if (buffer[0] != '*')
    return cmd;

  char *ptr = buffer + 1;
  char *end_ptr;
  cmd.argc = strtol(ptr, &end_ptr, 10);
  ptr = end_ptr + 2; // Skip "\r\n"

  if (cmd.argc > 0) {
    cmd.argv = malloc(sizeof(char *) * cmd.argc);
    for (int i = 0; i < cmd.argc; i++) {
      cmd.argv[i] = parse_bulk_string(&ptr);
    }
  }
  return cmd;
}

void free_command(RespCommand *command) {
  if (command->argv) {
    for (int i = 0; i < command->argc; i++) {
      if (command->argv[i]) {
        free(command->argv[i]);
      }
    }
    free(command->argv);
  }
}

void register_command(const char *name, redis_command_func func) {
  if (command_table_size < MAX_COMMANDS) {
    command_table[command_table_size].name = name;
    command_table[command_table_size].func = func;
    command_table_size++;
  } else {
    fprintf(stderr, "Command table full, cannot register %s\n", name);
  }
}

char *execute_command(RespCommand *command) {
  char *response = malloc(1024);
  if (command->argc == 0 || command->argv[0] == NULL) {
    strcpy(response, "-ERR invalid command\r\n");
    return response;
  }

  for (int i = 0; i < command_table_size; i++) {
    if (strcasecmp(command->argv[0], command_table[i].name) == 0) {
      return command_table[i].func(command);
    }
  }

  sprintf(response, "-ERR unknown command '%s'\r\n", command->argv[0]);
  return response;
}

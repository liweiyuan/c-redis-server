#ifndef PARSER_H
#define PARSER_H

typedef struct {
    int argc;
    char** argv;
} RespCommand;

// Function pointer types for commands
typedef char* (*redis_command_func)(RespCommand* command);

// Structure to hold command name and its corresponding function pointer
typedef struct {
    const char* name;
    redis_command_func func;
} CommandEntry;

// Global command table
extern CommandEntry command_table[];
extern int command_table_size;

RespCommand parse_command(char* buffer);
char* execute_command(RespCommand* command);
void free_command(RespCommand* command);
void register_command(const char* name, redis_command_func func);

#endif //PARSER_H
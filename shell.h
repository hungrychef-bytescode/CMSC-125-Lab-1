#ifndef SHELL_H     
#define SHELL_H

#include <stdbool.h> //for bool

#define MAX_ARGS 256
#define MAX_JOBS 100

typedef struct {
    char *command;
    char *args[MAX_ARGS];
    char *input_file;
    char *output_file;
    bool append;
    bool background;
} Command;

void tokenize(char *input, char *tokens[]);
Command parse_command(char *tokens[]);
void free_command(Command *cmd);
void executor(Command *cmd);


#endif
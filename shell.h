#ifndef SHELL_H     
#define SHELL_H

#include <stdbool.h> //for bool
#include <sys/types.h>

#define MAX_ARGS 256
#define MAX_JOBS 256

typedef struct {
    char *command;
    char *args[MAX_ARGS];
    char *input_file;
    char *output_file;
    bool append;
    bool background;
} Command;

typedef struct {
    int id;
    pid_t pid;
    char command[256];
} Job;

void tokenize(char *input, char *tokens[]);
Command parse_command(char *tokens[]);
void free_command(Command *cmd);
int executor(Command *cmd);
int built_in_commands(Command *cmd);
int external_commands(Command *cmd);
void cleanup_background_jobs(void);
void exit_cleanup(void);

#endif
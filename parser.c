#include <stdio.h> //for printf
#include <stdlib.h> //for free
#include <string.h> //for strdup - duplicate string
#include "shell.h"

/*
parse_command func -> takes array of pointers to tokens and constructs Command struct
-- command name, args, input/output redirection, background flag
*/
Command parse_command(char *tokens[]) {
    Command cmd = {0};
    int arg = 0;

    if (tokens == NULL || tokens[0] == NULL) return cmd;

    cmd.args[arg] = strdup(tokens[0]);
    if (!cmd.args[arg]) return (Command){0};

    cmd.command = strdup(tokens[0]);            // separate allocation so free_command can free both independently
    if (!cmd.command) { free(cmd.args[0]); return (Command){0}; }
    arg++;

    for (int i = 1; tokens[i] != NULL; i++) {
        char *next = tokens[i+1];                     //check next token

        // Handle redirections WITHOUT spaces (e.g. ">file", ">>file", "<file")
        char *op = NULL;
        char *attached_file = NULL;
        if (strncmp(tokens[i], ">>", 2) == 0 && tokens[i][2] != '\0') {
            op = ">>"; attached_file = tokens[i] + 2;
        } else if (tokens[i][0] == '>' && tokens[i][1] != '>' && tokens[i][1] != '\0') {
            op = ">";  attached_file = tokens[i] + 1;
        } else if (tokens[i][0] == '<' && tokens[i][1] != '\0') {
            op = "<";  attached_file = tokens[i] + 1;
        }

        if (op && attached_file) {
            if (*attached_file == '\0') {
                fprintf(stderr, "Error: Empty Filename\n");
                free_command(&cmd);
                return (Command){0};
            }
            if (strcmp(op, "<") == 0) {
                free(cmd.input_file);                   // free previous if duplicate
                cmd.input_file = strdup(attached_file);
                if (!cmd.input_file) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
            } else if (strcmp(op, ">") == 0) {
                free(cmd.output_file);                  // free previous if duplicate
                cmd.output_file = strdup(attached_file);
                if (!cmd.output_file) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
            } else if (strcmp(op, ">>") == 0) {
                free(cmd.output_file);                  // free previous if duplicate
                cmd.output_file = strdup(attached_file);
                if (!cmd.output_file) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
                cmd.append = true;
            }
            continue;
        }

        if (strcmp(tokens[i], "<") == 0) {              //input redirection
            if (!next || next[0] == '\0'){
                fprintf(stderr, "Error: No Input File\n");
                free_command(&cmd);
                return (Command){0};
            }
            free(cmd.input_file);                       // free previous if duplicate
            cmd.input_file = strdup(next);
            if (!cmd.input_file) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
            i++;
            continue;
        } else if (strcmp(tokens[i], ">") == 0) {       //output redirection
            if (!next || next[0] == '\0'){
                fprintf(stderr, "Error: No Output File\n");
                free_command(&cmd);
                return (Command){0};
            }
            free(cmd.output_file);                      // free previous if duplicate
            cmd.output_file = strdup(next);
            if (!cmd.output_file) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
            i++;
            continue;
        } else if (strcmp(tokens[i], ">>") == 0) {      //append output redirection
            if (!next || next[0] == '\0'){
                fprintf(stderr, "Error: No Output File\n");
                free_command(&cmd);
                return (Command){0};
            }
            free(cmd.output_file);                      // free previous if duplicate
            cmd.output_file = strdup(next);
            if (!cmd.output_file) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
            cmd.append = true;
            i++;
            continue;
        } else if (strcmp(tokens[i], "&") == 0) {       //background process
            if (tokens[i+1]) {
                fprintf(stderr, "Error: & Must Be Last Token\n");
                free_command(&cmd);
                return (Command){0};
            }
            cmd.background = true;
        } else {
            if (arg >= MAX_ARGS - 1) {
                fprintf(stderr, "Error: Too Many Args\n");
                free_command(&cmd);
                return (Command){0};
            }
            cmd.args[arg] = strdup(tokens[i]);
            if (!cmd.args[arg]) { fprintf(stderr, "Error: Memory Allocation Failed\n"); free_command(&cmd); return (Command){0}; }
            arg++;
        }
    }
    cmd.args[arg] = NULL; //null-terminate args array
    return cmd;
}

void free_command(Command *cmd) {
    for (int i = 0; cmd->args[i] != NULL; i++) { 
        free(cmd->args[i]);
        cmd->args[i] = NULL;
    }
    if (cmd->input_file) {
        free(cmd->input_file);
        cmd->input_file = NULL;
    }
    if (cmd->output_file) {
        free(cmd->output_file);
        cmd->output_file = NULL;
    }
    if (cmd->command) {
        free(cmd->command);                     // separate allocation, free independently
        cmd->command = NULL;
    }
}
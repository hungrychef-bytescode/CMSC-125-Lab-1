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
    
    cmd.command = cmd.args[arg]; 
    arg++;

    for (int i = 1; tokens[i] != NULL; i++) {
        char *next = tokens[i+1];                     //check next token
        if (strcmp(tokens[i], "<") == 0) {      //input redirection
            if (!next){
                printf("Error: no input file");
                free_command(&cmd);
                return cmd;
            }
            cmd.input_file = strdup(next);
            i++;
            continue;
        } else if (strcmp(tokens[i], ">") == 0) {  //output redirection
            if (!next){
                printf("Error: no output file");
                free_command(&cmd);
                return cmd;
            }
            cmd.output_file = strdup(next);
            i++;
            continue;
        } else if (strcmp(tokens[i], ">>") == 0) { //append output redirection
            if (!next){
                printf("Error: no output file");
                free_command(&cmd);
                return cmd;
            }
            cmd.output_file = strdup(next);
            cmd.append = true;
            i++;
            continue;
        } else if (strcmp(tokens[i], "&") == 0) {   //background process
            
             if (tokens[i+1]) {
                printf("Error: & must be last token\n");
                free_command(&cmd);
                return cmd;
             }
             cmd.background = true;
        } else {
            if (arg >= MAX_ARGS - 1) {
                printf("Error: too many args\n");
                free_command(&cmd);
                return cmd;
            }
            cmd.args[arg++] = strdup(tokens[i]);
        }
    }
    cmd.args[arg] = NULL; //null-terminate args array
    return cmd;
}

void free_command(Command *cmd) {
    for (int i = 0; cmd->args[i] != NULL; i++) { 
        free(cmd->args[i]);
        cmd-> args[i] = NULL;
    }
    if (cmd->input_file) {
        free(cmd->input_file);
        cmd->input_file = NULL;
    }

    if (cmd->output_file) {
        free(cmd->output_file);
        cmd->output_file = NULL;
    }
    
    cmd->command = NULL;
}
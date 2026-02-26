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

    //should u check if null -> tokenize null is already handled main loop

    cmd.command = strdup(tokens[0]);            //1st token - command name
    cmd.args[arg++] = strdup(tokens[0]);        //1st arg is command

    for (int i = 1; tokens[i] != NULL; i++) {
        char *next = tokens[i+1];                     //check next token
        if (strcmp(tokens[i], "<") == 0) {      //input redirection
            if (!next){
                printf("Error: no input file");
                return cmd;
            }
            cmd.input_file = strdup(next);
            i++;
            continue;
        } else if (strcmp(tokens[i], ">") == 0) {  //output redirection
            if (!next){
                printf("Error: no output file");
                return cmd;
            }
            cmd.output_file = strdup(next);
            i++;
            continue;
        } else if (strcmp(tokens[i], ">>") == 0) { //append output redirection
            if (!next){
                printf("Error: no output file");
                return cmd;
            }
            cmd.output_file = strdup(next);
            cmd.append = true;
            i++;
            continue;
        } else if (strcmp(tokens[i], "&") == 0) {   //background process
            cmd.background = true;
        } else {
            cmd.args[arg++] = strdup(tokens[i]);
        }
    }
    cmd.args[arg] = NULL; //null-terminate args array
    return cmd;
}

void free_command(Command *cmd) {
    if (cmd->command) free(cmd->command);
    for (int i = 0; cmd->args[i] != NULL; i++) free(cmd->args[i]);
    if (cmd->input_file) free(cmd->input_file);
    if (cmd->output_file) free(cmd->output_file);
}

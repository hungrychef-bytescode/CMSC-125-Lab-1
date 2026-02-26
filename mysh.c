//mysh.c  simple shell implementation for CMSC 125 lab 1
#include <stdio.h>      //for printf, fgets
#include <stdlib.h>     //for free
#include <string.h>     //for strcspn, strdup
#include "shell.h"      //for Command struct and parse_cmd function

#define MAX_INPUT 1000

/*
main function for the shell
entry point and used for the interactive loop
*/
int main() {

    char input[MAX_INPUT];
    char *tokens[MAX_ARGS];

    while (1){
        printf("mysh> ");

        if (fgets(input, MAX_INPUT, stdin) == NULL) break;              //exit on error.

        input[strcspn(input, "\n")] = '\0';                             //remove excess newline

        char *input_dup = strdup(input);                                //duplicate input for tokenization

        tokenize(input_dup, tokens);                                    //tokenize input to tokens arr

        if (tokens[0] == NULL) {                                        //if empty, prompt again
            free(input_dup);
            continue;
        }

        Command cmd = parse_command(tokens);
        printf("Command: %s\n", cmd.command);
        for (int i = 0; cmd.args[i] != NULL; i++) {
            printf("Arg[%d]: %s\n", i, cmd.args[i]);
        }

        if (cmd.input_file)
            printf("Input redirection: %s\n", cmd.input_file);

        if (cmd.output_file) {
            printf("Output redirection: %s\n", cmd.output_file);
            printf("Append mode: %s\n", cmd.append ? "true" : "false");
        }

        if (cmd.background)
            printf("Background: true\n");  
        free(input_dup);                                                //free input dup string
        free_command(&cmd);  
        }
        // free_command(&cmd);
    return 0;
}

        

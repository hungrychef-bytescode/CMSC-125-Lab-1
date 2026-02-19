//mysh.c  simple shell implementation for CMSC 125 lab 1

#include <stdio.h>      //for printf, fgets
#include <string.h>     //for strcspn
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

        if (fgets(input, MAX_INPUT, stdin) == NULL) break;              //exit on error. add printf
        printf("Input: %s", input);

        input[strcspn(input, "\n")] = '\0';                             //remove excess newline

        tokenize(input, tokens);

        if (tokens[0] == NULL) continue;                               //if no input, continue loop

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
        }
        // free_command(&cmd);
    return 0;
}

        

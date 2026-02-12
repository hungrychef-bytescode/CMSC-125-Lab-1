//mysh.c  simple shell implementation for CMSC 125 lab 1

//to run: 1) gcc mysh.c -o mysh
//2) ./mysh
//next -> create makefile and parser

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

    while (1){
        Command cmd;  // Assuming Command is defined in shell.h
        printf("mysh> ");

        if (fgets(input, MAX_INPUT, stdin) == NULL) break;              //exit on EOF
        printf("Input: %s", input);

        input[strcspn(input, "\n")] = '\0';                   
              //remove newline

        if (parse_command(input, &cmd) != 0) {
            continue;  // Empty or invalid input
        }

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
    return 0;
}
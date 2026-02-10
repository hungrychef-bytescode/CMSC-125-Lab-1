//mysh.c  simple shell implementation for CMSC 125 lab 1

//to run: 1) gcc mysh.c -o mysh
//2) ./mysh
//next -> create makefile and parser

#include <stdio.h>      //for printf, fgets

#define MAX_INPUT 1000


/*
main function for the shell
entry point and used for the interactive loop
*/
int main() {

    char input[MAX_INPUT];

    while (1){
        printf("mysh> ");

        fgets(input, MAX_INPUT, stdin);
        printf("Input: %s", input);
    }
    return 0;
}
#include <stdio.h>      //for printf, fgets
#include <stdlib.h>     //for exit
#include <string.h>     //for strcmp
#include <unistd.h> //for fork, execvp, getcwd
#include <fcntl.h> //for open
#include <sys/wait.h> //for waitpid
#include "shell.h"    

static pid_t background_jobs[MAX_JOBS];     //storage for background job PIDs
static int job_count = 0;


//to do: may ginareturn dapat whether successful or not etc. 
//perror -> print syscall error message
void executor(Command *cmd){

     if (cmd == NULL || cmd->command == NULL)
        return;

    // built-in commands
    if (strcmp(cmd->command, "exit") == 0) {            //exit shell
        printf("exiting shell......\n");
        exit(0);
    }

    if (strcmp(cmd->command, "cd") == 0) {              //change directory
        if (cmd->args[1] == NULL) {
            fprintf(stderr, "cd: requires an argument\n");
        } else if (chdir(cmd->args[1]) == 0) {
            printf("changed directory");
        } else {
            perror("failed to change directory");
        }
        return;
    }

    if (strcmp(cmd->command, "pwd") == 0) {             //print current working directory
        char cwd[1024];     
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("pwd: %s\n", cwd);
        } else {
            perror("failed to get current directory");
        }
        return;
    }

    //external commands
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    } else if (pid == 0) { //child process
        if (cmd->input_file){
            int file = open(cmd->input_file, O_RDONLY);
            if (file < 0) {
                perror("open input file");
                exit(1);
            }
            dup2(file, STDIN_FILENO);    //redirect stdin to file
            close(file);                 //close original fd
        }

        if (cmd->output_file) {        //open for writing, create if doesn't exist, truncate or append based on cmd->append
            int flags = O_WRONLY | O_CREAT;  //write only, create if not exist
            if (cmd->append) {
                flags |= O_APPEND;      //append
            } else {
                flags |= O_TRUNC;     //replace entire file
            }
            int file = open(cmd->output_file, flags, 0644);   //0644-file permissions
            if (file < 0) {
                perror("open output file");
                exit(1);
            }
            dup2(file, STDOUT_FILENO);
            close(file);
        }

        execvp(cmd->command, cmd->args);
        perror("exec faile");
        exit(127);     //cmd not found
    } else {  // Parent process
        if (!cmd->background) {
            int status;         
            waitpid(pid, &status, 0);       //wait for child to finish
            if (WIFEXITED(status)) {        //check if child exited
                int exit_code = WEXITSTATUS(status);    //get exit code
                if (exit_code != 0) {                   //if error: print exit code
                    printf("Command exited with code %d\n", exit_code);
            }
        }
        } else {
            printf("[%d] Started: %s (PID: %d)\n", job_count, cmd->command, pid); //print background job info
            background_jobs[job_count++] = pid; 
        }
    }
        }
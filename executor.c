#include <stdio.h>      //for printf, fgets
#include <stdlib.h>     //for exit
#include <string.h>     //for strcmp
#include <unistd.h> //for fork, execvp, getcwd
#include <fcntl.h> //for open
#include <sys/wait.h> //for waitpid
#include "shell.h"    

/*background job tracking*/
static Job background_jobs[MAX_JOBS];
static int job_count = 0;
static int next_job_id = 1;         //start job IDs from 1 same with real shell


//perror -> print syscall error message
int executor(Command *cmd){

    if (cmd == NULL || cmd->command == NULL)               //check for null command
        return 1;

    int built_in_result = built_in_commands(cmd);
    if (built_in_result != -1) {                         //check for built-in commands first (exit, cd, pwd) without forking
        return built_in_result;                                       //if built-in command executed, return result
    }
    

    //external commands
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
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
        perror("exec failed");
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
            background_jobs[job_count].id = next_job_id++;
            background_jobs[job_count].pid = pid;
            strcpy(background_jobs[job_count].command, cmd->command);
            background_jobs[job_count].is_running = true;
            job_count++;
        }
    }
    return 0;}

int built_in_commands(Command *cmd) {
    
    if (strcmp(cmd->command, "exit") == 0) {            //exit shell
        printf("exiting shell......\n");
        exit(0);
    }

    if (strcmp(cmd->command, "cd") == 0) {              //change directory
        if (cmd->args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
            return 1;    
        } else if (chdir(cmd->args[1]) == 0) {
            return 0;                                   //success
            // printf("changed directory");
        } else {
            perror("cd");
            return 1;
        }
    }

    if (strcmp(cmd->command, "pwd") == 0) {             //print current working directory
        char cwd[1024];     
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            return 0;                                   //success
        } else {
            perror("failed to get current directory");
            return 1;
        }     
    }
    return -1;                                          //not built-in command
}

void cleanup_background_jobs(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {

        for (int i = 0; i < job_count; i++) {

            if (background_jobs[i].pid == pid && background_jobs[i].is_running) {

                background_jobs[i].is_running = false;

                printf("[%d]   Done                    %s\n",
                       background_jobs[i].id,
                       background_jobs[i].command);
            }
        }
    }
}
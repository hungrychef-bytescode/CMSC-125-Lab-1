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

    int status = built_in_commands(cmd);                        //built-in commands first (exit, cd, pwd) without forking

    return (status != -1) ? status : external_commands(cmd);     //if not built-in, execute external command
}

int built_in_commands(Command *cmd) {
    //exit command
    if (strcmp(cmd->command, "exit") == 0) {
        printf("exiting shell......\n");
        exit(0);
    }

    //change directory command
    if (strcmp(cmd->command, "cd") == 0) {
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

    //print current working directory
    if (strcmp(cmd->command, "pwd") == 0) {
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

int external_commands(Command *cmd) {
    pid_t pid = fork();                     //create child process

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {                  //child process
        
        //input redirection
        if (cmd->input_file){
            int file = open(cmd->input_file, O_RDONLY);
            if (file < 0) {
                perror("input redirection");
                exit(1);
            }
            if (dup2(file, STDIN_FILENO) < 0) {    //redirect stdin to file
                perror("dup2 input redirection");
                close(file);
                exit(1);
            }
            close(file);                 //close original fd
        }

        //output redirection
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
            if (dup2(file, STDOUT_FILENO) < 0) {    //redirect stdout to file
                perror("dup2 output redirection");
                close(file);
                exit(1);
            }
            close(file);
        }

        execvp(cmd->command, cmd->args);
        perror(cmd->command);
        exit(127);     //cmd not found
    }
                                            
        //parent process
    if (!cmd->background) {

            int status;         
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid failed");
                return 1;
            }

            if (WIFEXITED(status)) {                            //check if child exited
                return WEXITSTATUS(status);                     //return child's exit code
            }

            if (WIFSIGNALED(status)) {                            //check if child terminated by signal
                printf("Process terminated by signal %d\n", WTERMSIG(status));
                return 1;
            }
            return 1;                                           //not normal exit
    } else {
            //background jobs
            if (job_count >= MAX_JOBS) {
                fprintf(stderr, "Maximum background jobs reached\n");
                return 1;
            }

            background_jobs[job_count].id = next_job_id++;
            background_jobs[job_count].pid = pid;
            background_jobs[job_count].is_running = true;

            //store command name for job listing. snprintf to avoid buffer overflow
            snprintf(background_jobs[job_count].command, sizeof(background_jobs[job_count].command), "%s", cmd->command);

            printf("[%d] %d\n", job_count, pid);                    //print background job info
            job_count++;

            return 0;
        }
    return 1;
}



void cleanup_background_jobs(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {

        for (int i = 0; i < job_count; i++) {

            if (background_jobs[i].pid == pid && background_jobs[i].is_running) {

                background_jobs[i].is_running = false;

                printf("[%d]   Done \t %s\n",
                       background_jobs[i].id,
                       background_jobs[i].command);
            }
        }
    }
}
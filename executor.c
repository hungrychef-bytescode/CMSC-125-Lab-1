#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h> //for kill
#include <errno.h>
#include "shell.h"    

/*background job tracking*/
static Job background_jobs[MAX_JOBS];
static int job_count = 0;
static int next_job_id = 1;         //start job IDs from 1 same with real shell

static Job finished_jobs[MAX_JOBS];
static int finished_job_count = 0;

int executor(Command *cmd){

    if (cmd == NULL || cmd->command == NULL) return 1;      // null commands

    int status = built_in_commands(cmd);                        //built-in commands first (exit, cd, pwd) without forking

    return (status != -1) ? status : external_commands(cmd);     //if not built-in, execute external command
}

//perror -> print syscall error message

int built_in_commands(Command *cmd) {
    //exit command
    if (strcmp(cmd->command, "exit") == 0) {
        exit_cleanup();
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
                _exit(1);
            }
            dup2(file, STDIN_FILENO);
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
                _exit(1);
            }
            dup2(file, STDOUT_FILENO);
            close(file);
        }

        execvp(cmd->command, cmd->args);
        if (errno == ENOENT) {
            fprintf(stderr, "%s: command not found \n", cmd->command);
        } else {
            perror(cmd->command);
        }
        _exit(127);     //cmd not found
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
                printf("process terminated by signal %d\n", WTERMSIG(status));
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

            //store command name for job listing. snprintf to avoid buffer overflow
            snprintf(background_jobs[job_count].command, sizeof(background_jobs[job_count].command), "%s", cmd->command);

            printf("[%d] %d\n", background_jobs[job_count].id, pid);                  //print background job info
            job_count++;

            return 0;
        }
    return 1;
}

/* cleanup_background_jobs function -> 
clean up background jobs that have finished running
uses waitpid with WNOHANG to check if each background job has completed without blocking the shell
if a job has finished, print a message and remove it from the background jobs list by shifting the remaining jobs left in the array
*/
void cleanup_background_jobs(void) {
    int status; //for waitpid

    for (int i = 0; i < job_count; ) {

        pid_t result = waitpid(background_jobs[i].pid, &status, WNOHANG);

        if (result > 0) {
            // process finished
            printf("[%d]   Done\t%s\t PID: %d\n", background_jobs[i].id, background_jobs[i].command, background_jobs[i].pid);

            if (finished_job_count < MAX_JOBS) { 
                finished_jobs[finished_job_count++] = background_jobs[i];
            }

            background_jobs[i] = background_jobs[job_count - 1];
            job_count--;   
        }
        else if (result == 0) {     //job still running, move to next job
            i++;     
        }
    }
}

/* exit_cleanup func -> 
terminate all background jobs when shell exits
checks if each job is still running and sends termination signal if so, then waits for it to finish
*/
void exit_cleanup(void) {

    for (int i = 0; i < job_count; i++) {
        if (kill(background_jobs[i].pid, 0) == 0) {
            kill(background_jobs[i].pid, SIGTERM);
            waitpid(background_jobs[i].pid, NULL, 0);
        }
    }
}
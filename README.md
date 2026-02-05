# CMSC 125 Lab 1: Unix Shell

## Group Members:
- Angel Mae Janiola
- Myra Verde

## Task Distribution

### Janiola – Parser & Command Structure

- Design parsing and tokenization logic  
- Detect operators (`<`, `>`, `>>`, `&`)  
- Separate command arguments from operators  
- Design `Command` data structure  
- Identify edge cases and malformed input scenarios  

### Verde – Execution & Process Management

- Implement process creation (`fork()`)  
- Execute commands using `execvp()`  
- Handle I/O redirection with `open()` and `dup2()`  
- Manage background processes using `waitpid()`  

### Both Members

- overall system architecture  
- Integrate parser and execution modules  
- Testing and debugging  
- Documentation and README

---

## Files

1. mysh.c
- main program
- main func, interactive shell loop, prompt display, read user input, call parser and executor, background cleanup and exit
2. parser.c
- tokenization of user input, detect special operators, separate command arguments, validation
3. executor.c
- built-in command handling, external command execution, i/o redirection, background process, zombie process prevention, error handling
4. shell.h
- function prototype for parser, executor, and built-in commands, shared constants and flags
5. documentation.md
- file for supported commands, examples and test cases, limitations
6. makefile
- automation file. targets all: compile the shell, clean: remove binaries and object files

---

## Problem Analysis

The shell must:

* Accept interactive user input in a loop
* Distinguish built-in vs external commands
* Execute external programs using POSIX process API
* Support I/O redirection:

  * Input (`<`)
  * Output overwrite (`>`)
  * Output append (`>>`)
* Support background execution (`&`)
* Prevent zombie processes
* Handle file and system call errors properly

### Key Risks

* Incorrect parsing of operators and arguments
* Built-in commands executed in child process
* File descriptor leaks
* Unreaped background processes
* Crashes from malformed input

---

## Solution Architecture

shell reads input and calls parser -> parse and build Command structure -> return Command to mysh.c -> mysh.c calls executor -> executes by handling built-in commands (parent) and fork for external commands

### Main Components

#### 1. Input Handler

* mysh interactive loop prints prompt `mysh>`
* Reads user input line
* Skips empty input

#### 2. Parser Module

* Tokenize input by whitespace
* Detect redirection operators and background flag
* Builds `Command` structure containing:

  * command name
  * argument list
  * input file
  * output file
  * append flag
  * background flag

#### 3. Built-in Command Handler

* `cd` using `chdir()`
* `pwd` using `getcwd()`
* `exit` to terminate shell

#### 4. Execution Module

* Creates child process using `fork()`
* Applies redirection using `open()` and `dup2()`
* Executes program using `execvp()`
* Parent waits unless background job

#### 5. Background Job Manager

* Tracks running background processes
* Cleans up completed jobs using `waitpid(..., WNOHANG)`

---

## Timeline Implementation

### Week 1

* Github repo
* Architecture design (problem analysis, solution architecture)
* Task division

### Week 2

* Core features (process execution and parsing)
* Built-in commands
* Basic I/O redirection

### Week 3

* Background job handling
* Edge case processing
* Error handling

### Week 4

* finish and finalize lacking features
* testing and bug fixing
* documentation
* laboratory defense

---

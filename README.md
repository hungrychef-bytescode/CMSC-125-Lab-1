
---

# CMSC 125 Lab 1: Unix Shell

## Group Members

* ANGEL MAY B JANIOLA
* MYRA S VERDE

---
## Recorded Test Case Compiled Output
[Click Here to Watch Test Case Compiled Output](https://drive.google.com/file/d/1JEfb5DFEqtjBml_DvvDuAnfS-ZHEDaja/view?usp=sharing)

---

# Compilation and Usage Instructions

## Requirements

* GCC compiler
* Unix-based system (Linux / macOS) or WSL
* POSIX-compliant environment

## Compilation

Using the provided Makefile:

```bash
make
```

This compiles all source files and generates the executable:

```
mysh
```

To remove compiled files:

```bash
make clean
```

## Running the Shell

```
./mysh
```

The shell will display the prompt:

```
mysh>
```

To exit:

```
exit
```

---

# Task Distribution

### Janiola – Parser & Command Structure

* Design parsing and tokenization logic
* Detect operators (`<`, `>`, `>>`, `&`)
* Separate command arguments from operators
* Design `Command` data structure
* Identify edge cases and malformed input scenarios

### Verde – Execution & Process Management

* Implement process creation (`fork()`)
* Execute commands using `execvp()`
* Handle I/O redirection with `open()` and `dup2()`
* Manage background processes using `waitpid()`

### Both Members

* Overall system architecture
* Integration of parser and execution modules
* Testing and debugging
* Documentation and README

---

# Files

1. **mysh.c** (Myra)

   * Main program
   * Interactive shell loop
   * Prompt display
   * Reads user input
   * Calls parser and executor
   * Background cleanup and exit

2. **parser.c** (Angel)

   * Tokenization of user input
   * Detect special operators
   * Separate command arguments
   * Validation

3. **executor.c** (Myra)

   * Built-in command handling
   * External command execution
   * I/O redirection
   * Background process handling
   * Zombie process prevention
   * Error handling

4. **shell.h** (Both)

   * Function prototypes for parser, executor, and built-in commands
   * Shared constants and flags

5. **documentation.md** (Both)

   * Supported commands
   * Examples and test cases
   * Limitations

6. **Makefile** (Angel)

   * `make` → compile the shell
   * `make clean` → remove binaries and object files

---

# Implemented Features

## 1. Interactive Shell

* Accepts user input in a continuous loop
* Displays custom prompt `mysh>`
* Skips empty input

## 2. Built-in Commands

* `cd` (uses `chdir()`)
* `pwd` (uses `getcwd()`)
* `exit`

## 3. External Command Execution

* Creates child processes using `fork()`
* Executes commands using `execvp()`
* Parent waits for foreground processes

## 4. I/O Redirection

* Input redirection `<`
* Output overwrite `>`
* Output append `>>`
* Implemented using `open()` and `dup2()`

## 5. Background Execution

* Supports `&` operator
* Parent does not block for background processes
* Uses `waitpid(..., WNOHANG)` to prevent zombie processes

## 6. Error Handling

* Detects malformed input
* Handles system call failures
* Prevents file descriptor leaks
* Ensures background processes are reaped

---

# Known Limitations or Bugs

* Does not support piping (`|`)
* Does not support multiple commands separated by `;`
* No support for environment variable expansion (e.g., `$HOME`)
* No quotation mark handling (`"` or `'`)
* No advanced job control (`jobs`, `fg`, `bg`)
* Limited validation for complex malformed inputs

---

# Problem Analysis

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

# Design Decisions and Architecture Overview

## Solution Architecture

Shell reads input and calls parser → parser builds `Command` structure → returns to `mysh.c` → `mysh.c` calls executor → executor handles built-in commands (parent) or forks for external commands.

```
Shell Loop (mysh.c)
        ↓
Parser Module (parser.c)
        ↓
Command Structure
        ↓
Executor Module (executor.c)
```

---

## Main Components

### 1. Input Handler

* Prints prompt `mysh>`
* Reads user input
* Skips empty lines

### 2. Parser Module

* Tokenizes input by whitespace
* Detects redirection operators and background flag
* Builds `Command` structure containing:

  * Command name
  * Argument list
  * Input file
  * Output file
  * Append flag
  * Background flag

### 3. Built-in Command Handler

Built-in commands are executed in the parent process because:

* `cd` must change the shell’s working directory
* `exit` must terminate the shell

### 4. Execution Module

* `fork()` creates child process
* Child applies redirection using `open()` and `dup2()`
* Executes program using `execvp()`
* Parent waits unless command runs in background

### 5. Background Job Manager

* Tracks background processes
* Uses `waitpid(..., WNOHANG)` to clean up completed jobs

---

# Timeline Implementation

### Week 1

* GitHub repository setup
* Architecture design
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

* Finalizing features
* Testing and bug fixing
* Documentation
* Laboratory defense

---

# Proof of Functionality

### Test Script Code (test_mysh.sh)

```bash
#!/bin/bash

PASS=0
FAIL=0

print_header() {
    echo ""
    echo "╔════════════════════════════════════════════════════╗"
    echo "║              mysh Test Suite                       ║"
    echo "╚════════════════════════════════════════════════════╝"
    echo ""
}

print_footer() {
    echo ""
    echo "╔════════════════════════════════════════════════════╗"
    echo "║                  SUMMARY                           ║"
    printf  "║  ✅ PASSED: %-2d   ❌ FAILED: %-2d                     ║\n" $PASS $FAIL
    echo "║                                                    ║"
    echo "║              Test Suite Complete                   ║"
    echo "╚════════════════════════════════════════════════════╝"
    echo ""
}

run_test() {
    local desc=$1
    shift
    local commands=("$@")

    echo "┌─────────────────────────────────────────────────────"
    echo "│ 🧪 TEST : $desc"
    echo "│ 📌 CMD  : ${commands[*]}"
    echo "│ 📤 OUT  :"
    echo "│"

    local input=""
    for cmd in "${commands[@]}"; do
        input+="$cmd"$'\n'
    done

    local output
    output=$(echo "$input" | ./mysh 2>&1 \
        | grep -v "^Input:" \
        | grep -v "^Command:" \
        | grep -v "^Arg\[" \
        | grep -v "^Output redirection" \
        | grep -v "^Append mode" \
        | grep -v "^Background" \
        | grep -v "^Input redirection" \
        | grep -v "mysh>")

    if [ -z "$output" ]; then
        echo "│   (no output)"
        FAIL=$((FAIL + 1))
        echo "│"
        echo "│ ❌ RESULT: FAIL"
    else
        echo "$output" | while IFS= read -r line; do
            echo "│   $line"
        done
        PASS=$((PASS + 1))
        echo "│"
        echo "│ ✅ RESULT: PASS"
    fi

    echo "└─────────────────────────────────────────────────────"
    echo ""
}

# ── Setup ────────────────────────────────────────────────────
echo -e "banana\nangel\ncherry\nmyra\nzebra" > unsorted.txt

print_header

run_test "pwd"                  "pwd"
run_test "ls -la"               "ls -la"
run_test "echo"                 "echo Hello World"
run_test "output redirection"   "ls -la > output.txt" "cat output.txt"
run_test "input redirection"    "ls -la > output.txt" "wc -l < output.txt"
run_test "append redirection"   'echo "some text" >> output.txt' "cat output.txt"
run_test "combined redirection" "sort < unsorted.txt > sorted.txt" "cat sorted.txt"
run_test "background process"   "sleep 10 &" "sleep 20 &"
run_test "nonexistent command"  "nonexistent_command"
run_test "cd and pwd"           "cd /tmp" "pwd"
run_test "exit"                 "exit"

print_footer
```

### Recorded Output

[Click Here to Watch Test Case Compiled Output](https://drive.google.com/file/d/1JEfb5DFEqtjBml_DvvDuAnfS-ZHEDaja/view?usp=sharing)

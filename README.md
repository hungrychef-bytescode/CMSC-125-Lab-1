# CMSC 125 Lab 1 — Unix Shell (`mysh`)

> A simplified Unix shell implementing process management, I/O redirection, and background execution using the POSIX API in C.

---

## Group Members

| Name | Role |
|------|------|
| Angel May B. Janiola | Parser & Command Structure |
| Myra S. Verde | Execution & Process Management |

---

## Compilation & Usage

### Requirements

- GCC compiler
- Unix-based system (Linux / macOS) or WSL
- POSIX-compliant environment

### Compile

```bash
make
```

Generates the `mysh` executable. To clean compiled files:

```bash
make clean
```

### Run

```bash
./mysh
```

The shell will display the prompt:

```
mysh>
```

To exit the shell:

```
mysh> exit
```

---

## ✅ Implemented Features

### 1. Interactive Shell
- Continuous input loop with custom `mysh>` prompt
- Skips empty and whitespace-only input

### 2. Built-in Commands

| Command | Description |
|---------|-------------|
| `cd <dir>` | Change directory using `chdir()` |
| `pwd` | Print working directory using `getcwd()` |
| `exit` | Terminate the shell |

### 3. External Command Execution
- Creates child processes using `fork()`
- Executes commands using `execvp()`
- Parent waits for foreground processes to finish

### 4. I/O Redirection

| Operator | Description |
|----------|-------------|
| `<` | Input redirection |
| `>` | Output redirection (overwrite) |
| `>>` | Output redirection (append) |

Implemented using `open()` and `dup2()`.

### 5. Background Execution
- Supports `&` operator
- Parent does not block for background processes
- Uses `waitpid(..., WNOHANG)` to prevent zombie processes

### 6. Error Handling
- Detects malformed input
- Handles system call failures gracefully
- Prevents file descriptor leaks
- Ensures background processes are reaped

---

## ⚠️ Known Limitations

- No pipe support (`|`)
- No multi-command support (`;`)
- No environment variable expansion (`$HOME`, etc.)
- No quote handling (`"` or `'`)
- No advanced job control (`jobs`, `fg`, `bg`)
- Limited validation for complex malformed inputs

---

## 🗂️ File Structure

```
.
├── mysh.c          # Main shell loop, prompt, input handling
├── parser.c        # Tokenization, operator detection, Command struct builder
├── executor.c      # Built-ins, fork/exec, I/O redirection, background jobs
├── shell.h         # Shared structs, constants, function prototypes
├── Makefile        # Build system
├── documentation.md
├── test_mysh.sh    # Test script
└── README.md
```

### File Responsibilities

**`mysh.c`** *(Myra)*
- Main program entry point
- Interactive shell loop and prompt display
- Calls parser and executor
- Background cleanup on exit

**`parser.c`** *(Angel)*
- Tokenizes user input by whitespace
- Detects `<`, `>`, `>>`, `&` operators
- Builds and returns `Command` structure
- Input validation

**`executor.c`** *(Myra)*
- Built-in command handling (`cd`, `pwd`, `exit`)
- External command execution via `fork()` + `execvp()`
- I/O redirection with `open()` + `dup2()`
- Background process tracking and zombie prevention

**`shell.h`** *(Both)*
- `Command` struct definition
- Shared constants (`MAX_ARGS`, `MAX_JOBS`)
- Function prototypes

**`Makefile`** *(Angel)*
- `make` — compile the shell
- `make clean` — remove binaries and object files

---

## 🏗️ Architecture Overview

```
User Input
    │
    ▼
Shell Loop (mysh.c)
    │
    ▼
Parser Module (parser.c)
    │  tokenize input, detect operators
    ▼
Command Structure
    │  { command, args[], input_file, output_file, append, background }
    ▼
Executor Module (executor.c)
    ├── Built-in? → run in parent process
    └── External? → fork() → apply redirections → execvp()
```

Built-in commands (`cd`, `exit`) **must** run in the parent process — `cd` changes the shell's own working directory, and `exit` terminates the shell itself.

---

## 📅 Development Timeline

| Week | Milestone |
|------|-----------|
| Week 1 | Repository setup, architecture design, task division |
| Week 2 | Core features — parsing, built-ins |
| Week 3 | Background job handling, external commands, edge cases, error handling |
| Week 4 | Reap zombie processes, finalization, testing, bug fixes, documentation, lab defense/recording |

---

> *"Code tells you how, comments tell you why."* — Jeff Atwood

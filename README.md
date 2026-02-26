# CMSC 125 Lab 1 — Unix Shell (`mysh`)

> A simplified Unix shell implementing process management, I/O redirection, and background execution using the POSIX API in C.

---

## Group Members

| Name | Role |
|------|------|
| Angel May B. Janiola | Parser & Command Structure |
| Myra S. Verde | Execution & Process Management |

---

## Recorded Output

▶️ [Click Here to Watch Test Case Compiled Output](https://drive.google.com/file/d/1JEfb5DFEqtjBml_DvvDuAnfS-ZHEDaja/view?usp=sharing)

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

## 🧪 Test Cases

### Test Script (`test_mysh.sh`)

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

# Setup
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

### Run the Test Script

```bash
make all
bash test_mysh.sh
```

---

## 📅 Development Timeline

| Week | Milestone |
|------|-----------|
| Week 1 | Repository setup, architecture design, task division |
| Week 2 | Core features — process execution, parsing, built-ins, basic I/O redirection |
| Week 3 | Background job handling, edge cases, error handling |
| Week 4 | Finalization, testing, bug fixes, documentation, lab defense |

---

> *"Code tells you how, comments tell you why."* — Jeff Atwood

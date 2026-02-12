### POSIX: Portable Operating System Interface
- set of standards by IEEE
- ensure compatibility and portability between different UNIX-like OS

`dup2()` -  duplicate a file descriptor
`WNOHANG` - return immediately if no child has exited
zombie process - process done executing but still in the process table because its parent havent yet reaad the exit status.
cd - change working directory of current shell exec env
chdir() - change current working directory of calling process
execvp() - execute v-vector p-path
waitpid() - 
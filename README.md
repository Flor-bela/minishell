*This project has been created as part of the 42 curriculum by omaestre, fda-roch.*

## Description
Minishell is a simplified Unix shell written in C, with the goal of replicating core features of Bash.
The main objective of this project is to understand how shells work by implementing command parsing, and execution.
The program handles user input in an interactive prompt, interprets commands, and executes them while using essential shell functionalities such as pipes, redirections, environment variable management, built-in commands and signals.

## Instructions
### Compilation:

To compile the project:
make	# Compile the project
make clean	# Remove object files
make fclean	# Remove object files and executable
make re		# Recompile everything

### Execution:

Launch the shell with:
./minishell

- Execution of binaries and system commands
- Built-in commands (cd, echo, pwd, export, unset, env, exit)
- Pipes (|)
- Input and output redirections (<, >, >>)
- Environment variable expansion ($VAR)
- Signal handling (Ctrl+C, Ctrl+D, Ctrl+\\)
- Error handling and exit status management

## Resources

As resources, we used mainly our project manual available in intra.

Other useful resources:
- Bash manual: https://www.gnu.org/software/bash/manual/
- Linux man pages (for specific functions, like man dup2, map fork, etc.)
- Graphic representation of the steps to take into account: https://www.figma.com/community/file/1269608787526815167/minishell-execution-part
- For pipes: https://unix.stackexchange.com/questions/574260/how-does-pipe-work-in-bash-commands
- Handling file descriptors: https://www.codequoi.com/en/handling-a-file-by-its-descriptor-in-c/
- The Linux Programming Interface (https://man7.org/tlpi/).

### AI Usage

AI tools were used as a support resource during the development of this project. 
Specifically for:

Helping naming functions; grammatical errors, especially for writing this manual.
Obtaining examples to test our own code and stress tests.

All implementation decisions, code writing, and final debugging were performed by us, though.
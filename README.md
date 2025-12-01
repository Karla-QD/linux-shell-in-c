# Shell-en-Linux

Operating Systems Course Project

The project consists of developing a Shell for Linux using the C programming language, with the following highlighted features:

1. Full Compatibility with Linux Commands:

The Shell is capable of executing any command native to the Linux operating system, providing a complete and functional interface for the user.

2. Command History:

It implements a history that stores the last 35 executed commands. Previous commands can be retrieved by entering a specific position followed by the exclamation mark, as in the example !n.

3. Background Command Execution:

It allows the execution of commands in the background using fork functions. To achieve this, the user places the command to be executed in the background followed by '&' and presses enter. Subsequently, other commands can be entered that will run in the foreground.

4. Pipe Support:

It enables the use of pipes to create data flows between commands. For example, executing ls | sort performs a pipe operation between the ls and sort commands.

This project not only provides a fully functional Shell but also represents a valuable and comprehensive learning opportunity. Through the implementation of fundamental concepts of operating systems, programming in C, and process management.

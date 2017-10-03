# CMPUT301 Assignment1

Name: Daniel Choi  
CCID: 1438979  

## Objectives
The objective of this assignment is to become familiar with
Unix function calls and process control.  

## Design Overview
-simple shell program
-parent process: a1shell
	-implements its own shell commands for cd, pwd, umask, done
	-for other commands, the process forks a child process that
	 overlays itself with Bash
	-calculates the total time elasped, user and system CPU times spent
	 by a1shell and the child process during processing of command
-child process: a1monitor
	-displays current date, average system load, the number of running
	 processes and the total number of procesess in the system.

## Project Status
The program has implemented all the requirements that are mentioned in
the assignment.

## Testing and Results

## Acknowledgments
Error handling code is based on the code of AWK programming language.
Many of the terminal commands that are recreated are based
on the [APUE 3/E]book.

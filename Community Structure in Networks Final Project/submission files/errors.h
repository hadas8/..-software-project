#ifndef STDLIB_H_
#include <stdlib.h>
#endif
#ifndef STDIO_H_
#include <stdio.h>
#endif

#ifndef ERRORS_H_
#define ERRORS_H_

/*
 * errors module:
 *
 * This module is designed to handle errors which require exiting the program.
 *
 * It supports one function:
 *
 * exit_prog	- Exit the program and print a message explaining the problem
 */

void exit_prog(unsigned int n);

#endif /* ERRORS_H_ */

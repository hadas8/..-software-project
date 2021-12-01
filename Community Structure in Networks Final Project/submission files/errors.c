#include "errors.h"


/*
 *Handles errors by printing a message and exiting the program.
 *
 *@param n: an unsigned integer indicating the type of error.
 */
void exit_prog(unsigned int n) {
	fprintf(stderr, "Exiting program: ");
	switch(n) {
	case 0:
		fprintf(stderr, "error opening file");
		break;
	case 1:
		fprintf(stderr, "error writing into file");
		break;
	case 2:
		fprintf(stderr, "error reading from file");
		break;
	case 3:
		fprintf(stderr, "error allocating dynamic memory");
		break;
	case 4:
		fprintf(stderr, "division by zero error");
		break;
	case 5:
		fprintf(stderr, "illegal input");
		break;
	case 6:
		fprintf(stderr, "infinite loop");
		break;
	}
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}


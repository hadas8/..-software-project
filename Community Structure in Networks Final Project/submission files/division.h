#ifndef ERRORS_H_
#include "errors.h"
#endif

#ifndef TEMP_VECTORS_H
#include "temp_vectors.h"
#endif

#ifndef DIVISION_H_
#define DIVISION_H_

/*
 * division module:
 *
 * This module implements the division algorithm (algorithm 2) and modularity maximization (algorithm 4).
 *
 * The module includes several functions, and only one of them (division) should be used by an external user:
 * division		- Implementation of algorithm 2 and 4.
 *
 * Therefore the rest of the functions are "private" (not included in the header file).
 */

void division(mat* B, unsigned int* index_vector, unsigned int group_size, double* div_vector, temp_vectors *temps);

#endif /* DIVISION_H_ */

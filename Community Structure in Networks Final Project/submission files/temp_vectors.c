#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include "temp_vectors.h"

/*
 * Frees all resources used by the temp_vectors struct.
 *
 * @param v: the temp_vectors struct.
 */
void free_vecs(struct _temp_vectors *v) {
	unsigned int *unmoved = v->unmoved;
	double **indices = v->indices;
	double *eigenvector = v->eigenvector;
	double *local = v->local;

	free(unmoved);
	free(indices);
	free(eigenvector);
	free(local);
	free(v);
}

/*
 * Initializes a temp_vectors struct.
 * This function is called exactly one time, before algorithm 3.
 *
 * @param n: the length of the vectors in the struct.
 *
 * @return: a pointer to the initialized struct.
 */
temp_vectors* allocate_temps(unsigned int n) {
	temp_vectors *temps;

	temps = (temp_vectors*)malloc(sizeof(temp_vectors));
	if (temps == NULL) {
		exit_prog(3);
	}

	temps->unmoved = (unsigned int*)malloc(sizeof(unsigned int) * n);
	if (temps->unmoved == NULL) {
		exit_prog(3);
	}

	temps->indices = (double **)malloc(sizeof(double*) * n);
	if (temps->indices == NULL) {
		exit_prog(3);
	}

	temps->eigenvector = (double*)malloc(sizeof(double) * n);
	if (temps->eigenvector == NULL) {
		exit_prog(3);
	}

	temps->local = (double*)malloc(sizeof(double) * n);
	if (temps->local == NULL) {
		exit_prog(3);
	}

	temps->free = free_vecs;

	return temps;
}



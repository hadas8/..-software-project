#ifndef ERRORS_H_
#include "errors.h"
#endif

#ifndef _SPMAT_H
#define _SPMAT_H

/*
 * spmat module:
 *
 * This module represents the adjacency matrix in form of a sparse matrix.
 *
 * It consists of the struct _spmat that represents the matrix itself, and the struct _list_node that implements
 * this representation by linked-lists.
 *
 * Fields in _list_node:
 * value				- The value of the element in the matrix.
 * column				- The column of the element.
 * next					- Pointer to the next node in the list.
 *
 * Fields in _spmat:
 * n					- The number of rows in the matrix.
 * private				- Implementation of the matrix by linked-lists.
 *
 * Functions in _spmat:
 * add_row_by_col		- Adds a row to the matrix.
 * free					- Frees all resources used by the matrix.
 * mult					- multiplies the matrix by a given vector.
 * get_row				- Extracts a row from the matrix.
 * sum_row				- Sums a row in the matrix.
 *
 * Additional functions in the module:
 * spmat_allocate_list	- Initializes a sparse matrix A.
 */

typedef struct _list_node {
	double value;
	unsigned int column;
	struct _list_node *next;
} list_node;

typedef struct _spmat {
	/* Matrix size (n*n) */
	unsigned int		n;

	/* Adds to A the i-th row, represented by vector row of length len*/
	void 	(*add_row_by_col)(struct _spmat *A, const unsigned int *row, unsigned int i, unsigned int len);

	/* Frees all resources used by A */
	void	(*free)(struct _spmat *A);

	/* Multiplies matrix A by vector v, into result (result is pre-allocated) */
	void	(*mult)(const struct _spmat *A, const double *v, double *result);

	/* Puts into res the i-th row of A*/
	void (*get_row)(struct _spmat *A, unsigned int i, double *res);

	/* Sums row i of A*/
	double (*row_sum)(struct _spmat *A, unsigned int i);

	/* Implementation by linked-list */
	void	*private;
} spmat;

/* Allocates a new linked-lists sparse matrix of size n */
spmat* spmat_allocate_list(unsigned int n);

#endif

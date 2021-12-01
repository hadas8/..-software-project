#ifndef ERRORS_H_
#include "errors.h"
#endif

#ifndef MAT_H_
#define MAT_H_
#ifndef _SPMAT_H_
#include "spmat.h"
#endif

/*
 * mat module:
 *
 * This module represents the matrix B (and its derivatives, such as B_hat).
 *
 * It consists of the struct _mat to represent the matrix, and several functions.
 *
 * Fields in _mat:
 * A				- Adjacency matrix represented as a sparse matrix.
 * k				- Vector of the degrees for each node in the matrix.
 * M				- The sum of all degrees in B.
 * k_sum			- The sum of node degrees in the current sub matrix (for matrix B: M == k_sum).
 * diag				- Vector containing the values of f_i for each row in the matrix (for matrix B, all values are 0).
 *
 * Functions in _mat:
 * free				- Frees all memory used by the matrix.
 * mult				- Multiplies the matrix by a given vector.
 * mult_by_dot		- Calculates the dot product of a given vector with a specific roe from the matrix.
 * add_row			- Adds a row to the matrix.
 * sum_row			- Calculates the sum of a row.
 * max_sum			- Finds the maximum row sum in the matrix.
 * add_row_list		- Adds a sub-row of B to the matrix (relevant for B_hat).
 *
 * Additional functions in the module:
 * allocate_mat		- Initializes the matrix B.
 * dot				- Calculates the dot product of two given vectors.
 * sub_mat			- Initializes B_hat.
 */


typedef struct _mat {
	/*sparse adjacency matrix represented by linked lists*/
	spmat *A;

	/*vector of degrees for each node*/
	double *k;

	/*sum of all degrees*/
	unsigned int M;

	/*sum of degrees in sub matrix*/
	unsigned int k_sum;

	/*vector of f_i values*/
	double *diag;

	/*free all the memory used by B*/
	void	(*free)(struct _mat *B);

	/*multiply B by vector v and save to a pre-allocated result vector*/
	void	(*mult)(const struct _mat *B, const double *v, double *result, double one_norm);

	/*calculates a dot product between a given vector and a specific row from the matrix */
	double	(*mult_by_dot)(struct _mat *B, double* vec, unsigned int i, double *row);

	/*add row i to B, called exactly n times*/
	void	(*add_row)(struct _mat *B, const unsigned int *row, unsigned int i, unsigned int len);

	/*sum row i of B, flag indicates if numbers are taken regularly or by abs value*/
	double	(*sum_row)(struct _mat *B, unsigned int i, unsigned int flag, double *row);

	/*return max sum of row, sum by abs value*/
	double	(*max_sum)(struct _mat *B, double *local);

	/*add sub-list as row i to B-hat, according to the group vector */
	void	(*add_row_list)(struct _mat *B, struct _list_node *row, unsigned int* index_vector, unsigned int i);

} mat;

/*allocate and initialize the matrix B of size n*n*/
mat* allocate_mat(unsigned int n);

/*calculate dot product of v1 (double) and v2 (unsigned int) of size n*/
/*maybe change v2 to double too so it applies for more scenarios?*/
double dot(const double *v1, double *v2, unsigned int n);

/* returns the matrix B[g]-hat, a sub matrix of B with a main diagonal shift vector */
mat* sub_mat(mat* B, unsigned int* index_vector, unsigned int group_size, double *local);

#endif /* MAT_H_ */

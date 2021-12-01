#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mat.h"
#include "spmat.h"

/*
 * Adds a row to the matrix B, and updates struct fields accordingly.
 * This function is called exactly n times (where n is the number of nodes).
 *
 * @param B: the pre-initialized matrix B to add a row to.
 * @param row: vector of indices with non-zero values (neighbors of node i in the graph).
 * @param i: the index of the row.
 * @param k: the degree of node i (num of elements in the vector row).
 */
void add_row_mat(struct _mat *B, const unsigned int *row, unsigned int i, unsigned int k) {
	spmat *sp = B -> A;
	sp -> add_row_by_col(sp, row, i, k);
	B->M += k;
	B->k_sum += k;
}

/*
 * Adds a sub-row of B to B_hat, according to the elements in the group for which B_hat is constructed.
 * This function is called exactly n_g times (where n_g is the number of nodes in the current group g).
 *
 * @param B: a pre-initialized B_hat matrix to add the row to.
 * @param row: a linked-list containing all neighbors of node i in the original graph.
 * @param index_vector: a vector of the indices that are in the group.
 * @param i: the index of the row.
 */
void add_row_mat_list(struct _mat *B, struct _list_node *row, unsigned int* index_vector, unsigned int i) {
	list_node **private = (list_node **) (B -> A -> private);
	list_node **head = private + i;
	unsigned int col = 0, *index = index_vector, curr_index;

	while(row) {
		curr_index = *index;
		if(curr_index < row -> column) {
			/* calculating column offset */
			index++;
			col++;
		}
		else {
			if(curr_index == row -> column) {
				/* adding node to list */
				*head = (list_node*)malloc(sizeof(list_node));
				if(*head == NULL) {
					exit_prog(3);
				}
				(*head) -> value = row -> value;
				(*head) -> column = col;
				head = &((*head) -> next);
			}
			row = row -> next;
		}
	}
	*head = NULL;
}

/*
 * Frees all resources used by a matrix.
 *
 * @param B: the matrix to free.
 */
void free_mat(struct _mat *B) {
	double *K = B -> k;
	double *Diag = B->diag;
	spmat *sp = B -> A;

	free(K);
	free(Diag);
	sp -> free(sp);
	free(B);
}

/*
 * Calculates the dot product of two vectors.
 *
 * @param v1: the first vector (transposed).
 * @param v2: the second vector.
 * @param n: the length of the vectors (same for both of them).
 *
 * @return: the dot product of the vectors.
 */
double dot(const double *v1, double *v2, unsigned int n) {
	double res = 0.0;
	double const *curr1;
	double *curr2;
	unsigned int i;

	curr1 = v1;
	curr2 = v2;

	for (i = 0; i < n; i++) {
		res += (*(curr1++)) * (*(curr2++));
	}
	return res;
}

/*
 * Multiplies a matrix by a vector into a result vector.
 *
 * @param B: the matrix to multiply.
 * @param v: the vector to multiply.
 * @param result: a pre-initialized vector that by the end of the function contains the result of the multiplication.
 * @param one_norm: the one norm calculated for the matrix (relevant only for matrix shifting, otherwise equals to 0).
 */
void mult_mat(const struct _mat *B, const double *v, double *result, double one_norm) {
	spmat *sp = B -> A;
	double *curr_res, value, *curr_diag;
	unsigned int i;
	double *curr_k;

	if (B -> M == 0) {
		exit_prog(4);
	}
	value = dot(v, B -> k, sp -> n);
	value = value / B -> M;
	sp -> mult(sp, v, result);

	curr_res = result;
	curr_k = B -> k;
	curr_diag = B -> diag;
	/*the result = Av - k_i*dot(k, v)/M - fIv + cIv */
	for (i = 0; i < sp -> n; i++) {
		*curr_res = *curr_res - (value * (*(curr_k++))) + (one_norm - *(curr_diag++)) * (*(v++));
		curr_res++;
	}
}

/*
 * Calculates dot product of a given vector and the i'th row of the matrix without the i'th element
 * this function is used to calculate the modularity after only changing one element in the vector
 *
 * @param B: the matrix from which the row is taken.
 * @param vec: vector to multiply.
 * @param i: the desired row from the matrix.
 * @param row: a pre-allocated vector to fill with the i-th row from the matrix.
 *
 * @return: the result of the dot product without the i-th element.
 */
double mult_by_dot_mat(struct _mat *B, double* vec, unsigned int i, double *row) {
	unsigned int j;
	spmat *sp = B -> A;
	double res1 = 0.0,  res2 = 0.0, *k = B -> k, k_i = 0;

	sp -> get_row(sp, i, row);

	for (j = 0; j < sp -> n; j++) {
		if(j != i) {
			res1 += (*row) * (*vec);
			res2 += (*k) * (*vec);
		} else {
			k_i = *k;
		}
		row++;
		vec++;
		k++;
	}
	res1 = res1 - (k_i / B -> M) * res2;
	return res1;
}

/*
 * Calculates the sum of a row in a matrix.
 *
 * @param B: the matrix to calculate from.
 * @param i: the index of the row.
 * @param flag: indicates how to calculate the sum.
 * 				can be either 0 - then the sum is calculated normally; or 1 - then it's calculated by abs value.
 * @param row: pre-allocated vector that will receive the content of the i-th row
 *
 * @return: the row sum.
 */
double	sum_row_mat(struct _mat *B, unsigned int i, unsigned int flag, double *row) {
	spmat *A = B -> A;
	double element, sum = 0, *diag = B -> diag, *curr;
	unsigned int j, n = A -> n;
	double *degs = B -> k;
	double deg_i = degs[i];

	if (B -> M == 0) {
		exit_prog(4);
	}
	if (flag == 0) {
		sum += A->row_sum(A, i) - (deg_i * B->k_sum)/B->M - diag[i];
	}
	else { /*sum the absolute values of a row*/
		A -> get_row(A, i, row);
		curr = row;
		for (j = 0; j < n; j++) {
			element = (B->M) * (*(curr++)) - (deg_i * (*(degs++))); /* A_ij - (k_i*k_j)/M */
			if (j == i) {
				element -= diag[i];
			}
			if (element < 0) {
				sum -= element;
			}
			else {
				sum += element;
			}
		}
	}

	return sum / B->M;
}

/*
 * Finds the maximum row sum in a matrix.
 *
 * @param B: the matrix to calculate from.
 * @param local: pre-allocated vector to be given to sum_row.
 *
 * @return: the maximum row sum.
 */
double max_sum_mat(struct _mat *B, double *local) {
	spmat *A = B -> A;
	unsigned int n = A -> n;
	unsigned int i;
	double max = 0.0, row_sum;

	for (i = 0; i < n; i++) {
		row_sum = B -> sum_row(B, i, 1, local);
		if (row_sum > max) {
			max = row_sum;
		}
	}
	return max;
}

/*
 * Initializes the symmetric matrix B.
 *
 * @param n: number of rows in the matrix.
 *
 * @return: a pointer to the initialized matrix.
 */
mat* allocate_mat(unsigned int n) {
	mat *B;

	B = (mat*)malloc(sizeof(mat));
	if (B == NULL) {
		exit_prog(3);
	}
	B -> A = spmat_allocate_list(n);
	B -> M = 0;
	B->k_sum = 0;
	B -> k = (double*)malloc(sizeof(double) * n);
	if (B -> k == NULL) {
		exit_prog(3);
	}
	B -> diag = (double*)calloc(n, sizeof(double));
	if (B -> diag == NULL) {
		exit_prog(3);
	}

	B -> add_row = add_row_mat;
	B -> mult = mult_mat;
	B -> mult_by_dot = mult_by_dot_mat;
	B -> free = free_mat;
	B -> sum_row = sum_row_mat;
	B -> max_sum = max_sum_mat;

	return B;
}

/*
 * Initializes a B_hat matrix for a sub-group of nodes.
 *
 * @param B: the original matrix B.
 * @param index_vector: a vector containing the indices of the nodes in the group.
 * @param group_size: number of elements in the group.
 * @param local: a pre-allocated vector to be given to sum_row.
 *
 * @return: a pointer to the initialized B_hat matrix.
 */
mat* sub_mat(mat* B, unsigned int* index_vector, unsigned int group_size, double *local) {
	unsigned int i, n, *p;
	double *k, *diag_iter, *deg, curr_deg;
	mat* B_hat;
	list_node** private = B -> A -> private;

	n = group_size;
	p = index_vector;
	k = B -> k;

	B_hat = allocate_mat(n);
	deg = B_hat -> k;
	B_hat -> add_row_list = add_row_mat_list;
	B_hat -> M = B -> M;

	for (i = 0; i < n; ++i) {
		curr_deg = k[*p++];
		*(deg++) = curr_deg;
		B_hat -> k_sum += (unsigned int)curr_deg;
	}

	p = index_vector;
	diag_iter = B_hat -> diag;
	for(i = 0; i < n; ++i) {
		B_hat -> add_row_list(B_hat, private[*p++], index_vector, i);
		*(diag_iter++) = B_hat -> sum_row(B_hat, i, 0, local);
	}

	return B_hat;
}

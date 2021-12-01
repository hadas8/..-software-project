#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spmat.h"

/*
 * Adds a row to sparse matrix A.
 *
 * This function is called exactly n times (where n is the number of rows in the matrix).
 *
 * @param A: the pre-initialized matrix to add a row to.
 * @param row: a vector of indices of columns with non-zero values.
 * @param i: the index of the row.
 * @param len: number of non-zero elements (length of row).
 */
void add_row_list_col(struct _spmat *A, const unsigned int *row, unsigned int i, unsigned int len) {
	list_node *head = NULL, *tail = NULL, **private = (list_node **) (A->private);
	unsigned int j;
	const unsigned int *r = row;

	for(j = 0; j < len; ++j) {

			if(head == NULL) {
				head = (list_node *)malloc(sizeof(list_node));
				if (head == NULL) {
					exit_prog(3);
				}
				head -> value = 1.0;
				head -> column = *r;
				tail = head;
			} else {
				tail -> next = (list_node *)malloc(sizeof(list_node));
				if (tail -> next == NULL) {
					exit_prog(3);
				}
				tail = tail -> next;
				tail -> value = 1.0;
				tail -> column = *r;
			}

		r++;
	}
	tail -> next = NULL;

	private[i] = head;
}

/*
 * Frees all resources of a row (one linked-list).
 *
 * This is a helper function to the function free_list.
 *
 * @param head: a pointer to the linked-list to be freed.
 */
void free_row(list_node *head) {
	list_node* temp;

	while((temp = head) != NULL) {
		head = head -> next;
		free(temp);
	}
}

/*
 * Frees all resources used by matrix A.
 *
 * @param A: the sparse matrix to be freed.
 */
void free_list(struct _spmat *A) {
	list_node **private = (list_node **) (A->private);
	list_node **curr = private;
	unsigned int i;

	for(i = 0; i < A -> n; ++i) {
		free_row(*curr);
		curr++;
	}
	free(private);
	free(A);
}

/*
 * Multiplies a matrix by a vector into a result vector.
 *
 * @param A: the matrix to multiply.
 * @param v: the vector to multiply.
 * @param result: a vector that by the end of the function contains the result of the multiplication.
 */
void mult_list(const struct _spmat *A, const double *v, double *result) {
	unsigned int i, j;
	double res;
	const double *v_iter;
	list_node *node, **private = (list_node **) (A->private);
	list_node **curr = private;

	for(i = 0; i < A -> n; ++i) {
		node = *curr;
		res = 0;
		v_iter = v;
		j = 0;
		while(node != NULL) {
			v_iter += node -> column - j;
			j = node -> column;
			res += node -> value * (*v_iter);
			node = node -> next;
		}
		*result = res;
		result++;
		curr++;
	}
}

/*
 * Extracts a vector containing a row of the matrix.
 *
 * @param A: the matrix from which to extract the row.
 * @param i: the index of the row.
 * @param res: the vector that by the end of the function contains the row.
 */
void get_row_list(struct _spmat *A, unsigned int i, double *res) {
	list_node **private = (list_node**) (A -> private);
	list_node *node;
	unsigned int j;
	unsigned int n = A -> n;

	node = private[i];
	for (j = 0; j < n; j++) {
		if (node == NULL || j != node -> column) {
			*res = 0.0;
		}
		else {
			*res = node -> value;
			node = node->next;
		}
		res++;
	}
}

/*
 * Sums a row from a matrix.
 *
 * @param A: the matrix to calculate from.
 * @param i: the index of the row.
 *
 * @return: the sum of the row.
 */
double row_sum_list(struct _spmat *A, unsigned int i) {
	list_node **private = (list_node**) (A -> private);
	list_node *node;
	double res = 0.0;

	node = private[i];
	while (node != NULL) {
		res += node->value;
		node = node->next;
	}
	return res;
}

/*
 * Initializes a sparse matrix by linked-lists.
 *
 * @param n: number of rows in the matrix.
 *
 * @return: a pointer to the initialized matrix A.
 */
spmat* spmat_allocate_list(unsigned int n) {
	spmat* mat;

	mat = (spmat *)malloc(sizeof(spmat));
	if (mat == NULL) {
		exit_prog(3);
	}

	mat -> n = n;
	mat -> add_row_by_col = add_row_list_col;
	mat -> free = free_list;
	mat -> mult = mult_list;

	mat -> private = malloc(sizeof(list_node *)*n);
	if (mat -> private == NULL) {
		exit_prog(3);
	}

	mat->get_row = get_row_list;
	mat->row_sum = row_sum_list;

	return mat;
}

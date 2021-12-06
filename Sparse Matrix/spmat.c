#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "spmat.h"

/* defining a new structure: linked-list */
typedef struct _list_node {
	double value;
	int column;
	struct _list_node *next;
} list_node;


/* implementation of add_row in linked-lists */
void add_row_list(struct _spmat *A, const double *row, int i) {
	list_node *head = NULL, *tail, **private = (list_node **) (A->private);
	int j;
	const double *r = row;

	for(j = 0; j < A -> n; ++j) {
		if(*r != 0) {
			if(head == NULL) {
				head = (list_node *)malloc(sizeof(list_node));
				assert(head != NULL);
				head -> value = *r;
				head -> column = j;
				tail = head;
			} else {
				tail -> next = (list_node *)malloc(sizeof(list_node));
				assert(tail -> next != NULL);
				tail = tail -> next;
				tail -> value = *r;
				tail -> column = j;
			}
		}
		r++;
	}
	tail -> next = NULL;

	private[i] = head;
}


/* helper function for free that frees a given list */
void free_row(list_node *head) {
	list_node* temp;

	while((temp = head) != NULL) {
		head = head -> next;
		free(temp);
	}
}


/* implementation of free in linked-lists */
void free_list(struct _spmat *A) {
	list_node **private = (list_node **) (A->private);
	int i;

	for(i = 0; i < A -> n; ++i) {
		free_row(private[i]);
	}
	free(private);
	free(A);
}


/* implementation of mult in linked-lists */
void mult_list(const struct _spmat *A, const double *v, double *result) {
	int i;
	double res;
	list_node *node, **private = (list_node **) (A->private);

	for(i = 0; i < A -> n; ++i) {
		node = private[i];
		res = 0;
		while(node != NULL) {
			res += node -> value * v[node -> column];
			node = node -> next;
		}
		result[i] = res;
	}
}


/* implementation of spmat_allocate_list */
spmat* spmat_allocate_list(int n) {
	spmat* mat;

	mat = (spmat *)malloc(sizeof(spmat));
	assert(mat != NULL);

	mat -> n = n;
	mat -> add_row = add_row_list;
	mat -> free = free_list;
	mat -> mult = mult_list;

	mat -> private = malloc(sizeof(list_node *)*n);
	assert(mat -> private != NULL);

	return mat;
}



/* defining a structure for the array representation */
typedef struct _arr_rep {

	double *values;
	int *colind;
	int *rowptr;

} arr_rep;


/* implementation of add_row in arrays */
void add_row_arr(struct _spmat *A, const double *row, int i)
{
	arr_rep *private = A->private;
	int j;
	int ptr = private->rowptr[i];
	const double *r = row;

	for (j = 0; j < A->n; j++) {
		if (*r != 0) {
			private->values[ptr] = row[j];
			private->colind[ptr] = j;
			ptr++;
		}
		r++;
	}
	private->rowptr[i + 1] = ptr;
}


/*implementation of free in arrays */
void free_arr(struct _spmat *A)
{
	arr_rep *private = A->private;

	free(private->colind);
	free(private->rowptr);
	free(private->values);
	free(private);
	free(A);
}


/* implementation of mult in arrays */
void mult_arr(const struct _spmat *A, const double *v, double *result)
{
	arr_rep *private = A->private;
	int i, j, col;
	int curr, next;
	double row_sum;

	for (i = 0; i < A->n; i++) {
		curr = private->rowptr[i];
		next = private->rowptr[i + 1];
		row_sum = 0.0;
		for (j = curr; j < next; j++) {
			col = private->colind[j];
			row_sum += v[col] * private->values[j];
		}
		result[i] = row_sum;
	}
}


/* helper method for spmat_allocate_array that initializes the private field */
arr_rep* init_arr_rep(int n, int nnz) {
	arr_rep *rep;

	rep = (arr_rep *) malloc(sizeof(arr_rep));
	assert(rep != NULL);

	rep->colind = (int *) malloc(sizeof(int) * nnz);
	assert(rep->colind != NULL);

	rep->values = (double *) malloc(sizeof(double) * nnz);
	assert(rep->values != NULL);

	rep->rowptr = (int *) malloc(sizeof(int) * (n + 1));
	assert(rep->rowptr != NULL);

	rep->rowptr[0] = 0;

	return rep;
}


/* implementation of spmat_allocate_array */
spmat* spmat_allocate_array(int n, int nnz) {
	spmat *mat;

	mat = (spmat *) malloc(sizeof(spmat));
	assert(mat != NULL);

	mat->n = n;
	mat->add_row = add_row_arr;
	mat->free = free_arr;
	mat->mult = mult_arr;
	mat->private = init_arr_rep(n, nnz);

	return mat;
}

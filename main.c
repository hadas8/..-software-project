#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "spmat.h"

int main(int argc, char* argv[]) {
	char *impl;
	int i, j, n, length, cols = 1, nnz = 0;
	double *b, *row, *Ab, dot_product, vecor_val, epsilon = 0.00001;
	double *row_ptr;
	FILE *input, *vector, *output;
	clock_t start, end;
	spmat *A;
	

	assert(argc == 4 || argc == 5);
	start = clock();
	impl = argv[argc - 1];
	assert(strcmp(impl, "-list") == 0 || strcmp(impl, "-array") == 0);

	/* initialize the random number generator */
	srand(time(NULL));

	/* determining the size of the matrix */
	input = fopen(argv[1], "r");
	assert(input != NULL);
	n = fread(&length, sizeof(int), 1, input);
	assert(n == 1);


	/* reading the vector b0 or randomize it if not given */
	b = (double*)malloc(length * sizeof(double));
	assert(b != NULL);
	if(argc == 4) {
		for(i = 0; i < length; ++i) {
			b[i] = (double)rand();
		}
	} else {
		vector = fopen(argv[2], "r");
		assert(vector != NULL);
		n = fseek(vector, 2 * sizeof(int), SEEK_SET);
		assert(n == 0);
		n = fread(b, sizeof(double), length, vector);
		assert(n == length);
		fclose(vector);
	}


	row = (double*)malloc(length * sizeof(double));
	assert(row != NULL);
	
	/* allocate a sparse matrix in arrays */
	if(!strcmp(impl, "-array")) {
		n = fseek(input, 2 * sizeof(int), SEEK_SET);
		assert(n == 0);
		for(i = 0; i < length; ++i) {
			n = fread(row, sizeof(double), length, input);
			assert(n == length);
			row_ptr = row;
			for(j=0; j < length; j++) {
				if (*row_ptr != 0) {
					nnz++;
				}
				row_ptr++;
			}
		}
		
		A = spmat_allocate_array(length, nnz);
	}
	
	/* allocate a sparse matrix in linked-lists */
	if(!strcmp(impl, "-list")) {
		A = spmat_allocate_list(length);
	}

	/* reading the matrix, adding each row to the sparse matrix */
	n = fseek(input, 2 * sizeof(int), SEEK_SET);
	assert(n == 0);
	for(i = 0; i < length; ++i) {
		n = fread(row, sizeof(double), length, input);
		assert (n == length);
		A -> add_row(A, row, i);
	}
	fclose(input);
	free(row);

	/* allocate memory for Ab */
	Ab = (double*)malloc(length * sizeof(double));
	assert(Ab != NULL);

	/* power algorithm */
	do {
		/* calculating Ab */
		A -> mult(A, b, Ab);

		/* Ab's magnitude */
		dot_product = 0;
		for(i = 0; i < length; ++i) {
			dot_product += Ab[i] * Ab[i];
		}
		dot_product = sqrt(dot_product);

		/*
		* calculating b_k+1 and differences
		* using n as a boolean, to check if differences are not small enough
		*/
		n = 0;
		for(i = 0; i < length; ++i) {
			vecor_val = Ab[i] / dot_product;
			if((!n) && fabs(b[i] - vecor_val) >= epsilon) {
				n = 1;
			}
			b[i] = vecor_val;
		}
	} while (n);
	free(Ab);
	A->free(A);

	/* writing the eigenvector in the output file as a matrix with 1 column and v_len rows */
	output = fopen(argv[argc - 2], "w");
	assert(output != NULL);
	n = fwrite(&cols, sizeof(int), 1, output);
	assert(n = 1);
	n = fwrite(&length, sizeof(int), 1, output);
	assert(n = 1);
	n = fwrite(b, sizeof(double), length, output);
	assert(n = length);
	
	end = clock();
	printf("exe time is %f seconds\n", ((double)(end - start)/CLOCKS_PER_SEC));

	fclose(output);
	free(b);
	return 0;
}


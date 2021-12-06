#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "SPBufferset.h"

int main(int argc, char* argv[]) {
	int v_len, i, j, n, cols = 1;
	double *b, *a_dot_b, *mat_row, vector_val, dot_product, epsilon = 0.00001;
	FILE *input, *output;
	SP_BUFF_SET();

	assert(argc == 3);

	/* initialize the random number generator */
	srand(time(NULL));

	/* determining vector length */
	input = fopen(argv[1], "r");
	assert (input != NULL);
	n = fread(&v_len, sizeof(int), 1, input);
	assert (n == 1);

	/* allocating memory for vector b0 and initializing random numbers */
	b = (double*) malloc(v_len * sizeof(double));
	assert(b != NULL);
	for (i = 0; i < v_len; ++i) {
		b[i] = (double) rand();
	}

	/* allocating memory for Ab and for matrix row */
	a_dot_b = (double*) malloc(v_len * sizeof(double));
	mat_row = (double*) malloc(v_len * sizeof(double));


	/* power algorithm */
	do {
		n = fseek(input, 2 * sizeof(int), SEEK_SET);
		assert (n == 0);

		/* calculating Ab */
		for (i = 0; i < v_len; ++i) {
			n = fread(mat_row, sizeof(double), v_len, input);
			assert (n == v_len);
			dot_product = 0;
			for (j = 0; j < v_len; ++j) {
				dot_product += mat_row[j] * b[j];
			}
			a_dot_b[i] = dot_product;
		}

		/* calculating Ab's magnitude */
		dot_product = 0;
		for (i = 0; i < v_len; ++i) {
			dot_product += a_dot_b[i] * a_dot_b[i];
		}
		dot_product = sqrt(dot_product);

		/*
		 * calculating b_k+1 and differences
		 * using n as a boolean, to check if differences are not small enough
		 *  */
		n = 0;
		for (i = 0; i < v_len; ++i) {
			vector_val = a_dot_b[i] / dot_product;
			if ((!n) && (fabs(b[i] - vector_val) >= epsilon)) {
				n = 1;
			}
			b[i] = vector_val;
		}
	} while (n);
	fclose(input);
	free(a_dot_b);
	free(mat_row);

	/* writing the eigenvector in the output file as a matrix with 1 column and v_len rows */
	output = fopen(argv[2], "w");
	assert (output != NULL);
	n = fwrite(&cols, sizeof(int), 1, output);
	assert (n == 1);
	n = fwrite(&v_len, sizeof(int), 1, output);
	assert (n == 1);
	n = fwrite(b, sizeof(double), v_len, output);
	assert (n == v_len);


	fclose(output);
	free(b);
	return 0;
}



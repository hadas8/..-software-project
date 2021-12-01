#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "mat.h"
#include "spmat.h"
#include "vec_group.h"
#include "division.h"
#include "temp_vectors.h"

/*
 * This is the main file of the program that unites all of the modules.
 */

/*
 * Reads the input file into the matrix B.
 *
 * @param input: the input file.
 * @param length: the number of nodes in the graph.
 * @param B: the initialized matrix B.
 * @param vec: an initialized vector, that by the end of the function will contain all indices from 0 to length - 1.
 * 			   This vector represents the trivial division (will be the first vector in the set P).
 */
void read_into_B(FILE *input, unsigned int length, mat *B, unsigned int *vec) {
	unsigned int n, i, k;
	unsigned int *row, *vec_curr;
	double *k_curr;

	row = (unsigned int*)malloc(length * sizeof(unsigned int));
	if (row == NULL) {
		exit_prog(3);
	}
	k_curr = B -> k;
	vec_curr = vec;

	for (i = 0; i < length; i++) {
		n = fread(&k, sizeof(unsigned int), 1, input);
		if (n != 1) {
			exit_prog(2);
		}
		*k_curr = k;
		n = fread(row, sizeof(unsigned int), k, input);
		if (n != k) {
			exit_prog(2);
		}
		B->add_row(B, row, i, k);
		k_curr++;
		*vec_curr++ = i; /*create initial group vector containing all vertices*/
	}
	free(row);
	fclose(input);
}

/*
 * Writes the final clusters of the graph into the output file.
 *
 * @param output: the output file.
 * @param O: the output set O from algorithm 3, containing the clusters.
 */
void write_output(FILE *output, vec_group *O) {
	unsigned int n, i, group_size, O_size;
	unsigned int *popped;

	O_size = O->size;
	n = fwrite(&O_size, sizeof(unsigned int), 1, output); /*write num of groups*/
	if (n != 1) {
		exit_prog(1);
	}

	for (i = 0; i < O_size; i++) {
		popped = O->pop_group(O, &group_size);
		n = fwrite(&group_size, sizeof(unsigned int), 1, output); /*write num of elements in a group*/
		if (n != 1) {
			exit_prog(1);
		}
		n = fwrite(popped, sizeof(unsigned int), group_size, output); /*write indices*/
		free(popped);
	}

	O->free(O);
	fclose(output);
}

/*
 * Creates vectors for the two groups determined by the division.
 *
 * @param group1: initialized vector for the first group.
 * @param group2: initialized vector for the second group.
 * @param groupA: address of the variable that receives the size of group1.
 * @param groupB: address of the variable that receives the size of group2.
 * @param vec: vector containing the original group (before the division).
 * @param div_vec: vector of 1's and -1's that determines the division.
 * @param group_size: size of the original group.
 */
void new_vecs(unsigned int *group1, unsigned int *group2, unsigned int *groupA, unsigned int *groupB, unsigned int *vec,	double *div_vec, unsigned int group_size) {
	unsigned int i;

	for (i = 0; i < group_size; i++) {
		if (*div_vec++ == 1) {
			*group1++ = *vec;
			(*groupA)++;
		}
		else {
			*group2++ = *vec;
			(*groupB)++;
		}
		vec++;
	}
}

/*
 * Implements algorithm 3 and creates clusters from the graph.
 *
 * @param O: empty set of groups, that will be filled with the final clusters.
 * @param vec: vector representing the trivial division.
 * @param length: number of nodes in the graph.
 * @param B: the matrix B.
 */
void algorithm3(vec_group *O, unsigned int *vec, unsigned int length, mat *B) {
	unsigned int vec_size, groupA, groupB;
	unsigned int *group1, *group2;
	double *div_vec;
	vec_group *P;
	temp_vectors *temps;

	P = allocate_group();
	P->add_group(P, vec, length);

	temps = allocate_temps(length);

	div_vec = (double*)malloc(sizeof(double) * length);
	if (div_vec == NULL) {
		exit_prog(3);
	}

	while (P->size > 0) {
		groupA = 0;
		groupB = 0;
		vec = P->pop_group(P, &vec_size);
		division(B, vec, vec_size, div_vec, temps); /*Algorithm 2 + 4*/

		group1 = (unsigned int*)malloc(sizeof(unsigned int) * vec_size);
		if (group1 == NULL) {
			exit_prog(3);
		}

		group2 = (unsigned int*)malloc(sizeof(unsigned int) * vec_size);
		if (group2 == NULL) {
			exit_prog(3);
		}

		new_vecs(group1, group2, &groupA, &groupB, vec, div_vec, vec_size);

		if (groupA == 0 || groupB == 0) { /*if one of the groups is empty*/
			O->add_group(O, vec, vec_size);
			free(group1);
			free(group2);
		}
		else {
			free(vec);
			if (groupA > 1) {
				P->add_group(P, group1, groupA);
			}
			else {
				O->add_group(O, group1, groupA);
			}
			if (groupB > 1) {
				P->add_group(P, group2, groupB);
			}
			else {
				O->add_group(O, group2, groupB);
			}
		}
	}

	free(div_vec);
	P->free(P);
	B->free(B);
	temps->free(temps);
}

int main(int argc, char* argv[]) {

	unsigned int n, length;
	unsigned int *vec;
	FILE *input, *output;
	mat *B;
	vec_group *O;
	clock_t start, end;


	start = clock();
	srand(time(NULL));

	if (argc != 3) {
		exit_prog(5);
	}

	/*read input into matrix B*/
	input = fopen(argv[1], "r");
	if (input == NULL) {
		exit_prog(0);
	}

	n = fread(&length, sizeof(unsigned int), 1, input);
	if (n != 1) {
		exit_prog(2);
	}

	vec = (unsigned int*)malloc(sizeof(unsigned int) * length);
	if (vec == NULL) {
		exit_prog(3);
	}

	B = allocate_mat(length);

	read_into_B(input, length, B, vec);

	if (B->M == 0) { /*if there are no edges - B is illegal*/
		exit_prog(4);
	}

	/*Algorithm 3*/
	O = allocate_group();

	algorithm3(O, vec, length, B);

	/*write into output file*/
	output = fopen(argv[2], "w");
	if (output == NULL) {
		exit_prog(0);
	}

	write_output(output, O);

	end = clock();

	printf("exe time: %f\n", (double)(end - start)/CLOCKS_PER_SEC);

	return 0;
}


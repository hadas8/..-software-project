#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "SPBufferset.h"


int main(int argc, char* argv[])
{
	int rows, cols, n, size;
	int i, j, k;
	int curr;
	FILE *inp;
	FILE *outp;
	double *arr, *avgs, *row_dot, *first_pos, *first_row, *sec_pos;
	double sum, count;
	SP_BUFF_SET();

	assert(argc == 3);


	inp = fopen(argv[1], "r");
	assert(inp != NULL);

	n = fread(&cols, sizeof(int), 1, inp);
	assert(n == 1);
	n = fread(&rows, sizeof(int), 1, inp);
	assert(n == 1);

	size = cols * rows;
	arr = (double*) calloc(sizeof(double), size);
	assert(arr != NULL);
	n = fread(arr, sizeof(double), size, inp);
	assert(n == size);

	/*
	 * find average of each row
	 */
	curr = 0;
	avgs = (double*) calloc(sizeof(double), rows);
	assert(avgs != NULL);
	
	for (i = 0; i < rows; i++) {
		count = 0.0;
		for (j = 0; j < cols; j++) {
			count += arr[curr];
			curr++;
		}
		count = count / cols;
		avgs[i] = count;
	}
	
	/*
	 * standardize each row by its average
	 */
	curr = 0;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			arr[curr] = arr[curr] - avgs[i];
			curr++;
		}
	}
	
	/*
	 * compute and write covariance matrix into output file
	 */

	outp = fopen(argv[2], "w");
	assert(outp != NULL);
	n = fwrite(&rows, sizeof(int), 1, outp);
	assert(n == 1);
	n = fwrite(&rows, sizeof(int), 1, outp);
	assert(n == 1);	

	row_dot = (double*) calloc(sizeof(double), rows);
	assert(row_dot != NULL);

	first_row = arr;
	for (i = 0; i < rows; i++) {
		sec_pos = arr;
		for (j = 0; j < rows; j++) {
			sum = 0.0;
			first_pos = first_row;
			for (k = 0; k < cols; k++) {
				sum += (*(first_pos++)) * (*(sec_pos++));
			}
			row_dot[j] = sum;
		}
		first_row = first_pos;

		n = fwrite(row_dot, sizeof(double), rows, outp);
		assert(n == rows);
	}
	
	

	/*
	 * finish
	 */
	fclose(inp);
	fclose(outp);
	free(arr);
	free(avgs);
	free(row_dot);
	return 0;
}

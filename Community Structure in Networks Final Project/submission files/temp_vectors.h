#ifndef TEMP_VECTORS_H_
#define TEMP_VECTORS_H_

/*
 * temp_vectors module:
 *
 * This module helps to reduce memory allocations for local vectors, by allocating them once
 * and reusing them.
 *
 * It consists of the struct _temp_vectors.
 *
 * Fields in _temp_vectors:
 * unmoved			- A vector for integers, used only in the modularity maximization algorithm.
 * indices			- A vector of pointers to doubles, used only in the modularity maximization algorithm.
 * eigenvector		- A vector of doubles, used to store the values of the eigenvector created in the power iteration algorithm.
 * local			- A vector of doubles, used in several different functions, such as inner_product, to store values of vectors only used locally.
 *
 * Functions in _temp_vectors:
 * free				- Frees all resources used by the temp_vectors struct.
 *
 * Additional functions:
 * allocate_temps	- Initializes the temp_vectors struct, used once before starting algorithm3.
 */

typedef struct _temp_vectors {

	/* vector of integers, used in modularity maximization */
	unsigned int *unmoved;

	/*vector of pointers to doubles, used in modularity maximization */
	double **indices;

	/* vector of doubles, used in power iteration */
	double *eigenvector;

	/* vector of doubles, used for local use in several different functions */
	double *local;

	/* Frees all resources used by v */
	void	(*free)(struct _temp_vectors *v);

} temp_vectors;

/* Initialize a temp_vectors struct with vectors of length n */
temp_vectors* allocate_temps(unsigned int n);

#endif /* TEMP_VECTORS_H_ */

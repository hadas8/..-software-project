#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "spmat.h"
#include "mat.h"
#include "temp_vectors.h"

#define IS_POSITIVE(X) ((X) > 0.0001)
/*
 * Implementation of algorithm 2 + Algorithm 4:
 * The division of a group into two groups,
 * represented by a division vector
 */

/*
 * Power iteration algorithm:
 * calculating the dominant eigen vector of a matrix
 *
 * @param B_hat: the matrix representing the group
 * @param one_norm: the sum of the largest column
 * @param eigenvector: an initialized vector to be filled with the values
 * 						of the dominant eigenvector
 * @param numer: the numerator from the power iteration formula.
 */
void power_iteration(mat* B_hat, unsigned int one_norm, double *eigenvector, double *numer) {
	unsigned int i, bool, iter = 0, n = B_hat -> A -> n;
	double *eigenvector_iter = eigenvector, *numer_iter, denom, val;

	/* randomizing initial vector */
	for(i = 0; i < n; ++i) {
		*(eigenvector_iter++) = (double)rand();
	}


	/*power formula */
	do {
		/* avoiding infinite loop */
		if(iter > 150000 * n + 1) {
			exit_prog(6);
		}
		/* calculating numerator */
		B_hat -> mult(B_hat, eigenvector, numer, one_norm);

		/* calculating denominator */
		denom = sqrt(dot(numer, numer, n));

		/*
		 * calculating eigenvector_k+1 and differences
		 * using bool to check if differences are not small enough
		 */
		bool = 0;
		eigenvector_iter = eigenvector;
		numer_iter = numer;
		for(i = 0; i < n; ++i) {
			val = *(numer_iter++) / denom;
			if((!bool) && IS_POSITIVE(fabs(*eigenvector_iter - val))) {
				bool = 1;
			}
			*(eigenvector_iter++) = val;
			++iter;
		}
	} while (bool);
}

/*
 * helper method that calculates the inner product:
 * vAv for vector v and matrix B
 *
 * @param B: an input matrix
 * @param v: an input vector
 * @param one_norm: the sum of the largest column
 * @param Bv: pre-allocated vector to store the product of the multiplication.
 *
 * @return: the inner product vBv
 */
double inner_product(mat* B, double* v, double one_norm, double *Bv) {
	double vBv;
	unsigned int n = B -> A -> n;
	B -> mult(B, v, Bv, one_norm);
	vBv = dot(v, Bv, n);
	return vBv;
}

/*
 * calculating dominant eigen value
 *
 * @param B_hat: the matrix representing the group
 * @param eigenvector: the dominant eigen vector
 * @param one_norm: the sum of the largest column
 * @param local: pre-allocated vector to be given to inner_product as variable
 *
 * @return: the dominant eigen value
 */
double dominant_value(mat* B_hat, double* eigenvector, double one_norm, double *local) {
	double numer, denom;
	numer = inner_product(B_hat, eigenvector, one_norm, local);
	denom = dot(eigenvector, eigenvector, B_hat -> A -> n);
	if (denom == 0) {
		exit_prog(4);
	}
	return numer / denom;
}

/*
 * calculating modularity Q for group g
 *
 * @param B_hat the matrix representing the group
 * @param div_vector: a vector of  1/-1 representing a division
 * @param local: pre-allocated vector to be given to inner_product.
 *
 * @return: the modularity of the division in the group
 */
double modularity(mat* B_hat, double* div_vector, double *local) {
	return 0.5 * inner_product(B_hat, div_vector, 0, local);
}

/*
 * set the vector (1,...,1)
 *
 * @param vector: an initialized vector
 * @param size: size of the vector
 */
void set1(double* vector, unsigned int size) {
	unsigned int i;
	for(i = 0; i < size; ++i) {
		*(vector++) = 1.0;
	}
}


/*
 * modularity maximization - algorithm 4
 *
 * @param B_hat the matrix representing the group
 * @param div_vector: a vector of  1/-1 representing a division
 * @param n: size of the group
 * @param temps: a struct containing pre-allocated vectors for local use.
 */
void mod_max(mat* B_hat, double* div_vector, unsigned int n, temp_vectors *temps) {
	unsigned int i, j, safe = 0,
		*unmoved  /* Array of indices changed in div_vector */,
		argmax_improve,
		*unmoved_iter,  /* Iterator over unmoved */
		*argmax_score;  /* Pointer to the max score index in unmoved */
	double score, max_score, max_improve, last_improve, mod_delta, **indices_iter, *local,
        *div_vector_iter,  /* Iterator over div_vector */
        *div_idx,  /* Pointer to the max score in div_vector */
        **indices;  /* Array of pointers in div_vector in order of changing */

	unmoved = temps->unmoved;
	indices = temps->indices;
	local = temps->local;

	do {
		memset(unmoved, 0, sizeof(unsigned int) * n);

		max_improve = 0;
		argmax_improve = 0;
		last_improve = 0;
        indices_iter = indices;
		/* find improvement for the partition defined by div_vector */
		for(i = 0; i < n; ++i) {
			max_score = 0;
			/* computing change in modularity for each unmoved vertex
			 * determining the vertex with the maximal change
			 */
            unmoved_iter = unmoved;
            div_vector_iter = div_vector;
			safe = 0;
            for(j = 0; j < n; ++j) {
				if(*unmoved_iter == 0) {
					score = -4 * (B_hat -> mult_by_dot(B_hat, div_vector, j, local)) * (*div_vector_iter);

                    if(score > max_score || max_score == 0) {
                    	safe = 1;
                        max_score = score;
                        div_idx = div_vector_iter;
                        argmax_score = unmoved_iter;
                    }
                }
                ++unmoved_iter;
                ++div_vector_iter;
			}

			/*
			 * moving the vertex with the maximal score
			 */
            if(!safe) {
            	continue;
            }
			*div_idx *= -1;
            /*
             * move indices_iter to indices[n-1]
             */
			*(indices_iter++) = div_idx;
			last_improve = last_improve + max_score;
			if(last_improve > max_improve) {
				max_improve = last_improve;
				argmax_improve = i;
			}

            /*
             * Remove max score index from unmoved
             */
			*argmax_score = 1;
		}

		/*
		 * find maximum improvement and update div_vector
		 */
		if(argmax_improve == n - 1 || !IS_POSITIVE(max_improve)) {
			mod_delta = 0;
		} else {
			mod_delta = max_improve;
			for(i = n - 1; i > argmax_improve; --i) {
				**(--indices_iter) *= -1;
			}
		}
	} while (IS_POSITIVE(mod_delta));

}

/*
 * Implementation of algorithm 2:
 * dividing a group into two
 *
 * @param B: the matrix representing the original group
 * @param index_vector: a vector of indices
 * 						representing the group to be divided
 * @param group_size: size of the group
 * @param div_vector: an initialized vector to be filled with 1/-1
 * 					  representing the division of the group into two
 * @param temps: a struct containing pre-allocated vectors for local use
 */
void division(mat* B, unsigned int* index_vector, unsigned int group_size ,double* div_vector, temp_vectors *temps)
{
	mat* B_hat;
	double one_norm, *eigenvector, *local, *eigenvector_iter, *div_vector_iter = div_vector, eigenvalue, mod;
	unsigned int i;

	local = temps->local;

	/*
	 * creating B_hat
	 */
	B_hat = sub_mat(B, index_vector, group_size, local);

	/*
	 * finding leading eigenpair
	 * matrix shifting and power iteration
	 */

	one_norm = B_hat -> max_sum(B_hat, local);
	eigenvector = temps->eigenvector;
	power_iteration(B_hat, one_norm, eigenvector, local);
	eigenvalue = dominant_value(B_hat, eigenvector, one_norm, local) - one_norm;

	/*
	 * calculating division vector s
	 */
	if (!IS_POSITIVE(eigenvalue)) {
		set1(div_vector, group_size);
		goto cleanup;
	} else {
		eigenvector_iter = eigenvector;
		for(i = 0; i < group_size; ++i) {
			*(div_vector_iter++) = *(eigenvector_iter++) > 0 ? 1.0 : -1.0;
		}
		/*
		 * modularity maximization
		 */
		mod_max(B_hat, div_vector,group_size, temps);

		mod = modularity(B_hat, div_vector, local);
		if(!IS_POSITIVE(mod)) {
			set1(div_vector, group_size);
			goto cleanup;
		}
	}

cleanup:
	B_hat -> free(B_hat);
}

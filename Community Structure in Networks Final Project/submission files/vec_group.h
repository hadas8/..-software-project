#ifndef ERRORS_H_
#include "errors.h"
#endif

#ifndef VEC_GROUP_H_
#define VEC_GROUP_H_

/*
 * vec_group module:
 *
 * This module represents a set of vectors (the sets P and O in algorithm 3).
 * The set works like a stack.
 * Each vector represents a sub-group of nodes in the graph.
 *
 * It contains the struct _vec_group to represent the set.
 *
 * Fields in _vec_group:
 * size				- The number of vectors in the set.
 * group_list		- A linked-list of groups.
 *
 * Functions in _vec_group:
 * free				- Frees all resources used by the set.
 * add_group		- Adds a group to the set.
 * pop_group		- Pops a group from the set.
 *
 * Additional functions in the module:
 * allocate_group	- Initializes an empty group set.
 */

typedef struct _vec_group {

	/*number of groups in the set*/
	unsigned int size;

	/*linked-list of groups in this set*/
	void **group_list;

	/*Frees all resources used by P*/
	void	(*free)(struct _vec_group *P);

	/*Adds a group represented by group_vec of size num into P*/
	void	(*add_group)(struct _vec_group *P, unsigned int *group_vec, unsigned int num);

	/*Returns a vector of the first group in P and saves the group's size into num*/
	unsigned int*	(*pop_group)(struct _vec_group *P, unsigned int *num);

} vec_group;

/*allocates and initializes an empty set of groups*/
vec_group* allocate_group();

#endif /* VEC_GROUP_H_ */

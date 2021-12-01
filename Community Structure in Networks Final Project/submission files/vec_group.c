#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec_group.h"

/*
 * struct _vec_node:
 *
 * This struct is a linked-list node in the vec_group field group_list, representing a sub-group of vertices.
 *
 * Fiels in _vec_node:
 * num		- The number of vertices in the group.
 * vector	- A vector of indices of the nodes in the group.
 * next		- Pointer to the next group in the set.
 */

typedef struct _vec_node {
	/*the size of the group*/
	unsigned int num;

	/*vector that represents the group*/
	unsigned int *vector;

	/*pointer to the next node in the linked-list*/
	struct _vec_node *next;
} vec_node;

/*
 * Adds a group (represented by vec_node) to the set.
 *
 * @param P: the set to add the group to.
 * @param group_vec: vector containing indices of the vertices in the group.
 * @param num: number of vertices in the group.
 */
void add_group_vec(struct _vec_group *P, unsigned int *group_vec, unsigned int num) {
	vec_node *head, *new_head, **groups = (vec_node**) P -> group_list;

	new_head = (vec_node*)malloc(sizeof(vec_node));
	if (new_head == NULL) {
		exit_prog(3);
	}
	new_head->next = NULL;
	new_head->vector = group_vec;
	new_head->num = num;

	head = *groups;

	if (head != NULL) {
		new_head->next = head;
	}
	*groups = new_head;
	P->size ++;
}

/*
 * Pops a group from the set.
 *
 * @param P: the set to pop the group from.
 * @param num: the address of a variable that receives the size of the group.
 *
 * @return: a vector containing the indices of the vertices in the group.
 */
unsigned int* pop_group_vec(struct _vec_group *P, unsigned int *num) {
	vec_node *head, *new_head, **groups = (vec_node**) P->group_list;
	unsigned int* vec;

	head = *groups;
	new_head = head->next;
	vec = head->vector;
	*num = head->num;
	*groups = new_head;
	P->size --;
	free(head);
	return vec;
}

/*
 * Frees all resources used by the set.
 *
 * @param P: the set to be freed.
 */
void free_vec(struct _vec_group *P) {
	vec_node *head, *curr, **groups = (vec_node **) P->group_list;
	unsigned int *vec;

	head = *groups;

	while((curr = head) != NULL) {
		vec = curr->vector;
		head = head -> next;
		free(vec);
		free(curr);
	}

	free(groups);
	free(P);
}

/*
 * Initializes an empty set of group.
 *
 * @return: a pointer to the initialized set.
 */
vec_group* allocate_group() {
	vec_group *group;

	group = (vec_group*)malloc(sizeof(vec_group));
	if (group == NULL) {
		exit_prog(3);
	}
	group->size = 0;

	group->group_list = malloc(sizeof(vec_node*));
	if (group->group_list == NULL) {
		exit_prog(3);
	}
	*group->group_list = NULL;

	group->add_group = add_group_vec;
	group->free = free_vec;
	group->pop_group = pop_group_vec;

	return group;
}

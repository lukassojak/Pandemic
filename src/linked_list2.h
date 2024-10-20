#ifndef LINKED_LIST2_H
#define LINKED_LIST2_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "agent_utils.h"
#include "world_utils.h"






/*
 * This file contains the definition of a singly linked list.
 * The linked list is used to store sorted pointers to struct agent in struct place.
 * The linked list is sorted by the id of the agent. So the first element of the list has the smallest id.
 * There are several functions to manipulate the linked list:
 * - create_linked_list() - creates an empty linked list
 * - insert() - inserts a new node with a pointer to struct agent in the linked list (and keeps the list sorted)
 * - remove_element() - removes a node with a pointer to struct agent from the linked list, frees the memory (and keeps the list sorted)
 * - free_linked_list() - frees the memory allocated for the linked list
*/






        /* * * * * * * * * * * * * * * * * * * * * * *
         *         Structs for linked list           *
         * * * * * * * * * * * * * * * * * * * * * * */


struct node {
    struct agent *agent; // payload of the node - pointer to struct agent
    struct node *next;   // pointer to the next node in the list
};

struct linked_list {
    struct node *head;   // pointer to the first node in the list
};




        /* * * * * * * * * * * * * * * * * * * * * * *
         *           Linked list functions           *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function create_linked_list() creates an empty linked list.
 * The linked list is dynamically allocated and the fields are initialized to NULL.
 * The function returns a pointer to the created linked list.
 * If the memory allocation fails, the function returns NULL.
*/
struct linked_list *create_linked_list2();

/*
 * The function insert() inserts a new node with a pointer to struct agent in the linked list.
 * The function receives a pointer to the linked list, a pointer to struct agent.
 * The function inserts the new node with the pointer to struct agent in the linked list and keeps the list sorted by the id of the agent.
 * The function returns true if the insertion was successful, otherwise false.
*/
bool insert2(struct linked_list *list, struct agent *agent);


/*
 * The function remove_element() removes a node with a pointer to struct agent from the linked list.
 * The function receives a pointer to the linked list and an id of the agent.
 * The function removes the node with the pointer to struct agent from the linked list, frees the memory, and keeps the list sorted by the id of the agent.
 * If the agent with the given id is not in the list, the function does nothing.
*/
void remove_element2(struct linked_list *list, int id);

/*
 * The function free_linked_list() frees the memory allocated for the linked list.
 * The function receives a pointer to the linked list and frees the memory allocated for the list and its nodes.
*/
void free_linked_list2(struct linked_list *list);






#endif // LINKED_LIST2_H
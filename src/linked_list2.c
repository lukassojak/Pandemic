#include "linked_list2.h"






/*
 * The function create_linked_list() creates an empty linked list.
 * The linked list is dynamically allocated and the fields are initialized to NULL.
 * The function returns a pointer to the created linked list.
 * If the memory allocation fails, the function returns NULL.
*/
struct linked_list *create_linked_list2()
{
    struct linked_list *new_ll = malloc(sizeof(struct linked_list));
    if (new_ll == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for linked list\n");
        return NULL;
    }

    (*new_ll).head = NULL;
    return new_ll;
}

/*
 * The function insert() inserts a new node with a pointer to struct agent in the singly linked list.
 * The function receives a pointer to the linked list, a pointer to struct agent.
 * The function inserts the new node with the pointer to struct agent in the linked list and keeps the list sorted by the id of the agent.
 * The function returns true if the insertion was successful, otherwise false.
*/
bool insert2(struct linked_list *list, struct agent *agent)
{
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for new node\n");
        return false;
    }
    (*new_node).agent = agent;
    (*new_node).next = NULL;

    // If the list is empty, insert the new node as the head
    if ((*list).head == NULL) {
        (*list).head = new_node;
        return true;
    }

    // If the new node's id is less than the head's id, insert the new node as the new head
    if ((*(*(*list).head).agent).id > (*agent).id) {
        (*new_node).next = (*list).head;
        (*list).head = new_node;
        return true;
    }

    struct node *current = (*list).head;
    struct node *previous = NULL;

    // Find the correct position to insert the new node
    int i = 0;
    while (current != NULL && ((*current).agent->id < (*agent).id)) {
        previous = current;
        current = (*current).next;
        i++;
    }

    // Insert the new node
    (*new_node).next = current;
    (*previous).next = new_node;

    return true;
}


/*
 * The function remove_element() removes a node with a pointer to struct agent from the linked list.
 * The function receives a pointer to the linked list and an id of the agent.
 * The function removes the node with the pointer to struct agent from the linked list, frees the memory, and keeps the list sorted by the id of the agent.
 * If the agent with the given id is not in the list, the function does nothing.
*/
void remove_element2(struct linked_list *list, int id)
{
    struct node *previous = (*list).head;
    // list is empty
    if ((*list).head == NULL) {
        fprintf(stderr, "remove_element2: item not found (empty list)\n");
        return;
    }
    if ((*(*(*list).head).agent).id == id) {
        struct node *former_head = (*list).head;
        (*list).head = (*former_head).next;
        free(former_head);
        return;
    }
    struct node *current = (*previous).next;
    while (current != NULL) {
        if ((*(*current).agent).id == id) {
            (*previous).next = (*current).next;
            free(current);
            return;
        }
        previous = current;
        current = (*current).next;
    }
    fprintf(stderr, "remove_element2: item not found\n");
}

/*
 * The function free_linked_list() frees the memory allocated for the linked list.
 * The function receives a pointer to the linked list and frees the memory allocated for the list and its nodes.
*/
void free_linked_list2(struct linked_list *list)
{
    if (list == NULL) {
        return;
    }
    struct node *current = (*list).head;
    while (current != NULL) {
        struct node *next = (*current).next;
        free(current);
        current = next;
    }
    free(list);
}
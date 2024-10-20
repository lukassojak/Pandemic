#ifndef WORLD_UTILS_H
#define WORLD_UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "agent_utils.h"
#include "linked_list2.h"






        /* * * * * * * * * * * * * * * * * * * * * * *
         *      Structs for access to world data     *
         * * * * * * * * * * * * * * * * * * * * * * */


/* Each place id in world.places array matches the index of the place element in the array
 * The world struct contains member "int array_size" which is the allocated size of the world.places array and is used to reallocate memory when needed
 * The world struct contains member "int max_index" which is the place with greatest id in the world.places array read from the file
 * The world struct contains member "int num_places" which is the number of read places from the file
 * Such approach allows to have place ids starting from 0 and not being consecutive integers
 * The place id is used as an index in the world.places array
 * If the file contains place with ids which are not consecutive integers, the world.places array will have empty places
 * These empty places will have id -1 and name NULL
 * Places must be checked for id -1 before using them
 * The world.places array will be always sorted by place id
 */


struct place {
    int id;                     // unique identifier of the place
    char *name;                 // name of the place
    float exposure;             // exposure rate of the place
    int total_infected_agents;  // total number of infections happened in the place
    int num_agents;             // number of agents in the place
    struct linked_list *agents; // linked list of agents in the place
};

struct world {
    size_t array_size;          // allocated size of the world.places array
    int max_index;              // the place with greatest id in the world.places array, -1 if no places in the world
    size_t num_places;          // number of places in the world
    struct place *places;       // array of places
};






        /* * * * * * * * * * * * * * * * * * * * * * *
         *            Read world function            *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function read_world() reads a world from a .csv file.
 * The function returns a pointer to a struct world with the information read from the file.
 * If an error occurs, the function returns NULL.
 */
struct world *read_world(FILE *file);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Other helper functions           *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function world_line_format_is_valid() checks whether string 'line' has required format
 */
bool world_line_format_is_valid(char line[128]);

/*
 * The function free_world() frees the memory allocated for a world.
 * The function receives a pointer to a struct world and frees the memory allocated for the struct and its fields.
 */
void free_world(struct world *world);



#endif // WORLD_UTILS_H
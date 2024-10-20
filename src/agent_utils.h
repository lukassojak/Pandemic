#ifndef AGENT_UTILS_H
#define AGENT_UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdbool.h>

#include "world_utils.h"
#include "linked_list2.h"





        /* * * * * * * * * * * * * * * * * * * * * * *
         *     Structs for access to agents data     *
         * * * * * * * * * * * * * * * * * * * * * * */


struct world;

enum chartype {
    dash,
    digit
};

enum agent_state {
    healthy = 0,                 
    infected = 1,                
    dead = 2                     
};


/* 
 * Each agent id in agents.agents_array matches the index of the agent element in the array
 * The world struct contains member "int array_size" which is the allocated size of the agents.agents_array and is used to reallocate memory when needed
 * The world struct contains member "int max_index" which is the agent with greatest id in the agents.agents_array read from the file
 * The world struct contains member "int num_agents" which is the number of read agents from the file
 * Such approach allows to have agents ids starting from 0 and not being consecutive integers
 * The agent id is used as an index in the agents.agents_array
 * If the file contains agents with ids which are not consecutive integers, the agents.agents_array will have empty agents
 * These empty agents will have id -1 and route NULL
 * Agents must be checked for id -1 before using them
 * The agents.agents_array will be always sorted by place id
 */


/*
 * The agent has a unique identifier, a route through the world, a current place, and vaccination and infection status.
 * The agent also has immunity, which is a value in the range [0, 1].
 * The agent can be in one of the following states: healthy, infected, dead.
*/
struct agent {
    int id;                 
    int *route;             // route of the agent through the world, cycle of place ids - array of integers
    int route_length;       // size of the route array
    int curr_route_idx;     // index to the route array of the current place in the route
    bool is_vaccinated;     
    float immunity;   
    enum agent_state state; // state of the agent - healthy, infected, dead
};

struct agents {
    size_t array_size;                  // allocated size of the agents_array
    int max_index;                      // the agent with greatest id in the agents_array, -1 if no agents in the world
    size_t num_agents;                  // number of agents in the world
    struct agent *agents_array;         // array of agents
};






        /* * * * * * * * * * * * * * * * * * * * * * *
         *           Read agents function            *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function read_agents() reads agents from a .csv file and updates the world with the agents.
 * The function creates a struct agents with the information read from the file and updates the world with the agents.
 * If an error occurs, the function returns NULL.
 * The function checks the validity of the route.
*/
struct agents *read_agents(FILE *file, struct world *world);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *         Route process functions           *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function agent_line_format_is_valid() checks whether string 'line' has required format
*/
bool agent_line_format_is_valid(char line[2048]);

/*
 * The function parse_route() parses the route string and returns an array of integers.
 */
int *parse_route(char *route, int *route_length, struct world *world);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Other helper functions           *
         * * * * * * * * * * * * * * * * * * * * * * */

        
/*
 * The function insert_agents_to_places() inserts agents to the places in the world.
 * The function receives a pointer to a struct agents and a pointer to a struct world.
 * The function inserts pointers to the agents to the places in the world according to the route of the agents.
 * The function returns true if the insertion was successful, false otherwise.
*/
bool insert_agents_to_places(struct agents *agents, struct world *world);

/*
 * The function free_agents() frees the memory allocated for a struct agents.
 * The function receives a pointer to a struct agents and frees the memory allocated for the struct and its fields.
 * In the struct world every struct place after calling free_agents() will have a dangling pointer to the agent.
 * Therefore, the function free_world() should be called after free_agents().
*/
void free_agents(struct agents *agents);





#endif // AGENT_UTILS_H
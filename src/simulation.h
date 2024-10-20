#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for time, srand

#include "world_utils.h"
#include "agent_utils.h"
#include "linked_list2.h"






        /* * * * * * * * * * * * * * * * * * * * * * *
         * Structs for simulation.c and main.c file  *
         * * * * * * * * * * * * * * * * * * * * * * */


struct virus {
    float lethality; // default 0.5, valid range (0, 1)
    float infectivity; // default 0.5, valid range (0, 1)
    float duration; // infection time, default 0.5, valid range (0, 1)
    float vaccine_modifier; // efficiency of vaccine, default 1.2, valid range > 0
};

struct simulation {
    int max_steps; // default -1, valid range > 0, -1 means no limit
    int random_seed; // default time(NULL) (if random_seed < 0), valid range > 0 - seed for random number generator
    bool summary; // default false - print summary at end of simulation, excluding verbose
    bool verbose; // default false - print detailed output at each step, excluding summary
};






        /* * * * * * * * * * * * * * * * * * * * * * *
         *     Structs for the simulation.c file     *
         * * * * * * * * * * * * * * * * * * * * * * */


enum result_type {
    step_limit,                  // simulation ended because the maximum number of steps was reached
    agents_dead,                 // simulation ended because all agents died
    virus_dead,                  // simulation ended because the virus died out and at least one agent survived
};

struct result {
    char *simulation_log;                       // log of the simulation
    int log_capacity;                           // capacity of the log
    int log_size;                               // size of the log
    enum result_type result;                    // type of result
    size_t total_infections;                    // total number of virus infections (transmissions from one agent to another)
    size_t total_deaths;                        // number of agents that died from the virus
    size_t survivors;                           // number of agents that survived the virus
    int most_infected_place;                    // id of the most infectious place (max total number of infected agents), -1 if multiple places have the same number of infected agents
    int steps;                                  // number of steps taken in the simulation
};






        /* * * * * * * * * * * * * * * * * * * * * * *
         *         Functions to move agents          *
         * * * * * * * * * * * * * * * * * * * * * * */

/*
 * The function move_agent_step() moves the agent to the next place in the route.
 */
bool move_agent_step(struct agent *agent, struct world *world);

/*
 * The function move_agents_step() moves all the agents in the agents array to the next place in their route.
 */
void move_agents_step(struct agents *agents, struct world *world);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *       Functions to progress illness       *
         * * * * * * * * * * * * * * * * * * * * * * */

/*
 * The function agent_illness_progress_step() progresses the illness of the agent by one step.
 * The function receives a pointer to a struct agent and a struct virus.
 * The function updates the state of the agent according to the virus and the simulation parameters.
 * The function writes a message to the simulation log.
 * The function updates struct result with the number of total infections and total deaths.
 * The function does not return a value.
 */
void agent_illness_progress_step(struct agent *agent, struct virus virus, struct simulation simulation, struct world *world, struct result *result, int *curr_infected);

/*
 * The function illness_progress_step() progresses the illness of the agents by one step.
 * The function receives a pointer to a struct agents, a struct virus and a pointer to a struct world.
 * The function updates the state of the agents according to the virus and the simulation parameters.
 * The function iterates over all the places (in ascending order by id) and over all the agents present in the place (in ascending order by id).
 * It uses the struct linked_list2 to iterate over the agents in the place.
 * The function writes a message to the simulation log for each agent that changes state.
 * The function does not return a value.
 */
void illness_progress_step(struct virus virus, struct simulation simulation, struct world *world, struct result *result, int *curr_infected);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Spread virus functions           *
         * * * * * * * * * * * * * * * * * * * * * * */

void place_spread_virus_step(struct place *place, struct virus virus, struct simulation simulation, struct result *result, int *curr_infected);

void spread_virus_step(struct world *world, struct virus virus, struct simulation simulation, struct result *result, int *curr_infected);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Main simulate function           *
         * * * * * * * * * * * * * * * * * * * * * * */

struct result *simulate(struct virus virus, struct simulation simulation, struct world *world, struct agents *agents);






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Other helper functions           *
         * * * * * * * * * * * * * * * * * * * * * * */
    
void write_to_log(struct result *result, char *line);

void write_step_to_log(struct result *result);

int infected_count(struct agents *agents);

void free_result(struct result *result);

void print_result(struct world *world, struct result *result, struct simulation simulation);



#endif // SIMULATION_H
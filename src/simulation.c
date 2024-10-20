#include "simulation.h"

const float EPS = 0.01;   // epsilon for float comparison








        /* * * * * * * * * * * * * * * * * * * * * * *
         *         Functions to move agents          *
         * * * * * * * * * * * * * * * * * * * * * * */


bool move_agent_step(struct agent *agent, struct world *world)
{
    int current_place_id = (*agent).route[(*agent).curr_route_idx];
    int next_place_id = (*agent).route[((*agent).curr_route_idx + 1) % (*agent).route_length];
    struct place *current_place = &(*world).places[current_place_id];
    struct linked_list *curr_place_agents = (*current_place).agents;

    // remove agent from current place
    remove_element2(curr_place_agents, (*agent).id);
    (*current_place).num_agents--;

    // add agent to the next place
    bool succ = insert2((*world).places[next_place_id].agents, agent);
    if (!succ) {
        fprintf(stderr, "Error: Could not insert agent into place\n");
        return false;
    }
    (*world).places[next_place_id].num_agents++;

    (*agent).curr_route_idx = ((*agent).curr_route_idx + 1) % (*agent).route_length;

    return true;
}

void move_agents_step(struct agents *agents, struct world *world)
{
    // iterate over all the agents
    for (int j = 0; j <= (*agents).max_index; j++) {
        if ((*agents).agents_array[j].id == -1) {
            continue;
        }
        struct agent *current_agent = &(*agents).agents_array[j];
        if ((*current_agent).state == dead) {
            continue;
        }

        bool succ = move_agent_step(current_agent, world);
        if (!succ) {
            fprintf(stderr, "Error: Could not move agent by one step\n");
            return;
        }
    }
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *       Functions to progress illness       *
         * * * * * * * * * * * * * * * * * * * * * * */


void agent_illness_progress_step(struct agent *agent, struct virus virus, struct simulation simulation, struct world *world, struct result *result, int *curr_infected)
{
    assert((*agent).state == infected);
    float lethality = virus.lethality;
    float duration = virus.duration;
    float vaccine_modifier = virus.vaccine_modifier;
    char *place_name = ((*world).places[((*agent).route[(*agent).curr_route_idx])]).name;

    // check if the agent dies
    double roll = (double) rand() / RAND_MAX;   // random number in the range [0, 1]
    if ((*agent).is_vaccinated) {
        roll = roll * vaccine_modifier;
    }
    bool is_dead = (roll - lethality) < EPS;
    if (is_dead) {
        (*agent).state = dead;
        (*result).total_deaths++;
        (*curr_infected)--;

        // write to the simulation log
        int buffer_size = strlen("Agent  (V) has died at . (X.XX < X.XX)\n") + strlen(place_name) + 11;    // 10 is the maximum agent.id length (+\0)
        char *buffer = malloc(sizeof(char) * buffer_size);
        if (buffer == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for buffer to log operation\n");
            return;
        }

        if (simulation.verbose) {
            if ((*agent).is_vaccinated)
                sprintf(buffer, "Agent %d (V) has died at %s. (%.2f < %.2f)\n", (*agent).id, place_name, roll, lethality);
            else
                sprintf(buffer, "Agent %d has died at %s. (%.2f < %.2f)\n", (*agent).id, place_name, roll, lethality);
        }
        else {
            sprintf(buffer, "Agent %d has died at %s.\n", (*agent).id, place_name);
        }
        
        write_to_log(result, buffer);
        free(buffer);
        return;
    }


    // check if the agent recovers
    roll = (double) rand() / RAND_MAX;
    bool cured = (roll - duration) > EPS;   // random number in the range [0, 1]
    if (cured) {
        (*agent).state = healthy;
        (*curr_infected)--;

        // write to the simulation log
        int buffer_size = strlen("Agent  has recovered at . (X.XX > X.XX)\n") + strlen(place_name) + 11;    // 10 is the maximum agent.id length (+\0)
        char *buffer = malloc(sizeof(char) * buffer_size);
        if (buffer == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for buffer to log operation\n");
            return;
        }

        if (simulation.verbose) {
            sprintf(buffer, "Agent %d has recovered at %s. (%.2f > %.2f)\n", (*agent).id, place_name, roll, duration);
        }
        else {
            sprintf(buffer, "Agent %d has recovered at %s.\n", (*agent).id, place_name);
        }
        write_to_log(result, buffer);
        free(buffer);
    }
}

void illness_progress_step(struct virus virus, struct simulation simulation, struct world *world, struct result *result, int *curr_infected)
{
    // iterate over all the places
    for (int place_i = 0; place_i <= (*world).max_index; place_i++) {
        if ((*world).places[place_i].id == -1) {
            continue;
        }

        struct place *current_place = &((*world).places[place_i]);
        for (struct node *current_node = (*(*current_place).agents).head; current_node != NULL; current_node = (*current_node).next) {
            if ((*(*current_node).agent).state == infected) {
                agent_illness_progress_step((*current_node).agent, virus, simulation, world, result, curr_infected);
            }
            // else healthy or dead -> continue
        }
    }
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Spread virus functions           *
         * * * * * * * * * * * * * * * * * * * * * * */


void place_spread_virus_step(struct place *place, struct virus virus, struct simulation simulation, struct result *result, int *curr_infected)
{
    // iterate over all the agents in the place from lowest id to highest id - order is determined by the linked list
    float infectivity = virus.infectivity;

    // create an array of ids of infected agents in the place, maximum number of infected agents is the number of agents in the place
    int agents_infected[(*place).num_agents];
    struct node *current_node = (*(*place).agents).head;
    int i = 0;
    while (current_node != NULL) {
        struct agent *current_agent = (*current_node).agent;
        if ((*current_agent).state == infected) {
            agents_infected[i] = (*current_agent).id;
            i++;
        }
        current_node = (*current_node).next;
    }

    // iterate over the infected agents and spread the virus to the healthy agents, i is the number of infected agents
    for (int j = 0; j < i; j++) {
        int infected_agent_id = agents_infected[j];

        // iterate over all the agents in the place through the linked list
        // if the agent is healthy, try to infect it
        current_node = (*(*place).agents).head;
        while (current_node != NULL) {
            struct agent *current_agent = (*current_node).agent;
            if ((*current_agent).state == healthy) {
                float immunity = (*current_agent).immunity;
                // if agent is vaccinated, increase immunity by vaccine_modifier
                if ((*current_agent).is_vaccinated) {
                    immunity = immunity * virus.vaccine_modifier;
                }


                // check if the agent gets infected
                double roll = (double) rand() / RAND_MAX;
                double final_roll = (*place).exposure * roll * infectivity;
                bool is_infected = final_roll - immunity > EPS;
                if (is_infected) {
                    (*current_agent).state = infected;
                    (*result).total_infections++;
                    (*place).total_infected_agents++;
                    (*curr_infected)++;


                    // write to the simulation log
                    int buffer_size = strlen("Agent  (R: X.XX) has infected agent  (I: X.XX, V) at  (E: X.XX). (X.XX > X.XX)\n") + strlen((*place).name) + 21;    // 10 is the maximum agent.id length (+\0)
                    char *buffer = malloc(sizeof(char) * buffer_size);
                    if (buffer == NULL) {
                        fprintf(stderr, "Error: Could not allocate memory for buffer to log operation\n");
                        return;
                    }

                    if (simulation.verbose) {
                        if ((*current_agent).is_vaccinated)
                            sprintf(buffer, "Agent %d (R: %.2f) has infected agent %d (I: %.2f, V) at %s (E: %.2f). (%.2f > %.2f)\n", infected_agent_id, roll, (*current_agent).id, (*current_agent).immunity, (*place).name, (*place).exposure, final_roll, immunity);
                        else
                            sprintf(buffer, "Agent %d (R: %.2f) has infected agent %d (I: %.2f) at %s (E: %.2f). (%.2f > %.2f)\n", infected_agent_id, roll, (*current_agent).id, (*current_agent).immunity, (*place).name, (*place).exposure, final_roll, immunity);
                    }
                    else {
                        sprintf(buffer, "Agent %d has infected agent %d at %s.\n", infected_agent_id, (*current_agent).id, (*place).name);
                    }
                    write_to_log(result, buffer);
                    free(buffer);
                }


            }
            current_node = (*current_node).next;
        }   
    }
}

void spread_virus_step(struct world *world, struct virus virus, struct simulation simulation, struct result *result, int *curr_infected)
{
    // iterate over all the places from lowest id to highest id
    for (int i = 0; i <= (*world).max_index; i++) {
        if ((*world).places[i].id == -1) {
            continue;
        }

        struct place *current_place = &(*world).places[i];
        place_spread_virus_step(current_place, virus, simulation, result, curr_infected);
    }
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Main simulate function           *
         * * * * * * * * * * * * * * * * * * * * * * */


struct result *simulate(struct virus virus, struct simulation simulation, struct world *world, struct agents *agents)
{
    struct result *result = malloc(sizeof(struct result));                 // will not be freed in the end, but the memory will be freed by the caller
    if (result == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for result\n");
        return NULL;
    }
    int log_capacity = 128;
    char *simulation_log = malloc(sizeof(char) * log_capacity);            // will not be freed in the end, but the memory will be freed by the caller
    if (simulation_log == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for simulation log\n");
        free(result);
        return NULL;
    }
    simulation_log[0] = '\0';   // initialize the string with '\0' for strcat to work correctly

    (*result).simulation_log = simulation_log;
    (*result).log_capacity = log_capacity;
    (*result).log_size = 0;
    (*result).result = step_limit;              // will be updated if the simulation ends for a different reason
    (*result).total_infections = 0;
    (*result).total_deaths = 0;
    (*result).survivors = 0;                    // will be updated at the end of the simulation: survivors = num_agents - total_deaths
    (*result).most_infected_place = -1;         // will be updated at the end of the simulation
    (*result).steps = 0;

    if (simulation.random_seed < 0) {
        // use default value here
        srand(time(NULL));
    }
    else {
        srand(simulation.random_seed);
    }

    // main simulation loop
    int curr_infected = infected_count(agents);
    while (((*result).steps < simulation.max_steps || simulation.max_steps == -1) && (*agents).num_agents > (*result).total_deaths && curr_infected > 0) {
        write_step_to_log(result);

        move_agents_step(agents, world);

        illness_progress_step(virus, simulation, world, result, &curr_infected);

        spread_virus_step(world, virus, simulation, result, &curr_infected);
        (*result).steps++;
    }
    
    // update the result
    if ((*result).steps == simulation.max_steps) {
        (*result).result = step_limit;
    }
    else if ((*agents).num_agents == (*result).total_deaths) {
        (*result).result = agents_dead;
    }
    else {
        (*result).result = virus_dead;
    }
    (*result).survivors = (*agents).num_agents - (*result).total_deaths;

    // find most infected place
    int most_infected1_infections = 0;
    int most_infected2_infections = 0;
    int id_most_infected1 = -1;
    for (int i = 0; i <= (*world).max_index; i++) {
        if ((*world).places[i].id == -1) {
            continue;
        }

        if ((*world).places[i].total_infected_agents > most_infected1_infections) {
            most_infected2_infections = most_infected1_infections;
            most_infected1_infections = (*world).places[i].total_infected_agents;
            id_most_infected1 = i;
        }
        else if ((*world).places[i].total_infected_agents > most_infected2_infections) {
            most_infected2_infections = (*world).places[i].total_infected_agents;
        }
    }
    if (most_infected1_infections == most_infected2_infections) {
        // multiple places have the same number of infected agents
        (*result).most_infected_place = -1;
    }
    else {
        (*result).most_infected_place = id_most_infected1;
    }

    return result;
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Other helper functions           *
         * * * * * * * * * * * * * * * * * * * * * * */
    

void write_to_log(struct result *result, char *line)
{
    int line_length = strlen(line);
    if (((*result).log_size + line_length) >= (*result).log_capacity) {
        int new_capacity = (*result).log_capacity + 128;
        char *new_log = realloc((*result).simulation_log, sizeof(char) * new_capacity);
        if (new_log == NULL) {
            fprintf(stderr, "Error: Could not reallocate memory for simulation log\n");
            return;
        }
        (*result).simulation_log = new_log;
        (*result).log_capacity = new_capacity;
    }
    strcat((*result).simulation_log, line);
    (*result).log_size += strlen(line) + 1; // +1 for the null terminator
}

void write_step_to_log(struct result *result)
{
    int buffer_size = strlen("\n*** STEP  ***\n") + 20;   // maximum number of digits for an integer
    char *buffer = malloc(sizeof(char) * buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for buffer to log operation\n");
        return;
    }
    snprintf(buffer, buffer_size, "\n*** STEP %d ***\n", ((*result).steps + 1));
    write_to_log(result, buffer);
    free(buffer);
}

int infected_count(struct agents *agents)
{
    int count = 0;
    for (int i = 0; i <= (*agents).max_index; i++) {
        if ((*agents).agents_array[i].id == -1) {
            continue;
        }

        if (((*agents).agents_array[i]).state == infected) {
            count++;
        }
    }
    return count;
}

void free_result(struct result *result)
{
    if (result == NULL) {
        fprintf(stderr, "Error: result is NULL\n");
        return;
    }

    free((*result).simulation_log);
    free(result);
}

void print_result(struct world *world, struct result *result, struct simulation simulation)
{
    if (result == NULL) {
        fprintf(stderr, "Error: result is NULL\n");
        return;
    }

    printf("Random seed: %d\n", simulation.random_seed);
    if (!simulation.summary) {
        printf("%s", (*result).simulation_log);
    }

    switch ((*result).result)
    {
    case step_limit:
        printf("Step limit expired.\n");
        break;
    case agents_dead:
        printf("Population is extinct.\n");
        break;
    case virus_dead:
        printf("Virus is extinct.\n");
        break;
    }

    printf("Statistics:\n");
    printf("\tTotal infections: %zu\n", (*result).total_infections);
    printf("\tTotal deaths: %zu\n", (*result).total_deaths);
    printf("\tNumber of survivors: %zu\n", (*result).survivors);


    printf("Most infectious location:\n");
    if ((*result).most_infected_place == -1) {
        printf("\tMultiple\n");
    }
    else {
        printf("\t- %s: %d infections\n", (*world).places[(*result).most_infected_place].name, (*world).places[(*result).most_infected_place].total_infected_agents);
    }

    printf("Simulation terminated after %d steps.\n", (*result).steps);

}
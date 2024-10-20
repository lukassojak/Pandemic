#include "agent_utils.h"






        /* * * * * * * * * * * * * * * * * * * * * * *
         *           Read agents function            *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function read_agents() reads agents from a .csv file and updates the world with the agents.
 * The function creates a struct agents with the information read from the file and updates the world with the agents.
 * If an error occurs, the function returns NULL.
 * The function checks the validity of the route.
*/
struct agents *read_agents(FILE *file, struct world *world)
{
    size_t agents_array_size = 128;
    struct agent *agents_array = malloc(sizeof(struct agent) * agents_array_size);
    if (agents_array == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for agents array\n");
        return NULL;
    }
    // set id to -1 for agents that are not in the world yet
    for (size_t i = 0; i < agents_array_size; i++) {
        agents_array[i].id = -1;
        agents_array[i].route = NULL;
    }

    struct agents *agents = malloc(sizeof(struct agents));
    if (agents == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for agents\n");
        free(agents_array);
        return NULL;
    }

    (*agents).array_size = agents_array_size;
    (*agents).max_index = -1;                // no agents in the world yet = no valid max index
    (*agents).agents_array = agents_array;
    (*agents).num_agents = 0;

    char line[2048];
    while (fgets(line, 2048, file) != NULL) {
        // check if the current line is valid
        if (!agent_line_format_is_valid(line)) {
            fprintf(stderr, "Error: Invalid format in agent input file, line n.: %d\n", (int) (*agents).num_agents);
            free_agents(agents);
            return NULL;
        }


        // init new agent
        struct agent agent;
        agent.id = atoi(strtok(line, ";"));              // id
        char *route_token = strtok(NULL, ";");           // route
        char *vac_token = strtok(NULL, ";");             // is_vaccinated
        char *imm_token = strtok(NULL, ";");             // immunity
        char *inf_token = strtok(NULL, ";");             // is_infected
        agent.route = parse_route(route_token, &agent.route_length, world);
        if (agent.route == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for agent route\n");
            free_agents(agents);
            return NULL;
        }
        agent.curr_route_idx = 0;
        agent.is_vaccinated = atoi(vac_token);
        agent.immunity = atof(imm_token);
        agent.state = atoi(inf_token);                  // 0 - healthy, 1 - infected, 2 - dead


        // reallocate agents_array if needed
        if ((int) (*agents).array_size <= agent.id) {
            size_t new_array_size = agent.id + 128;
            struct agent *tmp = realloc((*agents).agents_array, sizeof(struct agent) * new_array_size);
            if (tmp == NULL) {
                fprintf(stderr, "Error: Could not allocate memory for agents array\n");
                free(agent.route);
                free_agents(agents);
                return NULL;
            }
            // set id to -1 for agents that are not in the world yet
            for (size_t i = (*agents).array_size; i < new_array_size; i++) {
                tmp[i].id = -1;
                tmp[i].route = NULL;
            }

            (*agents).agents_array = tmp;
            (*agents).array_size = new_array_size;
            tmp = NULL;
        }


        // add new agent to the agents_array
        if ((*agents).agents_array[agent.id].id != -1) {
            fprintf(stderr, "Error: Agent with id %d already exists\n", agent.id);
            free(agent.route);
            free_agents(agents);
            return NULL;
        }
        (*agents).agents_array[agent.id] = agent;
        (*agents).num_agents++;
        if (agent.id > (*agents).max_index) {
            (*agents).max_index = agent.id;
        }
    }

    if ((*agents).num_agents == 0) {
        fprintf(stderr, "Error: No agents in the world\n");
        free_agents(agents);
        return NULL;
    }

    bool succ = insert_agents_to_places(agents, world);
    if (!succ) {
        fprintf(stderr, "Error: Could not insert agent into place\n");
        free_agents(agents);
        return NULL;
    }

    return agents;
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *         Route process functions           *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function agent_line_format_is_valid() checks whether string 'line' has required format
*/
bool agent_line_format_is_valid(char line[2048])
{
    char line_copy[2048];
    strcpy(line_copy, line);
    // check id presence and validity
    char *token = strtok(line_copy, ";");
    if (token == NULL) {
        return false;
    }
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i])) {
            return false;
        }
    }
    // check route presence and validity
    token = strtok(NULL, ";");
    if (token == NULL) {
        return false;
    }
    // check if route is valid, example of valid format: 1-2-14-3-4 
    bool is_previous_dash = true;
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i]) && token[i] != '-') {
            return false;
        }
        if (token[i] == '-') {
            if (is_previous_dash) {
                return false;
            }
            is_previous_dash = true;
        } else {
            is_previous_dash = false;
        }
    }
    // check is_vaccinated presence and validity
    token = strtok(NULL, ";");
    if (token == NULL) {
        return false;
    }
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i])) {
            return false;
        }
    }
    // check immunity presence and validity
    token = strtok(NULL, ";");
    if (token == NULL) {
        return false;
    }
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i]) && token[i] != '.') {
            return false;
        }
    }
    // check is_infected presence and validity
    token = strtok(NULL, ";");
    if (token == NULL) {
        return false;
    }
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i]) && token[i] != '\n') {
            return false;
        }
    }

    return true;
}

/*
 * The function parse_route() parses the route string and returns an array of integers.
 */
int *parse_route(char *route, int *route_length, struct world *world)
{
    int route_array_size = 256;
    int *route_array = malloc(sizeof(int) * route_array_size);
    if (route_array == NULL) {
        return NULL;
    }

    char *token = strtok(route, "-");
    int i = 0;
    while (token != NULL) {
        if (i >= route_array_size) {
            int new_size = route_array_size + 256;
            int *tmp = realloc(route_array, sizeof(int) * new_size);
            if (tmp == NULL) {
                free(route_array);
                return NULL;
            }
            route_array = tmp;
            route_array_size = new_size;
        }
        route_array[i] = atoi(token);

        // check if the place id is valid
        if ((route_array[i] < 0) || (route_array[i] > (*world).max_index) || ((*world).places[route_array[i]].id == -1)) {
            printf("Error: Invalid place id: %d in route\n", route_array[i]);
            free(route_array);
            return NULL;
        }

        token = strtok(NULL, "-");
        i++;
    }
    *route_length = i;

    return route_array;
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Other helper functions           *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function insert_agents_to_places() inserts agents to the places in the world.
 * The function receives a pointer to a struct agents and a pointer to a struct world.
 * The function inserts pointers to the agents to the places in the world according to the route of the agents.
 * The function returns true if the insertion was successful, false otherwise.
*/
bool insert_agents_to_places(struct agents *agents, struct world *world)
{
    for (int i = 0; i <= (*agents).max_index; i++) {
        if ((*agents).agents_array[i].id == -1) {
            continue;
        }

        int current_place_id = (*agents).agents_array[i].route[(*agents).agents_array[i].curr_route_idx];
        struct agent *agent_to_insert = &(*agents).agents_array[i];
        bool succ = insert2((*world).places[current_place_id].agents, agent_to_insert);
        if (!succ) {
            return false;
        }
        (*world).places[current_place_id].num_agents++;
    }
    return true;
    
}

/*
 * The function free_agents() frees the memory allocated for a struct agents.
 * The function receives a pointer to a struct agents and frees the memory allocated for the struct and its fields.
 * In the struct world every struct place after calling free_agents() will have a dangling pointer to the agent.
 * Therefore, the function free_world() should be called after free_agents().
*/
void free_agents(struct agents *agents)
{
    if (agents == NULL) {
        fprintf(stderr, "Error: agents is NULL\n");
        return;
    }
    for (int i = 0; i <= (*agents).max_index; i++) {
        if ((*agents).agents_array[i].id == -1) {
            continue;
        }

        free((*agents).agents_array[i].route);
        (*agents).agents_array[i].route = NULL;
    }

    free((*agents).agents_array);
    free(agents);

}
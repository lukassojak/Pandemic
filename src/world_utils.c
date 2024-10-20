#include "world_utils.h"

const int MAX_LINE_SIZE = 128;






        /* * * * * * * * * * * * * * * * * * * * * * *
         *            Read world function            *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function read_world() reads a world from a .csv file.
 * The function returns a pointer to a struct world with the information read from the file.
 * If an error occurs, the function returns NULL.
 */
struct world *read_world(FILE *file)
{
    size_t places_array_size = 128;
    struct place *places_array = malloc(sizeof(struct place) * places_array_size);
    if (places_array == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for places array\n");
        return NULL;
    }
    // set id to -1 for places that are not in the world yet
    for (size_t i = 0; i < places_array_size; i++) {
        places_array[i].id = -1;
        places_array[i].name = NULL;
    }

    struct world *world = malloc(sizeof(struct world));
    if (world == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for world\n");
        free(places_array);
        return NULL;
    }

    (*world).array_size = places_array_size;
    (*world).max_index = -1;                // no places in the world yet = no valid max index
    (*world).num_places = 0;                // no places in the world yet
    (*world).places = places_array;
    places_array = NULL;

    char line[MAX_LINE_SIZE];
    while (fgets(line, MAX_LINE_SIZE, file) != NULL) {
        // check if current line is valid
        if (!world_line_format_is_valid(line)) {
            fprintf(stderr, "Error: Invalid format in world input file, line n.: %d\n", (int) (*world).num_places);
            free_world(world);
            return NULL;
        }

        // init new place
        struct place place;
        place.id = atoi(strtok(line, ";"));
        char *token = strtok(NULL, ";");
        place.name = malloc(strlen(token) + 1);
        if (place.name == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for place name\n");
            free_world(world);
            return NULL;
        }
        strcpy(place.name, token);
        place.exposure = atof(strtok(NULL, ";"));
        place.total_infected_agents = 0;
        place.num_agents = 0;
        place.agents = create_linked_list2();
        if (place.agents == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for place agents\n");
            free(place.name);
            free_world(world);
            return NULL;
        }


        // reallocate memory for places array if needed
        if ((int) (*world).array_size <= place.id) {
            size_t new_array_size = place.id + 128;
            struct place *new_places_array = realloc((*world).places, sizeof(struct place) * new_array_size);
            if (new_places_array == NULL) {
                fprintf(stderr, "Error: Could not reallocate memory for places array\n");
                free(place.name);
                free_linked_list2(place.agents);
                free_world(world);
                return NULL;
            }
            // set id to -1 for places that are not in the world yet
            for (size_t i = (*world).array_size; i < new_array_size; i++) {
                new_places_array[i].id = -1;
                new_places_array[i].name = NULL;
            }
            (*world).places = new_places_array;
            (*world).array_size = new_array_size;
            new_places_array = NULL;
        }

        // add new place to the world
        if ((*world).places[place.id].id != -1) {
            fprintf(stderr, "Error: Place with id %d already exists\n", place.id);
            free(place.name);
            free_linked_list2(place.agents);
            free_world(world);
            return NULL;
        }
        (*world).places[place.id] = place;
        (*world).num_places++;
        if (place.id > (*world).max_index) {
            (*world).max_index = place.id;
        }
    }
    return world;
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *          Other helper functions           *
         * * * * * * * * * * * * * * * * * * * * * * */

        
/*
 * The function free_world() frees the memory allocated for a world.
 * The function receives a pointer to a struct world and frees the memory allocated for the struct and its fields.
 */
void free_world(struct world *world)
{
    if (world == NULL) {
        fprintf(stderr, "Error: world is NULL\n");
        return;
    }
    for (int i = 0; i <= (*world).max_index; i++) {
        if ((*world).places[i].id == -1) {
            continue;
        }

        free((*world).places[i].name);                      // free name of each place
        free_linked_list2((*world).places[i].agents);       // free linked list of agents in each place
    }

    free((*world).places);
    free(world);
}

/*
 * The function world_line_format_is_valid() checks whether string 'line' has required format
 */
bool world_line_format_is_valid(char line[128])
{
    char line_copy[MAX_LINE_SIZE];
    strcpy(line_copy, line);
    // check id presence and validity
    char *token = strtok(line_copy, ";");
    if (token == NULL) {
        fprintf(stderr, "Error: In input world file ID column not found.\n");
        return false;
    }
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i])) {
            fprintf(stderr, "Error: In input world file ID is not valid number.\n");
            return false;
        }
    }
    // check name presence and validity
    token = strtok(NULL, ";");
    if (token == NULL) {
        fprintf(stderr, "Error: In input world file Name column not found.\n");
        return false;
    }
    // check exposure presence and validity
    token = strtok(NULL, ";");
    if (token == NULL) {
        fprintf(stderr, "Error: In input world file Exposure column not found.\n");
        return false;
    }
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i]) && token[i] != '.' && token[i] != '\n') {
            fprintf(stderr, "Error: In input world file Exposure is not valid number.\n");
            return false;
        }
    }
    // check if there are no more columns
    token = strtok(NULL, ";");
    return token == NULL;
}
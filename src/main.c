#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h> // for getopts
#include <getopt.h> // for getopt_long - longopts
#include <stdbool.h> // for bool type
#include <string.h> // for strcmp
#include <float.h> // for FLT_MAX
#include <limits.h> // for INT_MAX

#include "world_utils.h"
#include "simulation.h"
#include "agent_utils.h"






        /* * * * * * * * * * * * * * * * * * * * * * *
         *      Number validity check functions      *
         * * * * * * * * * * * * * * * * * * * * * * */


bool is_valid_float_not_negative(const char* arg, float max)
{
    char *endptr;
    float value = strtof(arg, &endptr);
    if (endptr == arg || value < 0 || value > max) {
        fprintf(stderr, "Invalid float argument!\n");
        return false;
    }

    return true;
}

bool is_valid_int_not_negative(const char* arg, int max)
{
    char *endptr;
    int value = strtol(arg, &endptr, 10);
    if (endptr == arg || value < 0 || value > max) {
        fprintf(stderr, "Invalid integer argument!\n");
        return false;
    }

    return true;
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *             Parsing functions             *
         * * * * * * * * * * * * * * * * * * * * * * */


/*
 * The function check_and_use_option() checks the validity of the option and uses it to update the virus or simulation struct.
 */
bool check_and_use_option(const char* option, const char* arg, struct virus *virus, struct simulation *simulation)
{
    if (strcmp(option, "lethality") == 0) {
        // handle lethality option - takes a float argument in range (0, 1)
        if (!is_valid_float_not_negative(arg, 1.0)) {
            return false;
        }
        (*virus).lethality = strtof(arg, NULL);

    } else if (strcmp(option, "infectivity") == 0) {
        // handle infectivity option - takes a float argument in range (0, 1)
        if (!is_valid_float_not_negative(arg, 1.0)) {
            return false;
        }
        (*virus).infectivity = strtof(arg, NULL);

    } else if (strcmp(option, "duration") == 0) {
        // handle duration option - takes a float argument in range (0, 1)
        if (!is_valid_float_not_negative(arg, 1.0)) {
            return false;
        }
        (*virus).duration = strtof(arg, NULL);

    } else if (strcmp(option, "vaccine-modifier") == 0) {
        // handle vaccine-modifier option - takes a float greater than 0
        if (!is_valid_float_not_negative(arg, FLT_MAX)) {
            return false;
        }
        (*virus).vaccine_modifier = strtof(arg, NULL);

    } else if (strcmp(option, "max-steps") == 0) {
        // handle max-steps option - takes an int argument greater than 0
        if (!is_valid_int_not_negative(arg, INT_MAX)) {
            return false;
        }
        (*simulation).max_steps = strtol(arg, NULL, 10);

    } else if (strcmp(option, "random-seed") == 0) {
        // handle random-seed option - takes an int argument greater than 0
        if (!is_valid_int_not_negative(arg, INT_MAX)) {
            return false;
        }
        (*simulation).random_seed = strtol(arg, NULL, 10);

    } else if (strcmp(option, "summary") == 0) {
        // handle summary option - takes no argument
        (*simulation).summary = true;

    } else if (strcmp(option, "verbose") == 0) {
        // handle verbose option - takes no argument
        (*simulation).verbose = true;

    } else {
        fprintf(stderr, "Unknown option\n");
        return false;
    }
    return true;
}

/*
 * The function parse_options() parses the options from the command line and updates the virus and simulation structs.
 */
bool parse_options(int argc, char *argv[], struct virus *virus, struct simulation *simulation)
{
    int c;
    while (1) {  // loop ends at break - no more options
        int option_index = 0;
        static struct option long_options[] = {
            {"lethality", required_argument, NULL, 0 },
            {"infectivity", required_argument, NULL, 0 },
            {"duration", required_argument, NULL, 0 },
            {"vaccine-modifier", required_argument, NULL, 0 },
            {"max-steps", required_argument, NULL, 0 },
            {"random-seed", required_argument, NULL, 0 },
            {"summary", no_argument, NULL, 0 },
            {"verbose", no_argument, NULL, 0 },
            {0, 0, 0, 0 }
        };

        c = getopt_long(argc, argv, "", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0: // valid long option
                if (!check_and_use_option(long_options[option_index].name, optarg, virus, simulation)) {
                    return false;
                }
                break;

            default: // invalid option
                fprintf(stderr, "Invalid option ... ");
                return false;
        }
    }

    return true;
}

/*
 * The function parse_non_option_arguments() parses the non-option arguments from the command line.
 */
bool parse_non_option_arguments(int argc, char *argv[], char** agent_file_path, char** world_file_path) {
    // check for non-option arguments
    if (optind + 2 != argc) {
        fprintf(stderr, "Invalid number of arguments\n");
        return false;
    }

    *agent_file_path = argv[optind];
    *world_file_path = argv[optind + 1];
    return true;
}






        /* * * * * * * * * * * * * * * * * * * * * * *
         *               Main function               *
         * * * * * * * * * * * * * * * * * * * * * * */


int main(int argc, char *argv[])
{
    // TODO: Have fun

    struct virus virus = {0.5, 0.5, 0.5, 1.2};
    struct simulation simulation = {-1, time(NULL), false, false};

    bool succ = parse_options(argc, argv, &virus, &simulation);
    if (!succ) {
        return EXIT_FAILURE;
    }
    if (simulation.summary && simulation.verbose) {
        fprintf(stderr, "Cannot have both summary and verbose options\n");
        return EXIT_FAILURE;
    }
    // now virus and simulation structs are NOT supposed to be changed

    char *agent_file_path;
    char *world_file_path;
    succ = parse_non_option_arguments(argc, argv, &agent_file_path, &world_file_path);
    if (!succ) {
        return EXIT_FAILURE;
    }

    // open files
    FILE *agent_file = fopen(agent_file_path, "r");
    if (agent_file == NULL) {
        fprintf(stderr, "Error: Could not open agent file %s\n", agent_file_path);
        return EXIT_FAILURE;
    }

    FILE *world_file = fopen(world_file_path, "r");
    if (world_file == NULL) {
        fprintf(stderr, "Error: Could not open world file %s\n", world_file_path);
        fclose(agent_file);
        return EXIT_FAILURE;
    }

    // read files
    struct world *world = read_world(world_file);
    if (world == NULL) {
        fprintf(stderr, "Error: Could not read world file %s\n", world_file_path);
        fclose(agent_file);
        fclose(world_file);
        return EXIT_FAILURE;
    }
    struct agents *agents = read_agents(agent_file, world);
    if (agents == NULL) {
        fprintf(stderr, "Error: Could not read agent file %s\n", agent_file_path);
        free_world(world);
        fclose(agent_file);
        fclose(world_file);
        return EXIT_FAILURE;
    }

    fclose(world_file);
    fclose(agent_file);

    // simulate
    struct result *result = simulate(virus, simulation, world, agents);
    if (result == NULL) {
        fprintf(stderr, "Error: simulation was not successful\n");
        free_world(world);
        free_agents(agents);
        return EXIT_FAILURE;
    }

    print_result(world, result, simulation);

    free_result(result);
    free_world(world);
    free_agents(agents);
    return EXIT_SUCCESS;
}

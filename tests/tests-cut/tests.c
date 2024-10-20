#include "lib/cut.h"
#include "../../src/main.c" // Assuming the functions to test are in this file

TEST(example) {
    /* Common code. */
    int x = 1;

    SUBTEST(example_1) {
        CHECK(x == 1);
    }

    SUBTEST(example_2) {
        CHECK(x != 2);
    }
}

// Test if the program handles no arguments correctly
TEST(no_arguments) {
    int argc = 1;
    char *argv[] = {"program"};
    struct virus virus = {0.5, 0.5, 0.5, 1.2};
    struct simulation simulation = {-1, time(NULL), false, false};
    CHECK(parse_arguments(argc, argv, &virus, &simulation) == false);
}

// Test if the program handles invalid options correctly
TEST(invalid_options) {
    int argc = 2;
    char *argv[] = {"program", "-z"};
    struct virus virus = {0.5, 0.5, 0.5, 1.2};
    struct simulation simulation = {-1, time(NULL), false, false};
    CHECK(parse_arguments(argc, argv, &virus, &simulation) == false);
}

// Test if the program handles valid options correctly
TEST(valid_options) {
    int argc = 3;
    char *argv[] = {"program", "--lethality", "0.6"};
    struct virus virus = {0.5, 0.5, 0.5, 1.2};
    struct simulation simulation = {-1, time(NULL), false, false};
    CHECK(parse_arguments(argc, argv, &virus, &simulation) == true);
    CHECK(virus.lethality == 0.6);
}

// Test if the program handles invalid arguments correctly
TEST(invalid_arguments) {
    int argc = 3;
    char *argv[] = {"program", "--lethality", "invalid"};
    struct virus virus = {0.5, 0.5, 0.5, 1.2};
    struct simulation simulation = {-1, time(NULL), false, false};
    CHECK(parse_arguments(argc, argv, &virus, &simulation) == false);
}

// Test if the program handles valid arguments correctly
TEST(valid_arguments) {
    int argc = 3;
    char *argv[] = {"program", "--lethality", "0.6"};
    struct virus virus = {0.5, 0.5, 0.5, 1.2};
    struct simulation simulation = {-1, time(NULL), false, false};
    CHECK(parse_arguments(argc, argv, &virus, &simulation) == true);
    CHECK(virus.lethality == 0.6);
}
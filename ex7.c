#include <stdio.h>

int main(int argc, char *argv[])
{
    int bugs = 100;
    double bug_rate = 1.2;

    printf("You have %d bugs at the imaginary rate of %f.\n", bugs, bug_rate);

    long universe_of_defects = 1L * 1024L * 1024L * 1024L;
    printf("The entire universe has %ld bugs.\n", universe_of_defects);

    double expected_bugs = bugs * bug_rate;
    printf("You are expected to have %f bugs.\n", expected_bugs);

    double part_of_universe = expected_bugs / universe_of_defects;
    printf("That is only a %e portion of the universe.\n", part_of_universe);

    // this makes no sense, just a demo of something weird
    char nul_byte = '\0';
    int care_percentage = bugs * nul_byte;
    printf("Which means you should care %d%%.\n", care_percentage);

    // store an integer and then print using the %d and %c formatters
    int ascii_int = 90;
    printf("int %d printed as d: %d\n", ascii_int, ascii_int);
    printf("int %d printed as c: %c\n", ascii_int, ascii_int);

    char upper_S = 'S';
    int lower_s = upper_S + 32;
    printf("as c:\n\tupper_S: %c\n\tlower_s: %c\n", upper_S, lower_s);
    printf("as d:\n\tupper_S: %d\n\tlower_s: %d\n", upper_S, lower_s);

    return 0;
}

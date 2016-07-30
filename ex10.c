#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;

    // go through each string in argv
    // argv[0] is the program name
    for(i=0; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    // make our own array of strings
    char *states[] = {"California", "Oregon", "Washington", "North Carolina"};
    int num_states = 4;

    // trying to make a null pointer and put it in the states array
    char *mynull = 0;
    states[0] = mynull;

    for(i=0; i < num_states; i++){
        printf("state %d: s: %s , p: %p\n", i, states[i], states[i]);
    }

    return 0;
}

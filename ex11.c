#include <stdio.h>

int main(int argc, char *argv[])
{
    // go through each string in argv
    int i = 0;
    while(i < argc){
        printf("arg %d: %s\n", i, argv[i]);
        i++;
    }

    // make our own array of strings
    char *states[] = {"New York", "South Carolina", "Georgia", "Hawaii"};
    int num_states = 4;
    i =0;
    while(i < num_states){
        printf("state %d: %s\n", i, states[i]);
        i++;
    }

    // copy first 3 values from argv into recorded and print in reverse order; null if no arg
    char *n = 0;
    char *recorded[4] = {n, n, n, n};
    i = 0;
    while(i < 4 && i < argc){
        recorded[i] = argv[i];
        i++;
    }

    i = 4;
    while(i >= 0){
        printf("arg %d: %s\n", i, recorded[i]);
        i--;
    }

    return 0;
}

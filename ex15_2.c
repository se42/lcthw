// my custom practice/extra credit section for concepts covered in ex15

#include <stdio.h>


void print_1(int index, char **ptr)
{
    printf("arg %d: %s from pointer at %p / %p\n", index, ptr[index], ptr, ptr[index]);
}



int main(int argc, char *argv[])
{
    // normal way to process command line args
    int i = 0;
    for(i = 0; i < argc; i++){
        printf("arg %d: %s\n", i, argv[i]);
    }

    printf("---\n");

    // process command line args with pointers
    char **arg = argv;
    for(i = 0; i < argc; i++){
        // printf("arg %d: %s at %p\n", i, *(arg+i), arg[i]);
        print_1(i, arg);
    }
}

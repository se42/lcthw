#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;

    if(argc == 1){
        printf("You have no arguments.\n");
    } else if(argc > 1 && argc < 4){
        printf("Here are your arguments:\n");

        for(i = 1; i < argc; i++){
            printf("\targ %d: %s\n", i, argv[i]);
        }
    } else {
        printf("You have too many arguments for me to care...\n");
    }

    int nums[] = {1,2,3,4,5,6};
    i = 0;
    while(i < 6){
        printf("num: %d\n", nums[i]);
        i++;
        if(i == 4){
            break;
        }
    }

    return 0;
}

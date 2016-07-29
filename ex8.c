#include <stdio.h>

int main(int argc, char *argv[])
{
    int areas[] = {10, 12, 13, 14, 20};
    areas[1] = 200;
    char name[] = "Scott";
    name[2] = 'O';
    name[4] = 87;
    areas[0] = name[0];
    char full_name[] = {'S', 'c', 'o', 't', 't', ' ', 'M', '.', ' ',
        'E', 'd', 'w', 'a', 'r', 'd', 's', '\0'};

    // WARNING: On some systems you may have to change the
    // %ld in this code to a %u since it will use unsigned ints
    printf("The size of an int: %ld\n", sizeof(int));
    printf("The size of 'int areas[]': %ld\n", sizeof(areas));
    printf("The number of ints in areas: %ld\n", sizeof(areas) / sizeof(int));
    printf("The first area is %d, the second is %d\n", areas[0], areas[1]);

    printf("The size of a character: %ld\n", sizeof(char));
    printf("The size of 'char name[]': %ld\n", sizeof(name));
    printf("The number of characters in name: %ld\n", sizeof(name) / sizeof(char));

    printf("The size of 'char full_name[]': %ld\n", sizeof(full_name));
    printf("The number of characters in full_name: %ld\n", sizeof(full_name) / sizeof(char));

    printf("name=\"%s\" and full_name=\"%s\"\n\n", name, full_name);

    printf("The size of an int*: %ld\n", sizeof(int*));
    printf("The size of a long: %ld\n", sizeof(long));
    printf("The size of a short: %ld\n", sizeof(short));

    return 0;
}

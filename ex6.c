#include <stdio.h>

int main(int argc, char *argv[])
{
    int distance = 100;
    float power = 2.345f;
    double super_power = 56789.4532;
    char initial = 'M';
    char first_name[] = "Scott";
    char last_name[] = "Edwards";

    printf("You are %d miles away.\n", distance);
    printf("You have %f levels of power.\n", power);
    printf("You have %f super powers.\n", super_power);
    printf("I have the middle initial %c.\n", initial);
    printf("I have the first name %s.\n", first_name);
    printf("I have the last name %s.\n", last_name);
    printf("My whole name is %s %c. %s.\n", first_name, initial, last_name);

    return 0;
}

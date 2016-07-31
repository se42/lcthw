#include <stdio.h> // standard input/output library functions
#include <assert.h> // expression verification macro; terminates program if assertion fails
#include <stdlib.h> // stdlib.h includes malloc and free functions
#include <string.h> // includes strdup function which returns a pointer to a duplicate of the input string

// alternate declaration using typedef
typedef struct
{
    char *name;
    int age;
    int height;
    int weight;
} Person;

// function takes Person to operate on
void Person_print(Person who)
{
    // feed the *name pointer to the %s formatter
    printf("Name: %s\n", who.name);
    // feed the int variables to the %d formatter
    printf("\tAge: %d\n", who.age);
    printf("\tHeight: %d\n", who.height);
    printf("\tWeight: %d\n", who.weight);
}

int main(int argc, char *argv[])
{
    // make two Person structures
    Person joe;
    Person scott;

    // make pointers to strings representing their names
    char *joe_name = "Joe on the stack";
    char *scott_name = "Scott on the stack";

    joe.name = joe_name; // struct holds a pointer to a name
    joe.age = 22; // struct holds integers directly
    joe.height = 74;
    joe.weight = 200;

    scott.name = scott_name;
    scott.age = 28;
    scott.height = 72;
    scott.weight = 165;

    // scott and joe are now stack variables, not locations in heap memory
    Person_print(scott);
    Person_print(joe);

    joe.age += 20;
    joe.height -= 2;
    joe.weight += 40;
    Person_print(joe);

    scott.age += 1;
    scott.weight -= 5;
    Person_print(scott);

    return 0;
}

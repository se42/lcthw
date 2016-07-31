#include <stdio.h> // standard input/output library functions
#include <assert.h> // expression verification macro; terminates program if assertion fails
#include <stdlib.h> // stdlib.h includes malloc and free functions
#include <string.h> // includes strdup function which returns a pointer to a duplicate of the input string

// define new struct in terms of variable types of known size
struct Person {
    char *name;
    int age;
    int height;
    int weight;
};

// pointer to function to create a new Person struct
// why is this a pointer??
struct Person *Person_create(char *name, int age, int height, int weight)
{
    // we know how much memory to allocate for a new Person because
    // we only used variable of known size in the struct definition
    struct Person *who = malloc(sizeof(struct Person));
    // check to be sure the memory allocation was successful
    // i.e. malloc returned a pointer to an actual location
    assert(who != NULL);

    // strdup makes a copy of the input string 'name' and returns a ptr to its address
    who->name = strdup(name);
    // int type variables are stored directly in the memory allocated to this Person
    who->age = age;
    who->height = height;
    who->weight = weight;

    // returns a pointer to the memory location of the newly stored Person struct data
    return who;
}

// function takes pointer to memory location of Person to operate on
void Person_destroy(struct Person *who)
{
    // ensure the pointer is not a null pointer
    assert(who != NULL);

    // "look through?" the struct to free the memory pointed to by
    // the name pointer that is stored in the actual struct
    free(who->name);
    // then free the actual struct memory now that the stored name
    // won't be left hanging
    free(who);
}

// function takes pointer to memory location of Person to operate on
void Person_print(struct Person *who)
{
    // feed the *name pointer to the %s formatter
    printf("Name: %s\n", who->name);
    // feed the int variables to the %d formatter
    printf("\tAge: %d\n", who->age);
    printf("\tHeight: %d\n", who->height);
    printf("\tWeight: %d\n", who->weight);
}

int main(int argc, char *argv[])
{
    // make two people structures
    struct Person *joe = Person_create("Joe Alex", 32, 64, 100);
    struct Person *scott = Person_create("Scott Edwards", 28, 72, 170);

    // print them out and where they are in memory
    printf("Scott is at %p\n", scott);
    Person_print(scott);
    printf("Jos is at %p\n", joe);
    Person_print(joe);

    // change some data and print again
    // works because we can directly access and change the int data?
    joe->age += 20;
    joe->height -= 2;
    joe->weight += 40;
    Person_print(joe);

    scott->age += 1;
    scott->weight -= 5;
    Person_print(scott);

    // destroy them both to clean up
    Person_destroy(joe);
    Person_destroy(scott);

    return 0;
}

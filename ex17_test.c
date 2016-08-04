#include <stdio.h>
#include <stdlib.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address {
    int id;
    int set;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

struct Database {
    struct Address rows[MAX_ROWS];
};


// structs for making a variable-size database program
struct Var_address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Var_database {
    int max_data;
    int max_rows;
    struct Var_address *addr_data[];
    // malloc this after reading the first two ints from the file using
    // the CALCULATED address size (NOT the struct size).  This will
    // give you a pointer to an appropriately sized block of data that
    // will be interpreted as Var_address structs.
    //
    // THEN:
    // For each row i up to max_data, you'll have to execute 3 read operations.
    // First, read 2 int objects and write directly to the address pointed to by addr_data[i]
    // Then read a single char[max_data] object to be stored at addr_data[i]->name
    // Then read a single char[max_data] object to be stored at addr_data[i]->email
    //
    // When you write back to the file, you'll first write 2 int objects (max_data & max_rows).
    // Then, for each row i up to max_data, you'll write 2 int objects taken from
    // the address at addr_data[i].
    // Then you'll write 1 char[max_data] object taken from addr_data[i]->name.
    // Then you'll write 1 char[max_data] object taken from addr_data[i]->email.
};

int main(int argc, char *argv[])
{
    int max_data = atoi(argv[1]);
    int max_rows = atoi(argv[2]);

    int db_size = (2 * max_data + 2 * sizeof(int)) * max_rows;

    printf("db_size: %d\n", db_size);
    // printf("sizeof(struct Database): %lu\n", sizeof(struct Database));

    char name[max_data] = "Scott";
    char email[max_data] = "mymail";
    char varstruct[2 * sizeof(int) + sizeof(name) + sizeof(email)];

    int i = 0;



    for(i = 0; i < 7; i++){
        varstruct[i] = name[i];
        varstruct[i+7] = email[i];
    }

    printf("my data: %s , %s\n", name, email);
    printf("sizeof varstruct: %lu\n", sizeof(varstruct));

    printf("varstruct c's: ");
    for(i = 0; i < 14; i++){
        printf("%c", varstruct[i]);
    }
    printf("\n");
    printf("varstruct: %s", varstruct);
    printf("name: %s\n", varstruct);
    printf("email: %s\n", varstruct+7);

    return 0;
}
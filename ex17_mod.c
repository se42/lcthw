/*
Basic database application demonstrating stack and heap memory.
Also proved to be a challenging exercise in pointers and slightly
more advanced structs, as well as an introduction to FILE objects.
Cool to see how data is read from permanent storage and temporarily
held in RAM when you run a program.

This version of the code is where I've played around with some things
and attempted the Extra Credit challenges.

Modifications made to code in ex17.c:
1 - updated die() function to take conn as a parameter, which lets us
    call Database_close(conn) within the die function to eliminate a
    memory leak.  This required naming the Database_close function
    before defining die(), and similarly required the initialization
    of a NULL conn pointer before the first die test in main().  The
    elimination of the memory leak was confirmed using Valgrind.
2 - Change the code to allow MAX_DATA and MAX_ROWS as arguments instead
    of #define-ing them as constants.  NOTE:  These options should only
    apply in 'c' mode and a new 'r/resize' mode.  The parameters should
    be read from the db file in any other mode.

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Database {
    int max_data;
    int max_rows;
    struct Address *rows[];
};

struct Connection {
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn);

void die(const char *message, struct Connection *conn)
{
    if(errno){
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    Database_close(conn);

    exit(1);
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

struct Database *Database_allocate(int max_rows)
{
    struct Database *db = malloc(sizeof(struct Database));
    db->rows = malloc(max_rows * sizeof(struct Address));

    return db;
}

int Database_read_int(FILE *file)
{
    // fread 1 int object from the file
    return 50;
}

char *Database_read_char(FILE *file, int max_data)
{
    // fread a max_data-byte char[] from the file
    return 100
}

void Database_load(struct Connection *conn, int max_data, int max_rows)
{
    // conn->db = {.max_data=max_data, .max_rows=max_rows} or whatever works
    //
    // for i in max_rows
    //   rows[i]->id = Database_read_int(conn->file)
    //   rows[i]->set = Database_read_int(conn->file)
    //   if(set)
    //     rows[i]->name = malloc(max_data)
    //     rows[i]->name = Database_read_char(conn->file, max_data)
    //     rows[i]->email = malloc(max_data)
    //     rows[i]->email = Database_read_char(conn->file, max_data)
    //   else
    //     rows[i]->name = NULL
    //     rows[i]->email = NULL

    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
    if(rc != 1){
        die("Failed to load database.", conn);
    }
}

struct Connection *Database_open(const char *filename, char mode, int max_data, int max_rows)
{
    struct Connection *conn = malloc(sizeof(struct Connection));
    if(!conn){
        die("Memory error", conn);
    }

    if(mode == 'c'){
        conn->db = Database_allocate(max_rows);
        if(!conn->db){
            die("Memory error", conn);
        }
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");

        if(conn->file){
            max_data = Database_read_int(conn->file);
            max_rows = Database_read_int(conn->file);
            conn->db = Database_allocate(max_rows);
            if(!conn->db){
                die("Memroy error", conn);
            }
            Database_load(conn, max_data, max_rows);
        }
    }

    if(!conn->file){
        die("Failed to open the file", conn);
    }

    return conn;
}

void Database_close(struct Connection *conn)
{
    if(conn) {
        if(conn->file){
            fclose(conn->file);
        }

        // this will get a bit more complicated to track down the
        // now-multiple levels of malloc'd memory
        if(conn->db){
            free(conn->db);
        }
        free(conn);
    }
}

void Database_write(struct Connection *conn)
// first write the max_data and max_rows parameters
// then for every row, write the id and set variables
// if the row is set, then write two char[max_data]
// variables for name and email
// if the row is not set, then move on to the next row
{
    rewind(conn->file);

    int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
    if(rc != 1){
        die("Failed to write database", conn);
    }

    rc = fflush(conn->file);
    if(rc == -1){
        die("Cannot flush database", conn);
    }
}

void Database_create(struct Connection *conn)
// we're not saving name/email data for un-set rows, so all
// this needs to do is set the id=i and set=0 so subsequent
// file read operations have the information they need
{
    int i = 0;

    for(i = 0; i < MAX_ROWS; i++) {
        struct Address addr = {.id = i, .set = 0};
        conn->db->rows[i] = addr;
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
    struct Address *addr = &conn->db->rows[id];
    if(addr->set){
        die("Already set, delete it first", conn);
    }

    addr->set = 1;

    char *res = strncpy(addr->name, name, MAX_DATA - 1);
    addr->name[MAX_DATA - 1] = '\0';
    if(!res){
        die("Name copy failed", conn);
    }

    res = strncpy(addr->email, email, MAX_DATA - 1);
    addr->email[MAX_DATA - 1] = '\0';
    if(!res){
        die("Email copy failed", conn);
    }
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = &conn->db->rows[id];

    if(addr->set){
        Address_print(addr);
    } else {
        die("ID is not set", conn);
    }
}

void Database_delete(struct Connection *conn, int id)
{
    struct Address addr = {.id = id, .set = 0};
    conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn)
{
    int i = 0;
    struct Database *db = conn->db;

    for(i = 0; i < MAX_ROWS; i++){
        struct Address *cur = &db->rows[i];

        if(cur->set) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    struct Connection *conn = NULL;

    printf("struct Address: %lu\n", sizeof(struct Address));
    printf("struct Database: %lu\n", sizeof(struct Database));

    if(argc < 3){
        die("USAGE: ex17 <dbfile> <action> [action params]", conn);
    }

    char *filename = argv[1];
    char action = argv[2][0];

    if(action != 'c'){
        conn = Database_open(filename, action, 0, 0);
        int id = 0;
        if(argc > 3){
            id = atoi(argv[3]);
        }
        if(id >= MAX_ROWS){
            die("There aren't that many records", conn);
        }
    }

    switch(action) {
        case 'c':
            if(argc == 5){
                int max_data = atoi(argv[3]);
                int max_rows = atoi(argv[4]);
            } else {
                die("Create/c usage: ex17 <dbfile> c <max_data> <max_rows>", conn);
            }
            conn = Database_open(filename, action, max_data, max_rows);
            Database_create(conn, max_data, max_rows);
            Database_write(conn);
            break;
        case 'g':
            if(argc != 4){
                die("Need an id to get", conn);
            }

            Database_get(conn, id);
            break;
        case 's':
            if(argc != 6){
                die("Need id, name and email to set", conn);
            }

            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn);
            break;
        case 'd':
            if(argc != 4){
                die("Need id to delete", conn);
            }

            Database_delete(conn, id);
            Database_write(conn);
            break;
        case 'l':
            Database_list(conn);
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=del, l=list", conn);
    }

    Database_close(conn);

    return 0;
}


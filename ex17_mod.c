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
2 - Change the code to allow max_data and max_rows as arguments instead
    of #define-ing MAX_DATA and MAX_ROWS as constants.  My solution was
    to use a void pointer for the Database rows variable.  This let me
    dynamically malloc the memory needed after collecting the max_data
    and max_rows variables, and assign the returned pointer to 'rows'.
    Then, whenever I need to access the rows, I have to typecast the
    pointer using: ((struct Address *)conn->db->rows).  That expression
    can be indexed to get a specific address, and can be prefixed with
    '&' if I really need a pointer to a specific address.  Once I figured
    this out everything else pretty much fell into place.
3 - Implemented 'r' option to resize the database.  Data is preserved
    but can be truncated if either max_data or max_rows is set too
    small for existing entries.
4 - Implemented basic database search with 'f' option.  This will match
    the first 3 characters (or all of them if only 1-2 characters are given)
    against either name or email strings of set records.  It can only
    match from the beginning of the strings, and returns a match if
    the first 3 characters match.

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
    void *rows; // Database_open will dynamically malloc the appropriate memory size.
                // This lets us avoid any array size specifications here.
                // We then use a typecast pointer to access rows as Addresses.
};

struct Connection {
    FILE *file;
    struct Database *db;
};

void Database_close(struct Connection *conn)
{
    int i = 0;

    if(conn) {
        if(conn->file){
            fclose(conn->file);
        }
        if(conn->db){
            for(i = 0; i < conn->db->max_rows; i++){
                struct Address *addr = &((struct Address *)conn->db->rows)[i];
                if(addr->name){
                    free(addr->name);
                }
                if(addr->email){
                    free(addr->email);
                }
            }
            free(conn->db->rows);
            free(conn->db);
        }
        free(conn);
    }
}

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

void Database_read_int(struct Connection *conn, void *dest)
{
    // read 1 int object from conn->file into dest
    // dest is typically defined with conn-> notation
    // e.g. conn->db->max_data or conn->db->rows[i]->id
    int rc = fread(dest, sizeof(int), 1, conn->file);
    if(rc == 0){
        die("No ints read from file", conn);
    } else if(rc > 1) {
        die("Too many ints read from file", conn);
    }
}

void Database_read_char(struct Connection *conn, void *dest)
{
    char fields[conn->db->max_data];
    int rc = fread(dest, sizeof(fields), 1, conn->file);
    if(rc == 0){
        die("No fields read from file", conn);
    } else if(rc > 1) {
        die("Too many fields read from file", conn);
    }
}

void Database_load(struct Connection *conn)
{
    int i = 0;

    for(i = 0; i < conn->db->max_rows; i++){
        struct Address *addr = &((struct Address *)conn->db->rows)[i];
        Database_read_int(conn, &addr->id);
        Database_read_int(conn, &addr->set);
        if(addr->set){
            addr->name = malloc(conn->db->max_data);
            addr->email = malloc(conn->db->max_data);
            Database_read_char(conn, addr->name);
            Database_read_char(conn, addr->email);
        } else {
            addr->name = NULL;
            addr->email = NULL;
        }
    }

}

struct Connection *Database_open(const char *filename, char mode, int max_data, int max_rows)
{
    struct Connection *conn = malloc(sizeof(struct Connection));
    if(!conn){
        die("Memory error", conn);
    }

    // set the conn->file pointer
    // set the conn->db pointer
    // set the conn->db->max_data and conn->db->max_rows values
    // conn->db->rows is a void pointer to a block of memory that will hold max_rows Addresses
    if(mode == 'c'){
        conn->file = fopen(filename, "w");
        conn->db = malloc(sizeof(struct Database));
        conn->db->max_data = max_data;
        conn->db->max_rows = max_rows;
        conn->db->rows = malloc(conn->db->max_rows * sizeof(struct Address));
    } else {
        conn->file = fopen(filename, "r+");
        conn->db = malloc(sizeof(struct Database));
        if(conn->file){
            Database_read_int(conn, &conn->db->max_data);
            Database_read_int(conn, &conn->db->max_rows);
            conn->db->rows = malloc(conn->db->max_rows * sizeof(struct Address));
        }
    }

    if(!conn->file){
        die("Failed to open the file", conn);
    }
    if(!conn->db){
        die("Failed to allocate database memory", conn);
    }

    return conn;
}

void Database_write_int(struct Connection *conn, void *src)
{
    // read 1 int object into conn->file from src
    // src is typically defined with conn-> notation
    // e.g. conn->db->max_data or conn->db->rows[i]->id
    int rc = fwrite(src, sizeof(int), 1, conn->file);
    if(rc == 0){
        die("No ints written to file", conn);
    } else if(rc > 1) {
        die("Too many ints written to file", conn);
    }
}

void Database_write_char(struct Connection *conn, void *src)
{
    char fields[conn->db->max_data];
    int rc = fwrite(src, sizeof(fields), 1, conn->file);
    if(rc == 0){
        die("No fields written to file", conn);
    } else if(rc > 1) {
        die("Too many fields written to file", conn);
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

    Database_write_int(conn, &conn->db->max_data);
    Database_write_int(conn, &conn->db->max_rows);

    int i = 0;

    for(i = 0; i < conn->db->max_rows; i++){
        struct Address *addr = &((struct Address *)conn->db->rows)[i];
        Database_write_int(conn, &addr->id);
        Database_write_int(conn, &addr->set);
        if(addr->set){
            Database_write_char(conn, addr->name);
            Database_write_char(conn, addr->email);
        }
    }

    int rc = fflush(conn->file);
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

    for(i = 0; i < conn->db->max_rows; i++) {
        struct Address *addr = &((struct Address *)conn->db->rows)[i];
        addr->id = i;
        addr->set = 0;
        addr->name = NULL;
        addr->email = NULL;
    }
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
    struct Address *addr = &((struct Address *)conn->db->rows)[id];
    if(addr->set){
        die("Already set, delete it first", conn);
    }

    addr->set = 1;
    addr->name = malloc(conn->db->max_data);
    addr->email = malloc(conn->db->max_data);

    char *res = strncpy(addr->name, name, conn->db->max_data - 1);
    addr->name[conn->db->max_data - 1] = '\0';
    if(!res){
        die("Name copy failed", conn);
    }

    res = strncpy(addr->email, email, conn->db->max_data - 1);
    addr->email[conn->db->max_data - 1] = '\0';
    if(!res){
        die("Email copy failed", conn);
    }
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = &((struct Address *)conn->db->rows)[id];

    if(addr->set){
        Address_print(addr);
    } else {
        die("ID is not set", conn);
    }
}

int Compare_terms(struct Connection *conn, char *search_term, char *record)
{
    // will return a match if the first 3 characters match
    int i = 0;
    int match = 1;
    for(i = 0; i < 3; i++){
        if(search_term[i] != record[i] && search_term[i] != '\0' && record[i] != '\0'){
            match = 0;
            break;
        }
    }

    return match;
}

void Database_find(struct Connection *conn, char *term)
{
    int i = 0;
    int found = 0;

    for(i = 0; i < conn->db->max_rows; i++){
        struct Address *addr = &((struct Address *)conn->db->rows)[i];
        if(addr->set){
            if(Compare_terms(conn, term, addr->name) || Compare_terms(conn, term, addr->email)){
                Address_print(addr);
                found = 1;
            }
        }
    }

    if(!found){
        printf("Search term '%s' was not found\n", term);
    }
}

void Database_delete(struct Connection *conn, int id)
{
    struct Address addr = {.id = id, .set = 0};
    ((struct Address *)conn->db->rows)[id] = addr;
}

void Database_list(struct Connection *conn)
{
    int i = 0;
    struct Database *db = conn->db;

    for(i = 0; i < conn->db->max_rows; i++){
        struct Address *cur = &((struct Address *)db->rows)[i];

        if(cur->set) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[])
{
    struct Connection *conn = NULL;

    if(argc < 3){
        die("USAGE: ex17 <dbfile> <action> [action params]", conn);
    }

    char *filename = argv[1];
    char action = argv[2][0];
    int id = 0;
    char *term;
    int max_data = 0;
    int max_rows = 0;

    if(action != 'c'){
        conn = Database_open(filename, action, 0, 0);
        Database_load(conn);
        if(argc > 3 && action != 'r' && action != 'f'){
            id = atoi(argv[3]);
        }
        if(id >= conn->db->max_rows && action != 'r' && action != 'f'){
            die("There aren't that many records", conn);
        }
    }

    switch(action) {
        case 'c':
            if(argc == 5){
                max_data = atoi(argv[3]);
                max_rows = atoi(argv[4]);
            } else {
                die("c (create) usage: ex17 <dbfile> c <max_data> <max_rows>", conn);
            }
            conn = Database_open(filename, action, max_data, max_rows);
            Database_create(conn);
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
        case 'r':
            // database is already loaded using old size parameters
            // just change them in RAM then write back to the file
            if(argc == 5){
                conn->db->max_data = atoi(argv[3]);
                conn->db->max_rows = atoi(argv[4]);
            } else {
                printf("Current size:\n\tmax_data: %d\n\tmax_rows: %d\n", conn->db->max_data, conn->db->max_rows);
                die("r (resize) usage: ex17 <dbfile> r <max_data> <max_rows>", conn);
            }
            Database_write(conn);
            break;
        case 'f':
            if(argc != 4){
                die("Need a search term to look up", conn);
            }
            term = argv[3];
            Database_find(conn, term);
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=del, l=list", conn);
    }

    Database_close(conn);

    return 0;
}


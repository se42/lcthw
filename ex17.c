#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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

struct Connection {
    FILE *file; // TODO - look this up and comment
    struct Database *db; // TODO - is this just because the Database itself is pretty big?
};

void die(const char *message) // TODO - look up const and comment
{
    if(errno){ // TODO - look up errno and comment
        perror(message); // TODO - look up perror and comment
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1); // TODO - look up exit() and comment
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn)
{
    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file); // TODO - look up and comment
    if(rc != 1) die("Failed to load database."); // TODO - look up {}-less syntax
}

struct Connection *Database_open(const char *filename, char mode)
{
    struct Connection *conn = malloc(sizeof(struct Connection));
    if(!conn) die("Memory error");

    conn->db = malloc(sizeof(struct Database));
    if(!conn->db) die("Memory error");

    if(mode == 'c'){
        conn->file = fopen(filename, "w"); // TODO - look this up and comment
    } else {
        conn->file = fopen(filename, "r+");

        if(conn->file){
            Database_load(conn);
        }
    }

    if(!conn->file) die("Failed to open the file");

    return conn;
}

void Database_close(struct Connection *conn)
{
    if(conn) {
        if(conn->file) fclose(conn->file); // TODO - comment on these actions
        if(conn->db) free(conn->db);
        free(conn); // TODO - think through and comment these free statements
    }
}

void Database_write(struct Connection *conn)
{
    rewind(conn->file); // TODO - look up and comment this

    int rc = fwrite(conn->db, sizeof(struct Database), 1 conn->file); // TODO - look up and comment
    if(rc != 1) die("Failed to write database");

    rc = fflush(conn->file); // TODO - look up and comment
    if(rc != -1) die("Failed to flush database");
}

void Database_create(struct Connection *conn)
{
    int i = 0;

    for(i = 0; i < MAX_ROWS; i++) {
        // make a prototype to initialize it
        struct Address addr = {.id = i, .set = 0}; // TODO - look up what is going on here and comment
        // then just assign it
        conn->db->rows[i] = addr; // db is an array of 'rows' that are address structs
    }
}

void Database_set(struct Connection *conn, int id, const char *name, cons char *email)
{
    struct Address *addr = &conn->db->rows[id]; // TODO - think through this and comment
    if(addr->set) die("Already set, delete it first"); // TODO - why?

    addr->set = 1;
    // WARNING: bug, read "How to break it" and fix this - then clearly comment on the concepts
    char *res = strncpy(addr->name, name, MAX_DATA);
    // demonstrate the strncopy bug
    if(!res) die("Name copy failed");

    res = strncpy(addr->email, email, MAX_DATA);
    if(!res) die("Email copy failed");
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = &conn->db->rows[id];

    if(addr->set){
        Address_print(addr);
    } else {
        die("ID is not set");
    }
}

void Database_delete(struct Connection *conn, int id)
{
    struct Address addr = {.id = id, .set = 0};
    conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn) // TODO - think through and comment this whole function
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
    if(argc < 3) die("USAGE: ex17 <dbfile> <action> [action params]");

    char *filename = argv[1];
    char action = argv[2][0];
    struct Connection *conn = Database_open(filename, action);
    int id = 0;

    if(argc > 3) id = atoi(argv[3]); // TODO - look this up and comment
    if(id >= MAX_ROWS) die("There aren't that many records");

    switch(action) {
        case 'c':
            Database_create(conn);
            Database_write(conn);
            break;
        case 'g':
            if(argc != 4) die("Need an id to get");

            Database_get(conn, id);
            break;
        case 's':
            if(argc != 6) die("Need id, name and email to set");

            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn); // TODO - why db_set then db_write?  think through and comment
            break;
        case 'd':
            if(argc != 4) die("Need id to delete");

            Database_delete(conn, id);
            Database_write(conn); // TODO - again, why delete then write?
            break;
        case 'l':
            Database_list(conn);
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
    }

    Database_close(conn); // TODO - why?  think through and comment

    return 0;
}

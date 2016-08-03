/*
Basic database application demonstrating stack and heap memory.
Also proved to be a challenging exercise in pointers and slightly
more advanced structs, as well as an introduction to FILE objects.
Cool to see how data is read from permanent storage and temporarily
held in RAM when you run a program.

Code is more or less straight from the exercise.  Comments are my
study notes as I try to understand the concepts at work here, and
are a mix of my thoughts and in some places paraphrasing 'man'
documentation.
*/

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

    /*
    This is still a fixed-size struct, but this one has a compound structure.
    I figure we don't use a pointer here because we're still building up this
    compound data struct.  I guess you could have an array of pointers to the
    various Address structs, but those pointers would use more memory than just
    putting all of the Address structs together as is done here.

    Basically how I imagine this is [[id|set|name|email]|[id|set|name|email]|...]
    where the internal [id|set|name|email] are the Address sub-structs and
    the whole block of memory is the Database super-struct.
    The nested structs just let you build this block by block.
    */
};

struct Connection {
    FILE *file;
    struct Database *db;

    /*
    Why pointer to db?
    I figure this is because the Database struct isn't "part of" the Connection
    struct the way Address structs are "part of" Database structs.  The
    Database struct already exists in heap memory, and the Connection just
    needs to know where it is.  Hence the db pointer.

    FILE defined in <stdio.h>
    www.gnu.org on FILE:  This is the data type used to represent stream objects.
    A FILE object holds all of the internal state information about the connection
    to the associated file, including such things as the file position indicator
    and buffering information. Each stream also has error and end-of-file status
    indicators that can be tested with the ferror and feof functions
    */
};

void die(const char *message)
{
    if(errno){
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1); // from <stdlib.h>: perform normal program termination

    /*
    errno is a global variable that is often set by functions when they fail.
    They also typically return an integer value of -1 to indicate this failure.
    perror(message) prints 'message: <error message associated with current value of errno>'
    */
}

void Address_print(struct Address *addr)
{
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn)
{
    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
    if(rc != 1){
        die("Failed to load database.");
    }

    /*
    from man 3 fread:  The function fread(ptr, size, nitems, stream) reads
    'nitems' objects, each 'size' bytes long, from the stream pointed to by
    'stream', storing them at the location given by 'ptr'

    So this fread() is reading 1 object that is the size of a database struct
    from the stream pointed to by conn->file and storing that information at
    the location pointed to by conn->db.  AKA it's pulling data from the
    file into RAM and storing it at the location pointed to by conn->db.

    fread() returns the number of objects that were read, hence the rc != 1 logic.
    If an error occurs, the rc is a short object count or zero.

    Zed wrote a bunch of {}-less if statements throughout this example, but he
    said this is "very" bad form in ex12.  I've changed them because I agree
    with Zed-from-ex12...

    if(rc != 1) die("Failed to load database."); <-- sample code from lazy-ex17-Zed
    */
}

struct Connection *Database_open(const char *filename, char mode)
{
    struct Connection *conn = malloc(sizeof(struct Connection));
    if(!conn){
        die("Memory error");
    }

    conn->db = malloc(sizeof(struct Database));
    if(!conn->db){
        die("Memory error");
    }

    if(mode == 'c'){
        conn->file = fopen(filename, "w");
    } else {
        conn->file = fopen(filename, "r+");

        if(conn->file){
            Database_load(conn);
        }
    }

    if(!conn->file){
        die("Failed to open the file");
    }

    return conn;

    /*
    This function returns a pointer to a Connection struct.

    It begins by malloc'ing a block of heap memory that is the size of a Connection
    struct and returning a pointer to this memory block, stored in the variable conn,
    and then confirms that the pointer returned by malloc() is not a NULL pointer.

    With Connection struct created in memory and the conn variable holding a pointer
    to its location, the next step malloc's a block of heap memory to hold the
    Database struct that will eventually be read in from a file.  The function
    checks that the pointer returned by malloc() is not NULL.

    If the program has been run in create mode ('c'), then fopen() will open the file
    in 'w' mode.  Note that if bin/ex17 <filename> c is run on an existing file, that
    file will be overwritten per the 'w' specification.  For any other option, the
    file will be opened in 'r+' mode, which will allow both reading and writing but
    will not truncate the file to zero length as with 'w'.

    In either case, fopen() will return a FILE pointer if successful, or NULL if not.

    In any mode but 'c/create', the function will also attempt to use Database_load
    to load data from the database file into the heap memory location pointed to
    by conn->db (malloc'd a few lines up).

    In all modes, if the conn->file pointer has not been successfully set by fopen(),
    the function will cause the program to die.

    The function returns a pointer to the newly populated Connection struct.
    */
}

void Database_close(struct Connection *conn)
{
    if(conn) {
        if(conn->file){
            fclose(conn->file);
        }
        if(conn->db){
            free(conn->db);
        }
        free(conn);
    }

    /*
    After ensuring that the conn pointer is not NULL, and that the associated
    pointers to the FILE object and to the db heap memory are not NULL, this
    function 'closes' the file, frees the heap memory that was set aside for
    db, and finally frees the heap memory that was set aside for the conn.

    fclose() dissociates the named stream (conn->file) from its underlying file
    or set of functions after first writing any buffered data using fflush().
    It's not used here, but fclose() returns a 0 if successful.  If unsuccessful,
    it (presumably) returns -1 and sets the global errno variable.
    */
}

void Database_write(struct Connection *conn)
{
    rewind(conn->file);

    int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
    if(rc != 1){
        die("Failed to write database");
    }

    rc = fflush(conn->file); // TODO - look up and comment
    if(rc == -1){
        die("Cannot flush database");
    }

    /*
    rewind() sets the file position indicator for the stream pointed to by conn->file
    to the beginning of the file.  I'm guessing this is necessary because some file
    positioning attribute within the FILE object has to be reset as this file was
    potentially read in a Database_load operation.

    fwrite(conn->db, db_size, 1, conn->file) writes 1 object of size db_size to the
    stream pointed to by conn->file, obtaining the data from the location given by conn->db.

    fwrite() returns the number of objects written, so in this case an rc == 1 indicates success.
    */
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

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
    struct Address *addr = &conn->db->rows[id]; // setting the value of the pointer (an address)
    if(addr->set){
        die("Already set, delete it first"); // TODO - why?
    }

    addr->set = 1; // you know the addr is an Address struct, so ->set references a distance down the memory line for this value
    // WARNING: bug, read "How to break it" and fix this - then clearly comment on the concepts
    char *res = strncpy(addr->name, name, MAX_DATA);
    // demonstrate the strncopy bug
    if(!res){
        die("Name copy failed");
    }

    res = strncpy(addr->email, email, MAX_DATA);
    if(!res){
        die("Email copy failed");
    }
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
    struct Address addr = {.id = id, .set = 0}; // TODO - study this 'local prototype' concept
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
    if(argc < 3){
        die("USAGE: ex17 <dbfile> <action> [action params]");
    }

    char *filename = argv[1];
    char action = argv[2][0];
    struct Connection *conn = Database_open(filename, action);
    int id = 0;

    if(argc > 3){
        id = atoi(argv[3]); // TODO - look this up and comment
    }
    if(id >= MAX_ROWS){
        die("There aren't that many records");
    }

    // note this isn't the best way to process complex arguments
    switch(action) {
        case 'c':
            Database_create(conn);
            Database_write(conn);
            break;
        case 'g':
            if(argc != 4){
                die("Need an id to get");
            }

            Database_get(conn, id);
            break;
        case 's':
            if(argc != 6){
                die("Need id, name and email to set");
            }

            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn); // TODO - why db_set then db_write?  think through and comment
            // Database_set updates the corresponding RAM location with the new data
            // Database_write is actually interacting with the permanent file itself
            break;
        case 'd':
            if(argc != 4){
                die("Need id to delete");
            }

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


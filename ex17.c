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

    rc = fflush(conn->file);
    if(rc == -1){
        die("Cannot flush database");
    }

    /*
    rewind() sets the file position indicator for the stream pointed to by
    conn->file to the beginning of the file.  I'm guessing this is necessary
    because some file positioning attribute within the FILE object has to be
    reset as this file was potentially read in a Database_load operation.

    fwrite(conn->db, db_size, 1, conn->file) writes 1 object of size db_size
    to the stream pointed to by conn->file, obtaining the data from the
    location given by conn->db.

    fwrite() returns the number of objects written, so in this case an
    rc == 1 indicates success.

    From what I can tell, it seems like fwrite() is only sending data to the
    stream, so it will be buffered/actually written per the FILE object.
    So to be sure all of the data went all the way through to the file
    itself...

    fflush(ptr_to_stream) forces a write of all buffered data for the given
    output or update stream via the stream's underlying write function.
    rc == 0 if successful, otherwise rc == -1 and errno is set.
    */
}

void Database_create(struct Connection *conn)
{
    int i = 0;

    for(i = 0; i < MAX_ROWS; i++) {
        // make a prototype to initialize it and then just assign it
        struct Address addr = {.id = i, .set = 0};
        conn->db->rows[i] = addr;
    }

    /*
    This function is called when the program is run with the 'c/create'
    action.  For each row in the database up to the MAX_ROWS limit,
    an Address struct is created with id=i and ensuring set=0, and
    is then assigned to conn->db->rows[i].

    In this case we have an empty block of memory that was malloc'd
    in Database_open, which assigned a pointer to the db variable that
    is typed as a Database struct.  That variable typing is why we can
    do the db->rows[i] syntax here which will put these addr prototypes
    in the right place.

    Since the Database struct is just an array of Address "sub"-structs,
    we can iterate over this array and set each addr with the initial
    values that we want to use to represent an empty database.

    The 'struct Address addr = {}' syntax initializes an Address struct
    in place with the given values, which can be given either in positional
    order or by using .attr=value syntax.  We then use conn->db->rows[i]
    pointer syntax to assign the values of this prototype addr to the
    memory at the pointed-to location.
    */
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
    struct Address *addr = &conn->db->rows[id];
    if(addr->set){
        die("Already set, delete it first");
    }

    addr->set = 1;

    char *res = strncpy(addr->name, name, MAX_DATA - 1);
    addr->name[MAX_DATA - 1] = '\0';
    if(!res){
        die("Name copy failed");
    }

    res = strncpy(addr->email, email, MAX_DATA - 1);
    addr->email[MAX_DATA - 1] = '\0';
    if(!res){
        die("Email copy failed");
    }

    /*
    This function attempts to update the RAM version of the database when
    the program is called with the s action, but implements a warning if
    the file already contains a set record at the given id.

    The function first creates a struct Address variable called addr and
    assigns to it the address (via &) of the memory block that represents
    the Address record of the given id.  We use the & operator here because
    conn->db->rows[id] would return the actual data from this location.
    At this point we just want to know where the data is so we can work
    with it later, so getting a pointer makes sense.

    The first operation is to check the addr->set value.  If the value
    is anything but 0, the program raises a warning to first delete the entry.
    This is just a mock warning that Zed built into the program...this is
    not somethineg that necessarily has to be done.

    If the value of addr->set is currently 0, then we pass the 'already set'
    die test and then set the value of addr->set to 1 (remember this is
    the RAM representation of the database that we're working with here).
    The next step is to populate the name and email fields of the Address.

    strncpy() blindly reads characters up to the size given, so if there
    are more characters in the given string than the size given to strncpy(),
    the resulting copied string will not be terminated.  To fix this, we use
    'MAX_DATA - 1' as the size and force the last character of addr->name
    and addr->email to be set to the null terminator.  Without this, if
    the name is too long the program will continue reading characters into
    the memory representing email anytime name is requested.  Email gets off
    easy in this case as the next block of memory is the int representing id,
    and in our case the first byte of this int block will always be 0.
    However, if this bug made it into a large database, we would start to see
    some odd behavior vis-a-vis the email string at id ~16777216 (2^24).
    */
}

void Database_get(struct Connection *conn, int id)
{
    struct Address *addr = &conn->db->rows[id];

    if(addr->set){
        Address_print(addr);
    } else {
        die("ID is not set");
    }

    /*
    As in Database_set, we first look up the address of the relevant id
    and save this address as opposed to the actual data, in this case
    because the Address_print function we plan to use takes a pointer
    to an Address struct as its only parameter.

    The addr->set check prevents the program from returning empty records,
    which may or may not be desired behavior.
    */
}

void Database_delete(struct Connection *conn, int id)
{
    struct Address addr = {.id = id, .set = 0};
    conn->db->rows[id] = addr;

    /*
    This function is the first step in a 1-2 move to delete a row from
    the database when the program is run with the 'd' action.  This step
    updates the corresponding 'row' entry in the RAM db by assigning an
    empty prototype to the location corresponding to the given id.  The
    next step, handled by the Database_write function, is to push these
    RAM changes to the db file.
    */
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

    /*
    This function lists all populated/'set' rows in the RAM db.  Following,
    a similar pattern, we first create a variable for a pointer to the
    RAM object we want to work with.  We then loop over this object (which,
    because of its struct Database type, has indices correponding to
    'rows', which are of the struct Address type.)  For each row in the
    object pointed to by db, up to the MAX_ROWS, we create a pointer
    to this location (as opposed to creating a variable for the actual
    data).  Then, because we've typed this pointer as struct Address, we
    can test the value of cur->set.  If it's anything but 0 (in our case 1),
    then we execute the Address_print function, which takes a pointer of
    the struct Address type as its only parameter.

    Similar to the Database_get function, the if(cur->set) condition may
    or may not be the desired behavior.
    */
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
        id = atoi(argv[3]);
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
            Database_write(conn);
            break;
        case 'd':
            if(argc != 4){
                die("Need id to delete");
            }

            Database_delete(conn, id);
            Database_write(conn);
            break;
        case 'l':
            Database_list(conn);
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
    }

    Database_close(conn);

    return 0;

    /*
    main() function defines the overall flow of the database program.  The general
    idea is that the program will take a file and action paramater that will
    determine what it does to which file.  Depending on the action, a row id
    and data to enter may be required.

    The function first ensures that the minimum number of arguments have been
    given.  If not, the program is killed.

    With the minimum number of arguments present, the function first attempts to
    open a database connection by running the Database_open function, which will
    return a pointer to a struct Connection stored in the variable conn.  Note
    that at this point the action could be invalid, but the connection will
    still be opened in 'r+' mode.  Database_open is tasked with establishing
    a stream to the designated file (conn->file) and malloc'ing appropriate
    heap memory blocks for the conn object and the RAM representation of the
    db file.

    It is assumed that argv[3], if present, will always represent the id of a
    row to be operated on.  If there are more than 3 args, then argv[3] is
    converted from its ASCII string representation to an int and stored in the
    id variable.  If the id is >= the number of rows in the database (indexing
    starts at 0), the the program is killed.

    Zed notes that a switch-case is not the best way to handle arguments.  Not
    sure why he did it this way, but it's what he did.

    For every case, the general pattern here is to first interact with the RAM
    representation of the database, and then push the changes (if any) to the
    database file using Database_write.  Anything representing a get-type
    operation (g/get or l/list) will only interact with the RAM database.  Any
    option requiring write operations will first update the RAM representation
    and then use a common Database_write function to push those changes to the file.

    If a bad action is given, the program is killed via die() as the default case.

    Finally, Database_close executes cleanup operations by ensuring the file
    stream is properly closed, then freeing the heap memory malloc'd for the
    RAM db, then freeing the heap memory malloc'd for the conn struct.

    If all of this executes without triggering a die() call, then the program
    exits with status code 0 indicating success.

    Also note that Database_open is designed to do all of the requisite malloc'ing
    for the program, and that Database_close is designed to safely clean up
    all of the malloc'd memory and the open files from Database_open.
    */
}


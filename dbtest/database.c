
#include <stdio.h>

#include <string.h> // strerror()
#include <errno.h>
#include <stdarg.h> // va_ args
#include <stdlib.h> // exit()

#include "sqlite3.h"

// Function to test a condition
static void check (int test, const char * message, ...)
{
    if (test != 0) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", strerror(errno));
        
        // Failure
        exit (1);
    }
}

// Function to test a condition
static void checksql (int test, sqlite3* db, const char * message, ...)
{
    if (test != SQLITE_OK) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", sqlite3_errmsg(db));
        
        // Failure
        exit (1);
    }
}

int main(int argc, const char * argv[]) {
    
    // Variables
    int status, flags;
    sqlite3 *db;
    
    printf("DBtest\n");
    
    // Open / create database
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
    status = sqlite3_open_v2("file:test.db", &db, flags, NULL);
    checksql(status, db, "sqlite3_open_v2 error");
    
    // Close the database
    status = sqlite3_close_v2(db);
    checksql(status, db, "sqlite3_close_v2 error");
    
    return 0;
}

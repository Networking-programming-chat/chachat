
#include <stdio.h>

#include <stdarg.h> // va_ args
#include <stdlib.h> // exit()

#include "sqlite3.h"

// Function to test a condition
static void checksql (int test, sqlite3 * db, const char * message, ...)
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

// Function to test a condition
static int printsql (int test, char *errmsg, const char * message, ...)
{
    if (test != SQLITE_OK) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", errmsg);
    }
    return test;
}


// Function to create dummy data
void create_data(sqlite3 * db)
{
    int status, i;
    char *errmsg;
    char *names[4] = {"Olli", "Teflon", "George", "Anton"};
    char *nicks[4] = {"opa", "nasser" ,"hyhma" ,"antsa"};
    
    sqlite3_stmt *stmt;
    
    // write sql statements in uppercase for easy reading
    status = sqlite3_exec(db, "DROP TABLE IF EXISTS students", NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    status = sqlite3_exec(db, "CREATE TABLE students (id INTEGER NOT NULL PRIMARY KEY, name VARCHAR(30), nick VARCHAR(20))", NULL, NULL, &errmsg);
    if (printsql(status, errmsg, "sqlite3_exec fail") != SQLITE_OK) {return;}
    
    // Insert data and test variable binding
    status = sqlite3_prepare_v2(db, "INSERT INTO students VALUES (?,?,?)", -1, &stmt, NULL);
    if (printsql(status, errmsg, "sqlite3_prepare_v2 fail") != SQLITE_OK) {return;}
    
    // Bind and insert data
    for (i = 0; i < 4; ++i) {
        
        // id
        status = sqlite3_bind_int(stmt, 1, i);
        if (printsql(status, errmsg, "sqlite3_bind_int fail") != SQLITE_OK) {return;}
        
        // name
        status = sqlite3_bind_text(stmt, 2, names[i], -1, SQLITE_STATIC);
        if (printsql(status, errmsg, "sqlite3_bind_text fail") != SQLITE_OK) {return;}
        
        // nick
        status = sqlite3_bind_text(stmt, 3, nicks[i], -1, SQLITE_STATIC);
        if (printsql(status, errmsg, "sqlite3_bind_text fail") != SQLITE_OK) {return;}
        
        status = sqlite3_step(stmt);
        status = status == SQLITE_DONE ? SQLITE_OK : SQLITE_FAIL;
        if (printsql(status, errmsg, "sqlite3_step fail") != SQLITE_OK) {return;}
        
        // Reset for next use
        sqlite3_reset(stmt);

    }
    
    // Finalize the statement
    status = sqlite3_finalize(stmt);
    printsql(status, errmsg, "sqlite3_finalize fail");
}

// Callback function for reading data
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    
    printf("%s\n", (const char*)data);
    
    for(i = 0; i < argc; ++i){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    return 0;
}

// Function to test reading the dummy data
void read_data(sqlite3 *db)
{
    int status;
    char *errmsg;
    const char* data = "Data for callback";
    
    // List all students whose name ends with an n
    status = sqlite3_exec(db, "SELECT name, nick FROM students WHERE name LIKE '%n'", callback, (void*)data, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
}

int main(int argc, const char * argv[])
{
    // Variables
    int status, flags;
    sqlite3 *db;
    
    // Open / create database
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
    status = sqlite3_open_v2("file:test.db", &db, flags, NULL);
    checksql(status, db, "sqlite3_open_v2 error");
    
    // Create dummy data
    create_data(db);
    
    // Test reading the data
    read_data(db);
    
    // Close the database
    status = sqlite3_close_v2(db);
    checksql(status, db, "sqlite3_close_v2 error");
    
    return 0;
}

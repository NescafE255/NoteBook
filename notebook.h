#ifndef NOTEBOOK
#define NOTEBOOK
// #include "database.h"
// #include "logger.h"
// #include "server.h"
#include "list.h"
// #include <time.h>


enum requestMethod{
    STORE,
    GET,
    RENAME,
    DELETE,
    UNKNOWN
};


typedef struct client_request {
    // char *date;
    s_db_entry *note;
    enum requestMethod method;
} cli_req;

extern struct tm global_time;


s_db_entry *add_note();
void show_plansCertainDay(cli_req *request);
void display();
cli_req *make_print();
void date_init();
void deleteNote(cli_req *request);


void getCurrentDateTime();

#endif
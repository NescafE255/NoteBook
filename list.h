// #ifndef LIST
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#define SIZE_FILENAME 30


// #include "database.h"

#define MAX_NUMBER_DATE 11
#define MAX_TITLE_SYMBOLS 50
#define MAX_BODY_SYMBOLS 1024

typedef struct db_entry {
    char title[MAX_TITLE_SYMBOLS];
    char body[MAX_BODY_SYMBOLS];
    struct tm due_time;
    struct db_entry *next;
} s_db_entry;




extern s_db_entry *near_notes, *buffer_notes;



void append(s_db_entry **list, s_db_entry *new_note);
void free_list(s_db_entry *head);
void display_list(s_db_entry *note);
void delete_note_by_date(s_db_entry **list, s_db_entry *delete_note);
s_db_entry *note_dup(s_db_entry *note);

// #endif
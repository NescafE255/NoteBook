#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define SIZE_FILENAME 30


// #include "database.h"

#define MAX_NUMBER_DATE 11
#define MAX_TITLE_SYMBOLS 50
#define MAX_BODY_SYMBOLS 1000

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

s_db_entry *note_dup(s_db_entry *note);
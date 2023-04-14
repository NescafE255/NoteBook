#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define SIZE_FILENAME 50


// #include "database.h"


#define MAX_TITLE_SYMBOLS 50
#define MAX_BODY_SYMBOLS 1000

typedef struct db_entry {
    char title[MAX_TITLE_SYMBOLS];
    char body[MAX_BODY_SYMBOLS];
    struct tm due_time;
    struct db_entry *next;
} s_db_entry;

extern s_db_entry *near_notes, *buffer;



void append(s_db_entry *accepts, s_db_entry *head);
void free_memory(s_db_entry *head);
void save_file(s_db_entry *note, FILE *fp);
void display_list(s_db_entry *note);
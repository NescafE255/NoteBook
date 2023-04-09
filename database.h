#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/evp.h>



#define DB_DIR "database/"


// #define MAX_NOTES 100
#define MAX_TITLE_SYMBOLS 50
#define MAX_BODY_SYMBOLS 1000

typedef struct db_entry {
    char title[MAX_TITLE_SYMBOLS];
    char body[MAX_BODY_SYMBOLS];
    struct tm due_time;
    struct db_entry *next;
} s_db_entry;

extern s_db_entry *near_notes, *buffer_notes;

void init_db(struct tm *local_time);
void store_note(s_db_entry *note);

char *hash_md5(s_db_entry *note);
s_db_entry *get_note_by_date(struct tm *time);
s_db_entry *get_note_by_title(const char *title);
s_db_entry *get_note_by_range(struct tm *time_low, struct tm *time_high);
int update_note(s_db_entry *note); //TBD, Подумати над цим, можливо має приймати щось інше


// #ifndef DATABASE
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <aio.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include "list.h"


#define DB_DIR "database/"


extern struct tm real_date;



void store_note(s_db_entry *note);


void save_file(s_db_entry *note, FILE *fp);

char *db_delete_note(int number, s_db_entry *note);

s_db_entry *get_note_by_date(s_db_entry *date);

void db_update_near_notes(s_db_entry *note);

void db_init_date();

void loading_near_notes();

int return_Size_filename(char *filename);

int db_check_hash(FILE *fp, s_db_entry *notes);

char *hash_md5(s_db_entry *note);

s_db_entry *get_note_list(FILE *fp);


// #endif
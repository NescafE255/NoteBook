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


void init_db(struct tm *local_time);
void store_note(s_db_entry *note);
void init_tm_date(struct tm *local_time);
void save_file(s_db_entry *note, FILE *fp);
void db_delete_note(struct tm *date);
void get_note_by_date(struct tm *date);
void db_update_near_notes(s_db_entry *note);
void db_init_date();


int db_check_hash(FILE *fp, s_db_entry *notes);
char *hash_md5(s_db_entry *note);
s_db_entry *get_note_list(FILE *fp);



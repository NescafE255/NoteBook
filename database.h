#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include "list.h"


#define DB_DIR "database/"

void init_db(struct tm *local_time);
void store_note(s_db_entry *note, struct tm *local_time);

char *hash_md5(s_db_entry *note);
s_db_entry *get_note_list(char *filename);
s_db_entry *get_note_by_date(char *filename);
s_db_entry *get_note_by_title(const char *title);
s_db_entry *get_note_by_range(struct tm *time_low, struct tm *time_high);
int update_note(s_db_entry *note); //TBD, Подумати над цим, можливо має приймати щось інше
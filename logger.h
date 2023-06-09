// #ifndef LOGGER
#define LOGGER
#include <stdio.h>
#include <time.h>
#include  <stdlib.h>

#define MAX_NUMBER_DATE 11
#define MAX_SIZE_TITEL 50
#define MAX_SIZE_BODE 1024


struct log_info
{
    const char *cliennt_ip;
    const char *filepath;
    const char *method;
    const char *status_code;
};

struct save_log
{
    char title[MAX_SIZE_TITEL];
    char body[MAX_SIZE_BODE];
    struct tm due_time;
};

void init_log_file();
void write_log(struct save_log *save_log);
void write_debug_log(const char *message, ...);
// void get_current_time(char *out);
// void init_debug_log(s_db_entry *entry);
// #endif
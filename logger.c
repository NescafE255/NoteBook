#include "logger.h"
// #include "list.h"
#include <sys/stat.h>
#define FIELDS "History creat note"
#define YEAR 1900
#define MONTH 1


static void get_current_time(char *output);
const char *log_file = "log";




void write_log(struct save_log *save_log)
{
    char *date_time = (char *)malloc(sizeof(char));
    get_current_time(date_time);

    FILE *file = fopen(log_file, "a");
    if(file == NULL){
        perror("open log file");
        return;
    }

    char str_date_time[MAX_NUMBER_DATE];
    sprintf(str_date_time, "%02d:%02d:%04d", save_log->due_time.tm_mday, save_log->due_time.tm_mon, save_log->due_time.tm_year);

    char *date_cre = "DATE OF CREATION";
    fprintf(file, "TITEL: %sBODY: %s%s\n%s: %s\n", save_log->title, save_log->body, str_date_time, date_cre, date_time);
    fputs("------------", file);
    fputs("\n", file);
    fclose(file);
    free(date_time);
}




static void get_current_time(char *output)
{
    time_t local_time;
    struct tm *timeinfo;

    time(&local_time);
    timeinfo = localtime(&local_time);

    sprintf(output, "%04d.%02d.%02d %02d:%02d:%02d",timeinfo->tm_year + YEAR, timeinfo->tm_mon + MONTH,
	timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
}

void init_log_file()
{
    
    struct stat st;
    stat(log_file, &st);
    if(st.st_size == 0){
        FILE *file = fopen(log_file, "w");
        fprintf(file, "Save history notes\n");
        fclose(file);
    }
}
#include "database.h"


s_db_entry *near_notes = NULL;
// s_db_entry *buffer_notes;

void init_db(struct tm *local_time)
{
    // fill near_notes; localtime - 5 days < ---- > local_time + 15 days
    DIR *dir = opendir(DB_DIR);
    if(dir == NULL){
        perror("Error opening dir");
        exit(0);
    }
    closedir(dir);


    struct tm strart_date = *local_time;
    strart_date.tm_mday -= 5;
    mktime(&strart_date);

    struct tm end_date = *local_time;
    end_date.tm_mday += 15;
    mktime(&end_date);

    char filename[50];
    for(int month = strart_date.tm_mon; month <= end_date.tm_mon; month++){
        for(int year = strart_date.tm_year; year <= end_date.tm_year; year++){
            sprintf(filename, "%s%02d %d", DB_DIR, month + 1, year + 1900);

            // printf("%s\n", filename);
            FILE *fp = fopen(filename, "r");
            if(fp == NULL){
                perror("Error opening file");
                exit(1);
            }

            fseek(fp, 0, SEEK_END);

            if(ftell(fp) == 0){
                printf("File empty");
                // break;
            } else{

                fseek(fp, 0, SEEK_SET);
                int day, mon, year;
                // char line[1050];
                while(!feof(fp)){

                    s_db_entry *note = malloc(sizeof(s_db_entry)); 
                    s_db_entry *tmp = malloc(sizeof(s_db_entry));
                    
                    fgets(note->title, MAX_TITLE_SYMBOLS, fp);
                    strtok(note->title, "\n");
                    // strcpy(note->title, line);
                    
                    fgets(note->body, MAX_BODY_SYMBOLS, fp);
                    strtok(note->body, "\n");
                    // strcpy(note->body, line);

                    char data_time_str[50];
                    fgets(data_time_str, sizeof(data_time_str), fp);
                    strtok(data_time_str, "\n");
                    sscanf(data_time_str, "%d.%d.%d", &day, &mon, &year);


                    


                    // printf("%s", data_time_str);
                    // strftime(data_time_str, sizeof(50), "%d.%m.%Y", &note->due_time);
                    note->due_time.tm_mday = day;
                    note->due_time.tm_mon = mon;
                    note->due_time.tm_year = year;
                    note->next = NULL;

                    if(near_notes == NULL){
                        near_notes = note;
                    } else {
                        tmp = near_notes;
                        while(tmp->next != NULL){
                            tmp = tmp->next;
                        }
                        tmp->next = note;

                    }
                    // near_notes = tmp;
                    // free(tmp);
                    // free(note);

                }
            }

            fclose(fp);
        }

    }
}

void store_note(s_db_entry *note)
{
    char filename[30];
    sprintf(filename, "%s%02d %d", DB_DIR, note->due_time.tm_mon + 1, note->due_time.tm_year + 1900);

    FILE *fp = fopen(filename, "a");
    if(fp == NULL){
        perror("Error opening file");
        exit(0);
    }

    fputs(note->title, fp);
    fputs(note->body, fp);

    char date_time_str[50];
    strftime(date_time_str, sizeof(date_time_str), "%d.%m.%Y", &note->due_time);
    fputs(date_time_str, fp);
    // fputs("\n", fp);
    fflush(fp);
    fclose(fp);

    //IF note->due_data is in ranage of -5 days < ---- > +15 days - add the entry to the near_notes
}

// s_db_entry *get_note_by_date(struct tm *time)
// {
//     //IF time > localtime + 15 days  and buffer_notes contains data from another day
//         //Store the day of date to buffer_notes
// }

// s_db_entry *get_note_by_title(const char *title)
// {
//     s_db_entry *curr_note = near_notes;
//     while(curr_note != NULL){
//         if(strcmp(curr_note->title, title))
//             return curr_note;
//     }

//     return NULL;
// }

// s_db_entry *get_notes_by_range(struct tm *time_low, struct tm *time_high)
// {

// }

// int update_note(s_db_entry *note)
// {
//     //REMOVE PREV REC FROM FILE
//     //ADD NEW REC TO FILE
// }
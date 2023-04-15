#include "list.h"

// #define SIZE_FILENAME 50


void append(s_db_entry **accepts, s_db_entry *head){
    if(!accepts){
        return;
    }
    if(*accepts == NULL){
        *accepts = head;
    } else{
        s_db_entry *tmp = *accepts;
        while(tmp->next){
            tmp = tmp->next;
        }
        tmp->next = head;
    }
}


void free_memory(s_db_entry *head){
    s_db_entry *tmp;
    while(head){
        tmp = head->next;
        free(head);
        head = tmp;
    }
    head = NULL;
}

void save_file(s_db_entry *note, FILE *fp){

    if(strlen(note->title) == 0 || strlen(note->body) == 0 || note->due_time.tm_year == 0){
        printf("Error: Data not written to file - empty fields.\n");
        fclose(fp);
        return;
    }
    fputs(note->title, fp);
    fputs(note->body, fp);

    char date_time_str[SIZE_FILENAME];
    sprintf(date_time_str, "%02d.%02d.%04d", note->due_time.tm_mday, note->due_time.tm_mon, note->due_time.tm_year);
    fputs(date_time_str, fp);
    fputs("\n", fp);
}

void display_list(s_db_entry *note){
    s_db_entry *tmp = note;
    while(tmp){
        printf("TITLE: %s", tmp->title);
        printf("BODY: %s", tmp->body);
        printf("DATE: %02d.%02d.%04d\n",  tmp->due_time.tm_mday,
                                    tmp->due_time.tm_mon,
                                    tmp->due_time.tm_year);
        printf("\n");
        tmp = tmp->next;
    }
}
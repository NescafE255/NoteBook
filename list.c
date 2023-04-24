#include "list.h"


void append(s_db_entry **list, s_db_entry *new_note){
    if(!list){
        return;
    }
    if(*list == NULL){
        *list = new_note;
    } else{
        s_db_entry *tmp = *list;
        while(tmp->next){
            tmp = tmp->next;
        }
        tmp->next = new_note;
    }
}

void free_list(s_db_entry *head){
    s_db_entry *tmp;
    while(head){
        tmp = head->next;
        free(head);
        head = tmp;
    }
    //XXX Redundant. head is local variable. No need to NULL it here. 
    // head = NULL;
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


s_db_entry *note_dup(s_db_entry *note){
    s_db_entry *temp = malloc(sizeof(s_db_entry));
    // tmp = note;
    strcpy(temp->title, note->title);
    strcpy(temp->body, note->body);
    temp->due_time = note->due_time;
    temp->next = NULL;
    return temp;

}
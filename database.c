#include "database.h"


// #define SIZE_FILENAME 50



s_db_entry *near_notes = NULL;
    



//XXX LIST Consider moving actions with list to separate file (list.c + list.h for example)


void init_db(struct tm *local_time)
{
    // fill near_notes; localtime - 5 days < ---- > local_time + 15 days
    DIR *dir = opendir(DB_DIR);
    if(dir == NULL){
        perror("Error opening dir");
        exit(0);
    }
    closedir(dir);

  


    if(local_time == NULL){
        printf("LOCAL TIME NULL");
        return;
    }
    struct tm start_date = *local_time;
    start_date.tm_year += 1900;
    start_date.tm_mon += 1;
    start_date.tm_mday -= 5;
    start_date.tm_hour = 0;
    start_date.tm_min = 0; 
    printf("DATE init start: %d.%d\n", start_date.tm_mon, start_date.tm_year);
    struct tm end_date = *local_time;
    end_date.tm_year += 1900;
    end_date.tm_mon += 1;
    end_date.tm_mday += 15;
    end_date.tm_hour = 0;
    end_date.tm_min = 0;
    printf("DATE init end: %d.%d\n", end_date.tm_mon, end_date.tm_year);


    char *out;
    char filename[SIZE_FILENAME];
    s_db_entry *buffer;

    for(int month = start_date.tm_mon; month <= end_date.tm_mon; month++){
        for(int year = start_date.tm_year; year <= end_date.tm_year; year++){
            sprintf(filename, "%s%02d_%d", DB_DIR, month, year);
            printf("***************\n");
            printf("%s\n", filename);
            FILE *fp;
            buffer = get_note_list(filename);
            out = hash_md5(buffer);


            //need check here?????
            fp = fopen(filename, "r");
            if(fp == NULL){
                // perror("Error opening file");
                //XXX Why do you exit here? And this perror is redundant. User may not have notes at all.
                //XXX It does not mean that we have to exit! Just continue the loop
                // exit(0);
                continue;
            }

            char info_hash[33];
            memset(info_hash, 0, sizeof(info_hash));
            fgets(info_hash, sizeof(info_hash), fp);


            // printf("HASH STORY %s\n", info_hash);
            // printf("HASH NEW %s\n", out);
            if(strcmp(info_hash, out) != 0){
                printf("The file is corrupted\n");
                free_memory(near_notes);
                OPENSSL_free(out);
                continue;
            }


            s_db_entry *tmp = buffer;
            while(tmp){
                if(tmp->due_time.tm_mday >= start_date.tm_mday && tmp->due_time.tm_mday <= end_date.tm_mday){
                    // append(near_notes, tmp);
                    if(near_notes == NULL){
                        near_notes = tmp;
                    } else {
                        s_db_entry *head = near_notes;
                        while(head->next){
                            head = head->next;
                        }
                        head = tmp;
                    }
                    display_list(near_notes);
                    printf("append");
                }

                tmp = tmp->next;
            }


            OPENSSL_free(out);
            fclose(fp);
        }
    }
}



void store_note(s_db_entry *note, struct tm *local_time)
{


    struct tm start_date = *local_time;
    start_date.tm_year += 1900;
    start_date.tm_mon += 1;
    start_date.tm_mday -= 5;
    start_date.tm_hour = 0;
    start_date.tm_min = 0; 

    struct tm end_date = *local_time;
    end_date.tm_year += 1900;
    end_date.tm_mon += 1;
    end_date.tm_mday += 15;
    end_date.tm_hour = 0;
    end_date.tm_min = 0;

    // printf("Start mon %d\n", start_date.tm_mon);
    // printf("END mon %d\n", end_date.tm_mon);
    printf("DATE NOTE %d.%d\n", note->due_time.tm_mday, note->due_time.tm_year);
    printf("START %d.%d\n", start_date.tm_mday, start_date.tm_year);
    printf("END %d.%d\n", end_date.tm_mday, end_date.tm_year);

    if(note->due_time.tm_mday >= start_date.tm_mday && note->due_time.tm_mday <= end_date.tm_mday){
        // s_db_entry *tmp = malloc(sizeof(s_db_entry));
        // tmp = note;
        // append(near_notes, tmp);
        // display_list(near_notes);
            if(near_notes == NULL){
                near_notes = note;
            } else {
                s_db_entry *head = near_notes;
                while(head->next){
                    head = head->next;
                }
                head = note;
            }
            display_list(near_notes);
            printf("append");
            
        printf("Titel APPEND near_notes: %s\n ****\n", note->title);
        // printf("LIST near_notes: %s\n ****\n", near_notes->title);
        // display_list(near_notes);
    }

    s_db_entry *buffer = NULL;
    char *out;
    struct stat st;
    FILE *fp;
    int size;
    char filename[30];
    //%s%02d_%d would be better. Spaces in file_name is evel +++
    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon, note->due_time.tm_year);
    char file_hash[33];
    

    fp = fopen(filename, "r+");
    if(fp == NULL){
        // printf("ERROR OPEN FILE");
        fp = fopen(filename, "a+");
        if(fp == NULL){
            perror("ERROR FILE OPEN");
            exit(1);
        }
    }


    stat(filename, &st);
    size = st.st_size;
    // printf("Size of FILE: %d\n", size);
    if(size == 0){
        out = hash_md5(note);
        printf("created a new hash:  %s\n", out);
        // printf("Pos end out %ld", strlen(out));
        fputs(out, fp);
        fputs("\n", fp);
        save_file(note, fp);
        OPENSSL_free(out);
        free(note);
        fclose(fp);
        return;
    }

    if(size > 0){
        fseek(fp, 0, SEEK_SET);
        file_hash[0] = '\0';
        fgets(file_hash, sizeof(file_hash), fp);
        buffer = get_note_list(filename);
        out = hash_md5(buffer);
        // printf("HASH NEW FILES:  %s\n", out);
        // printf("HASH SAVE FILES: %s\n", file_hash);

        if(strcmp(file_hash, out) == 0){
            // printf("Title append  %s", note->title);
            append(buffer, note);
        } else {
            free_memory(buffer);
            unlink(filename);
            fp = fopen(filename, "w");
            out = hash_md5(note);
            printf("HASH NEW FILES ADD2:  %s\n", out);
            // printf("Pos end out %ld", strlen(out));
            fputs(out, fp);
            fputs("\n", fp);
            save_file(note, fp);
            free(note);
            fclose(fp);
            OPENSSL_free(out);
            return;
        }

    }



    out = NULL;
    out = hash_md5(buffer);
    rewind(fp);
    fputs(out, fp);
    fputs("\n", fp);
    // printf("HASH NEW FILES ADD:  %s\n", out);
    


    s_db_entry *tmp;
    while(buffer){
        save_file(buffer, fp);
        tmp = buffer->next;
        free(buffer);
        buffer = tmp;
    }
    buffer = NULL;
    OPENSSL_free(out);
    fclose(fp);
}



char *hash_md5(s_db_entry *note){

    EVP_MD_CTX *mdctx;
    char buf[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11];
    size_t buf_size;
    s_db_entry *tmp;
    



    // MD5_Init
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

  
    char date_time_str[SIZE_FILENAME];
    if(note == NULL){
        printf("hash NOTE empty!");
        //XXX return NULL here probably! At least no need to continue here!
    }

    tmp = note;
    while(tmp){
        sprintf(date_time_str, "%02d.%02d.%02d", tmp->due_time.tm_mday, 
                                                tmp->due_time.tm_mon, 
                                                tmp->due_time.tm_year);
        // printf("DATE in hash_md5: %s\n", date_time_str);
        //XXX magic number! Use define instead of 11
        buf_size = snprintf(buf, MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11, "%ld%ld%ld", strcspn(tmp->title, "\n"), strcspn(tmp->body, "\n"), strcspn(date_time_str, "\n"));
        // strcspn(buf, "\n");
        EVP_DigestUpdate(mdctx, buf, buf_size);
        tmp = tmp->next;
    }


    unsigned char hash_sum[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11];
    unsigned int hash_str_len = EVP_MD_size(EVP_md5());

    
    char *hash_str = (char *)OPENSSL_malloc(hash_str_len * 2);

    EVP_DigestFinal_ex(mdctx, hash_sum, &hash_str_len);
    EVP_MD_CTX_free(mdctx);

    for(int i = 0; i < hash_str_len; i++){
        sprintf(&hash_str[i * 2], "%02x", hash_sum[i]);
    }

    // hash_str[hash_str_len * 2] = '\n';

    return hash_str;
}


s_db_entry *get_note_list(char *filename){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        perror("Error opening file");
        // exit(0);
        return 0;
    }


    s_db_entry *buffer_notes = NULL;

    // char *hash_str = NULL;
    fseek(fp, 33, SEEK_SET);
    // printf("POS %ld\n", ftell(fp));
    // fgets(hash_str, 34, fp);
    // printf("HASH get_note_list %s", hash_str);

    char line[MAX_BODY_SYMBOLS];
    while(fgets(line, sizeof(line), fp) != NULL){
        s_db_entry *note = malloc(sizeof(s_db_entry));
        strcpy(note->title, line);

        fgets(line, sizeof(line), fp);
        strcpy(note->body, line);

        char date_time_str[SIZE_FILENAME];
        int day, mon, year;
        fgets(date_time_str, sizeof(date_time_str), fp);
        sscanf(date_time_str, "%d.%d.%d", &day, &mon, &year);


        note->due_time.tm_mday = day;
        note->due_time.tm_mon = mon;
        note->due_time.tm_year = year;
        note->next = NULL;

        //XXX LIST Consider moving actions with list to separate file
        if(buffer_notes == NULL){
            buffer_notes = note;
        } else {
            s_db_entry *head = buffer_notes;
            while(head->next){
                head = head->next;
            }
            head->next = note;
        }
        // free(note);
    }
    
    fclose(fp);
    return buffer_notes;
}



s_db_entry *get_note_by_date(char *filename)
{
    // struct tm today = *local_time;

    // char filename[SIZE_FILENAME];
    // sprintf(filename, "%s%02d_%d", DB_DIR, today.tm_mday, today.tm_year);

    fopen(filename, "r");
    return get_note_list(filename);
    // display_list(buffer);
    // free_memory(buffer);



    //IF time > localtime + 15 days  and buffer_notes contains data from another day
        //Store the day of date to buffer_notes
}



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

#include "database.h"


// #define SIZE_FILENAME 50

// struct tm global_tm = getCurrentDateTime();
s_db_entry *near_notes = NULL;
s_db_entry *buffer_notes;
struct tm start_date;
struct tm end_date;
    


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

    init_tm_date(local_time);


    struct stat st;
    int size;
    char *out;
    char filename[SIZE_FILENAME];
    s_db_entry *buffer = NULL;




    for(int month = start_date.tm_mon; month <= end_date.tm_mon; month++){
        for(int year = start_date.tm_year; year <= end_date.tm_year; year++){
            sprintf(filename, "%s%02d_%d", DB_DIR, month, year);
            // printf("***************\n");
            // printf("%s\n", filename);
            FILE *fp;


            //need check here?????
            fp = fopen(filename, "r");
            if(fp == NULL){
                printf("FILE ERROR");
                continue;
            }
            stat(filename, &st);
            size = st.st_size;
            if(size == 0){
                continue;
            }


            char info_hash[33];
            memset(info_hash, 0, sizeof(info_hash));
            fseek(fp, 0, SEEK_SET);
            fgets(info_hash, sizeof(info_hash), fp);
            fseek(fp, 33, SEEK_SET);
            // printf("%ld", ftell(fp));
            // if(buffer == NULL){
            //     printf("BUFFER NULL\n");
            // }
            buffer = get_note_list(fp);
            fclose(fp);

            out = hash_md5(buffer);

            // display_list(buffer);
            // printf("HASH STORY %s\n", info_hash);
            // printf("HASH NEW %s\n", out);

            if(strcmp(info_hash, out) != 0){
                printf("The file is corrupted\n");
                free_list(buffer);
                OPENSSL_free(out);
                continue;
            }




            s_db_entry *current_mon_notes = NULL;
            while(buffer){
                // mktime(&buffer->due_time);
                // printf("buffer day %d\n", buffer->due_time.tm_mday);
                // printf("Strart day %d\n", start_date.tm_mday);
                // printf("END day %d\n", end_date.tm_mday);
                if(mktime(&buffer->due_time) >= mktime(&start_date) && mktime(&buffer->due_time) <= mktime(&end_date)){
		    //XXX Use append here!

                    if(current_mon_notes == NULL){
                        current_mon_notes = buffer;
                    } else {
                        s_db_entry *head = current_mon_notes;
                        while(head){
                            // display_list(head);
                            // printf("TMP: %d.%d.%d\n", head->due_time.tm_mday, head->due_time.tm_mon, head->due_time.tm_year);
                            head = head->next;
                        }
                        head = buffer;
                    }
                    // printf("NEAR: %s", buffer->title);
                    // append(&current_mon_notes, buffer);
                }
                    // printf("TMP: %d.%d.%d\n", start_date.tm_mday, start_date.tm_mon, start_date.tm_year);
                
                buffer = buffer->next;
            }

            append(&near_notes, current_mon_notes);
            free_list(buffer);
            OPENSSL_free(out);
        }
    }
}



void store_note(s_db_entry *note, struct tm *local_time)
{

    init_tm_date(local_time);

    // printf("DATE NOTE %d.%d.%d\n", note->due_time.tm_mday, start_date.tm_mon, note->due_time.tm_year);
    // printf("START %d.%d.%d\n", start_date.tm_mday, start_date.tm_mon, start_date.tm_year);
    // printf("END %d.%d.%d\n", end_date.tm_mday, end_date.tm_mon, end_date.tm_year);


    
    if(mktime(&note->due_time) >= mktime(&start_date) && mktime(&note->due_time) <= mktime(&end_date)){

        s_db_entry *tmp = note_dup(note);
        append(&near_notes, tmp);
    }

    FILE *fp;
    s_db_entry *buffer = NULL;
    char *out;
    struct stat st;
    int size;
    char filename[SIZE_FILENAME];
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

    fseek(fp, 0, SEEK_SET);
    file_hash[0] = '\0';
    fgets(file_hash, sizeof(file_hash), fp);
    fseek(fp, 33, SEEK_SET);
    buffer = get_note_list(fp);
    out = hash_md5(buffer);
    // printf("HASH NEW FILES:  %s\n", out);
    // printf("HASH SAVE FILES: %s\n", file_hash);

    if(strcmp(file_hash, out) == 0){
        // printf("Title append  %s", note->title);
        append(&buffer, note);
    } else {
        free_list(buffer);
        fclose(fp);
        unlink(filename);
        fp = fopen(filename, "w");
        out = hash_md5(note);
        // printf("HASH NEW FILES ADD2:  %s\n", out);
        // printf("Pos end out %ld", strlen(out));
        fputs(out, fp);
        fputs("\n", fp);
        save_file(note, fp);
        free(note);
        fclose(fp);
        OPENSSL_free(out);
        return;
    }

    out = NULL;
    out = hash_md5(buffer);
    rewind(fp);
    fputs(out, fp);
    fputs("\n", fp);
    // printf("HASH NEW FILES ADD:  %s\n", out);
    

    //XXX Use free_list from list.c file!
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
    char buf[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + MAX_NUMBER_DATE];
    size_t buf_size;
    s_db_entry *tmp;
    



    // MD5_Init
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

  
    char date_time_str[SIZE_FILENAME];
    if(note == NULL){
        // printf("hash NOTE empty!");
        //XXX return NULL here probably! At least no need to continue here!
        return 0;
    }

    tmp = note;
    while(tmp){
        sprintf(date_time_str, "%02d.%02d.%02d", tmp->due_time.tm_mday, 
                                                tmp->due_time.tm_mon, 
                                                tmp->due_time.tm_year);
        // printf("DATE in hash_md5: %s\n", date_time_str);
        buf_size = snprintf(buf, MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + MAX_NUMBER_DATE, "%ld%ld%ld", strcspn(tmp->title, "\n"), strcspn(tmp->body, "\n"), strcspn(date_time_str, "\n"));
        EVP_DigestUpdate(mdctx, buf, buf_size);
        tmp = tmp->next;
    }


    unsigned char hash_sum[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + MAX_NUMBER_DATE];
    unsigned int hash_str_len = EVP_MD_size(EVP_md5());

    
    char *hash_str = (char *)OPENSSL_malloc(hash_str_len * 2);

    EVP_DigestFinal_ex(mdctx, hash_sum, &hash_str_len);
    EVP_MD_CTX_free(mdctx);

    for(int i = 0; i < hash_str_len; i++){
        sprintf(&hash_str[i * 2], "%02x", hash_sum[i]);
    }


    return hash_str;
}


s_db_entry *get_note_list(FILE *fp){
    if(fp == NULL){
        return 0;
    }


    s_db_entry *buffer = NULL;


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

        append(&buffer, note);
    }
    
    return buffer;
}

void add_note_bufferOFmon(int day, int mon, int year){
        s_db_entry *tmp = buffer_notes;
        while(tmp){
            if(day == tmp->due_time.tm_mday && mon == tmp->due_time.tm_mon && year == tmp->due_time.tm_year){
                s_db_entry *temp = note_dup(tmp);
                display_list(temp);
                free(temp);
            }
            tmp = tmp->next;
        }
}


s_db_entry *get_note_by_date(char *filename)
{
    s_db_entry *buffer;
    char info_hash[33];
    char *out;
    // char filename[SIZE_FILENAME];
    // sprintf(filename, "%s%02d_%d", DB_DIR, today.tm_mday, today.tm_year);
    FILE *fp = fopen(filename, "r");
    fgets(info_hash, sizeof(info_hash), fp);



    fseek(fp, 33, SEEK_SET);
    buffer = get_note_list(fp);
    out = hash_md5(buffer);
    fclose(fp);

    if(strcmp(info_hash, out) != 0){
        printf("The file is corrupted\n");
        free_list(buffer);
        OPENSSL_free(out);
        return 0;
    }



    return buffer;
    //IF time > localtime + 15 days  and buffer_notes contains data from another day
        //Store the day of date to buffer_notes
}


void init_tm_date(struct tm *local_time){

    start_date = *local_time;
    start_date.tm_year += 1900;
    start_date.tm_mon += 1;
    start_date.tm_mday -= 5;
    mktime(&start_date);

    // char start_data_str[SIZE_FILENAME];
    // strftime(start_data_str, SIZE_FILENAME, "%d-%m-%Y", &start_date);
    // printf("START DATE:%s\n", start_data_str);
    // printf("DATE init start: %d.%d.%d\n",start_date.tm_mday, start_date.tm_mon, start_date.tm_year);
  



    end_date = *local_time;
    end_date.tm_year += 1900;
    end_date.tm_mon += 1;
    end_date.tm_mday += 15;
    mktime(&end_date);

    // char end_date_str[SIZE_FILENAME];
    // strftime(end_date_str, SIZE_FILENAME, "%d-%m-%Y", &end_date);
    // printf("END DATE:%s\n", end_date_str);
    // printf("DATE init end: %d.%d.%d\n",end_date.tm_mday, end_date.tm_mon, end_date.tm_year);

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

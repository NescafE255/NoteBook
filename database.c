#include "database.h"


#define SIZE_FILENAME 50



s_db_entry *near_notes = NULL;
    



//XXX LIST Consider moving actions with list to separate file (list.c + list.h for example)
void dispalay_list(s_db_entry *note){
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

void init_db(struct tm *local_time)
{
    // fill near_notes; localtime - 5 days < ---- > local_time + 15 days
    DIR *dir = opendir(DB_DIR);
    if(dir == NULL){
        perror("Error opening dir");
        exit(0);
    }
    closedir(dir);



    //XXX Potential crash here! Check if local_time is not NULL -+-+
    if(local_time == NULL){
        printf("LOCAL TIME NULL");
        //XXX return
    }
    struct tm strart_date = *local_time;
    strart_date.tm_mday -= 5;
    strart_date.tm_hour = 0;
    strart_date.tm_min = 0;
    //XXX Do we really need this mktime? What it does?
    mktime(&strart_date);

    //XXX Potential crash here! Check if local_time is not NULL -+--+
    struct tm end_date = *local_time;
    end_date.tm_mday += 15;
    end_date.tm_hour = 0;
    end_date.tm_min = 0;
    //XXX Do we really need this mktime? What it does?
    mktime(&end_date);

    s_db_entry *buffer = NULL;
    char *out;
    char filename[SIZE_FILENAME];

    for(int month = strart_date.tm_mon; month <= end_date.tm_mon; month++){
        for(int year = strart_date.tm_year; year <= end_date.tm_year; year++){
            sprintf(filename, "%s%02d_%d", DB_DIR, month, year);
            printf("***************\n");
            printf("%s\n", filename);
            FILE *fp;
            buffer = get_note_list(filename);
            out = hash_md5(buffer);

            fp = fopen(filename, "r");
            if(fp == NULL){
                perror("Error opening file");
                //XXX Why do you exit here? And this perror is redundant. User may not have notes at all.
                //XXX It does not mean that we have to exit! Just continue the loop
                exit(0);
            }  

            char info_hash[33];
            memset(info_hash, 0, sizeof(info_hash));
            fgets(info_hash, sizeof(info_hash), fp);


            printf("HASH STORY %s\n", info_hash);
            printf("HASH NEW %s\n", out);
            if(strcmp(info_hash, out) != 0){
                printf("The file is corrupted\n");
                //XXX Memory leak here! You freed only the first item of the list. The rest are still not freed.
                free(buffer);
                OPENSSL_free(out);
                continue;
            }
            // else {
            //     //XXX You already have a list here. No need to get it another time. 
            //     near_notes = get_note_list(filename);
            // }

            //XXX Why do you need this malloc? The pointer to this memory is lost after next line
            near_notes = malloc(sizeof(s_db_entry));
            //XXX HERE! YOU JUST LOST POINTER TO ALLOCATED MEMORY!
            near_notes = buffer;
            //XXX Display, not dispalay
            dispalay_list(near_notes);

            //XXX You don't need to remove the list here. near_notes have to be storred forever here.
            //XXX If user will request an entry from this data range - return a value from this list without opening file

            //XXX LIST consider moving this free to separate list file
            s_db_entry *tmp, *tmp_next;
            tmp = near_notes;
            while (tmp) {
                tmp_next = tmp->next;
                free(tmp);
                tmp = tmp_next;
            }
            
            // free(near_notes);
            // free(buffer);
            OPENSSL_free(out);
            fclose(fp);
        }
    }
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


void store_note(s_db_entry *note)
{
    s_db_entry *buffer = NULL;
    char *out;
    struct stat st;
    FILE *fp;
    int size;
    char filename[30];
    //%s%02d_%d would be better. Spaces in file_name is evel +++
    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon, note->due_time.tm_year);
    char file_hash[33];
    

    fp = fopen(filename, "r");
    stat(filename, &st);
    size = st.st_size;
    printf("Size of FILE: %d\n", size);
    if(fp == NULL){
        fp = fopen(filename, "w");
        if(fp == NULL){
            perror("Error opening file");
            exit(0);
        }
        out = hash_md5(note);
        printf("created a new hash:  %s\n", out);
        // printf("Pos end out %ld", strlen(out));
        fputs(out, fp);
        fputs("\n", fp);
        save_file(note, fp);
        fclose(fp);
        OPENSSL_free(out);
        return;
    }

    if(size > 0){
        fseek(fp, 0, SEEK_SET);
        file_hash[0] = '\0';
        fgets(file_hash, sizeof(file_hash), fp);
        buffer = get_note_list(filename);
        out = hash_md5(buffer);
        printf("HASH NEW FILES:  %s\n", out);
        printf("HASH SAVE FILES: %s\n", file_hash);

        if(strcmp(file_hash, out) == 0){
            // printf("YES\n");
            //XXX fseek to the end of file does not work instead of reopening? Or you may open it already with "a", not in with "w"?
            fclose(fp);
            fp = fopen(filename, "a");
            // printf("HASH NEW FILES ADD1:  %s\n", out);
            //XXX Consider adding the note also to the "buffer" list. You will not need to read the entire file again
            save_file(note, fp);
        } else {
            printf("NO\n");
            fclose(fp);
            //XXX unlink instead of remove
            remove(filename);

            //XXX "w" will be more convenient
            fp = fopen(filename, "a");
            //XXX unlink file, not buffer! OPENSSL_free here!
            unlink(out);
            out = hash_md5(note);
            printf("HASH NEW FILES ADD2:  %s\n", out);
            // printf("Pos end out %ld", strlen(out));
            fputs(out, fp);
            fputs("\n", fp);
            save_file(note, fp);
            fclose(fp);
            OPENSSL_free(out);
            return;
        }

    }

    fclose(fp);
    //XXX unlink file! redundant here!
    unlink(out);
    //XXX WConsider adding "note" to the "buffer" list above! You will not need to read the entire file again.
    //XXX Memory leak here! HUGE!
    buffer = get_note_list(filename);
    out = hash_md5(buffer);
    //XXX OK. You for some reason reopens file each time. Please try to find a way how to open it once and do what you want
    //XXX using fseek probably
    fp = fopen(filename, "w+");
    fputs(out, fp);
    fputs("\n", fp);
    printf("HASH NEW FILES ADD:  %s\n", out);
    //XXX Why do you need to rewrite the file? Your file already contains all the data!
    while(buffer){
        save_file(buffer, fp);
        buffer = buffer->next;
    }

    fclose(fp);
    //XXX Memory leak here! Free buffer in loop to free all items in list!
    free(buffer);
    OPENSSL_free(out);
}



char *hash_md5(s_db_entry *note){

    EVP_MD_CTX *mdctx;
    char buf[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11];
    //XXX We do not read here. Change variable name to buf_size or something
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
        exit(0);
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

//     EVP_MD_CTX *mdctx;
//     FILE *file;
//     char buf[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11];
//     size_t bytes_read;



//     // MD5_Init
//     mdctx = EVP_MD_CTX_new();
//     EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

//     file = fopen(filename, "r");
//     if(file == NULL){
//         perror("Openong error file");
//         exit(1);
//     }

//     while((bytes_read = fread(buf, 1, sizeof(buf), file)) > 0){
//         // printf("%s", buf);
//         // printf("%ld", bytes_read);
//         EVP_DigestUpdate(mdctx, buf, bytes_read);
//     }
//         // printf("%s", buf);
//         // printf("%ld", bytes_read);

//     fclose(file);

//     unsigned char md5_sum[MD5_DIGEST_LENGTH];
//     unsigned int md5_sum_len = 0;

//     EVP_DigestFinal_ex(mdctx, md5_sum, &md5_sum_len);
//     EVP_MD_CTX_free(mdctx);
//     char *hash_str = (char *)OPENSSL_malloc(md5_sum_len + 1);

//     for(int i = 0; i < md5_sum_len; i++){
//         sprintf(&hash_str[i], "%02x", md5_sum[i]);
//         // fprintf(file, "%d", hash_str[i]);
//     }

//     hash_str[md5_sum_len] = '\n';
//     return hash_str;
// }


// s_db_entry *get_note_by_date(struct tm *local_time, struct tm *time)
// {
//     struct tm start_date = *local_time;

//     // start_date.tm_mon = 0;
    

//     char filename[50];
//     sprintf(filename, "%s%02d_%04d", DB_DIR, start_date.tm_mon, start_date.tm_year);

//     FILE *fp = fopen(filename, "r");


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

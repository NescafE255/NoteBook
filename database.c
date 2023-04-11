#include "database.h"


#define READ_BUF_SIZE 1024

s_db_entry *near_notes = NULL;
    
// char data_time_str[50];




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




    struct tm strart_date = *local_time;
    strart_date.tm_mday -= 5;
    //it worth to set minutes and hours to 0 here. You don't need to cut the day, right?
    mktime(&strart_date);

    struct tm end_date = *local_time;
    end_date.tm_mday += 15;
    //it worth to set minutes and hours to 0 here. You don't need to cut the day, right?
    mktime(&end_date);

    s_db_entry *buffer = NULL;
    char *out;
    //MAGIC NUMBER! Consider adding some DEFINE for filesize or something
    char filename[50];
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
                exit(0);
            }  

            char info_hash[34];
            memset(info_hash, 0, sizeof(info_hash));
            fgets(info_hash, sizeof(info_hash), fp);


            // strtok(info_hash, "\n");
            printf("HASH STORY %s\n", info_hash);
            printf("HASH NEW %s\n", out);
            if(strcmp(info_hash, out) == 0){
                near_notes = get_note_list(filename);
            }
            else {
                printf("The file is corrupted\n");
                continue;
            }

            dispalay_list(near_notes);

            free(near_notes);
            free(buffer);
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

    //Bring back this line. It is needed for us
    char date_time_str[50];
    sprintf(date_time_str, "%02d.%02d.%04d", note->due_time.tm_mday, note->due_time.tm_mon, note->due_time.tm_year);
    fputs(date_time_str, fp);
    fputs("\n", fp);
}


void store_note(s_db_entry *note)
{
    s_db_entry *buffer = NULL;
    // char *store_hash = NULL;
    char *out;
    // char *out1;
    FILE *fp;
    char filename[30];
    //%s%02d_%d would be better. Spaces in file_name is evel
    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon, note->due_time.tm_year);
    





    fp = fopen(filename, "a");
    if(fp == NULL){
        perror("Error opening file");
        exit(0);
    }

    fseek(fp, 0, SEEK_END);
    // printf("1) %ld\n", ftell(fp));


    if(ftell(fp) == 0){
        fseek(fp, 0, SEEK_SET);
        out = hash_md5(note);
        printf("Pos end out %ld", strlen(out));
        fputs(out, fp);
        save_file(note, fp);
        fclose(fp);
        OPENSSL_free(out);
        return;
    }
    else{
        fp = fopen(filename, "r");
        if(fp == NULL){
            perror("Error opening file");
            exit(0);
        }
        char file_hash[33];
        fgets(file_hash, 33, fp);
        buffer = get_note_list(filename);
        // memset(out, '\0', 33);
        out = hash_md5(buffer);
        strtok(file_hash, "\n");
        strtok(out, "\n");
        printf("%s\n", file_hash);
        printf("%s\n", out);
        if(strcmp(file_hash, out) != 0){
            fp = fopen(filename, "w+");
            out = hash_md5(note);
            fputs(out, fp);
            save_file(note, fp);
            OPENSSL_free(out);
            fclose(fp);
            return;
        } else {
            buffer = get_note_list(filename);
            out = hash_md5(buffer);
            fp = fopen(filename, "w+");
            fputs(out, fp);
            if(buffer == NULL){
                printf("BUFFER NULL");
            }
            while(buffer){
                printf("TITLE: %s\n", buffer->title);
                printf("BODY: %s\n", buffer->body);
                printf("date: %d.%d.%d\n", buffer->due_time.tm_mday, buffer->due_time.tm_mon, buffer->due_time.tm_year);
                save_file(buffer, fp);
                buffer = buffer->next;
            }
             save_file(note, fp);

        }
    }


    
        
        // save_file(note, fp);
        // // printf("%s", file_hash);
        // fclose(fp);


    fclose(fp);
    free(buffer);
    OPENSSL_free(out);
}



char *hash_md5(s_db_entry *note){

    EVP_MD_CTX *mdctx;
    char buf[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11];
    size_t bytes_read;
    s_db_entry *tmp;
    



    // MD5_Init
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

  
    char date_time_str[50];
    if(note == NULL){
        printf("hash NOTE empty!");
    }

    tmp = note;
    while(tmp){
        sprintf(date_time_str, "%02d.%02d.%04d", tmp->due_time.tm_mday, tmp->due_time.tm_mon, tmp->due_time.tm_year);
        // printf("DATE in hash_md5: %s\n", date_time_str);
        bytes_read = snprintf(buf, MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11, "%s\n%s\n%s\n", tmp->title, tmp->body, date_time_str);
        EVP_DigestUpdate(mdctx, buf, bytes_read);
        tmp = tmp->next;
    }


    unsigned char hash_sum[MAX_TITLE_SYMBOLS + MAX_BODY_SYMBOLS + 11];
    unsigned int hash_str_len = EVP_MD_size(EVP_md5());

    
    char *hash_str = (char *)OPENSSL_malloc(hash_str_len * 2 + 1);

    EVP_DigestFinal_ex(mdctx, hash_sum, &hash_str_len);
    EVP_MD_CTX_free(mdctx);

    for(int i = 0; i < hash_str_len; i++){
        sprintf(&hash_str[i * 2], "%02x", hash_sum[i]);
    }

    hash_str[hash_str_len * 2] = '\n';

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

        char date_time_str[50];
        int day, mon, year;
        fgets(date_time_str, sizeof(date_time_str), fp);
        sscanf(date_time_str, "%d.%d.%d", &day, &mon, &year);


        note->due_time.tm_mday = day;
        note->due_time.tm_mon = mon;
        note->due_time.tm_year = year;
        note->next = NULL;

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
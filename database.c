#include "database.h"


#define READ_BUF_SIZE 1024

s_db_entry *near_notes = NULL;
// char data_time_str[50];



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
    strart_date.tm_mday -= 15;
    //it worth to set minutes and hours to 0 here. You don't need to cut the day, right?
    mktime(&strart_date);

    struct tm end_date = *local_time;
    end_date.tm_mday += 30;
    //it worth to set minutes and hours to 0 here. You don't need to cut the day, right?
    mktime(&end_date);

    //MAGIC NUMBER! Consider adding some DEFINE for filesize or something
    char filename[50];
    for(int month = strart_date.tm_mon; month <= end_date.tm_mon; month++){
        for(int year = strart_date.tm_year; year <= end_date.tm_year; year++){
            sprintf(filename, "%s%02d_%d", DB_DIR, month, year);
            printf("%s\n", filename);


            FILE *fp = fopen(filename, "r");
            if(fp == NULL){
                perror("Error opening file");
                exit(0);
            }



            char info_hash[34];
            fgets(info_hash, sizeof(info_hash), fp);
            if(strlen(info_hash) < 33)
                continue;
                // printf("YEs");
            // strtok(info_hash, "\n");
            // info_hash[0] = 0;
            
            char line[MAX_BODY_SYMBOLS];
            while(fgets(line, MAX_TITLE_SYMBOLS, fp) != NULL){
                
                s_db_entry *note = malloc(sizeof(s_db_entry));

                // fgets(line, MAX_TITLE_SYMBOLS, fp);
                // printf("TITLE: %s", line);
                strtok(line, "\n");
                strcpy(note->title, line);



                fgets(line, MAX_BODY_SYMBOLS, fp);
                // printf("BODY: %s", line);
                strtok(line, "\n");
                strcpy(note->body, line);
                
                // printf("\033[0;31m Not end of file\033[0m\n");

                //Why do you malloc tmp here?
                

                char data_time_str[50];
                int day, mon, year;
                fgets(data_time_str, sizeof(data_time_str), fp);
                // printf("Data init_db: %s", data_time_str);
                // strtok(data_time_str, "\n");
                sscanf(data_time_str, "%02d.%02d.%04d", &day, &mon, &year);

                note->due_time.tm_mday = day;
                note->due_time.tm_mon = mon;
                note->due_time.tm_year = year;
                note->next = NULL;

                if(near_notes == NULL){
                    near_notes = note;
                } else {
                    s_db_entry *tmp;
                    tmp = near_notes;
                    while(tmp->next != NULL){
                        tmp = tmp->next;
                    }
                    tmp->next = note;

                }
                
                

                // free(note);
            }

            char *out = hash_md5(near_notes);

            // s_db_entry *temp = near_notes;

            if(near_notes == NULL)
                printf("NULL\n");

            while(near_notes){
                printf("<<<<%s>>>>!\n", near_notes->title);
                printf("<<<<%s>>>>!\n", near_notes->body);
                // strftime(date_time, sizeof(date_time), "%d.%m.%Y", &temp->due_time);
                printf("<<<<<%d.%d.%d>>>>>!\n", near_notes->due_time.tm_mday, near_notes->due_time.tm_mon, near_notes->due_time.tm_year);
                near_notes = near_notes->next;
            }
            near_notes = NULL;

            printf("HASH: %s", info_hash);
            printf("HASH OUT: %s", out);
            if(strcmp(out, info_hash) == 0)
                printf("***Yesss ***");

            // out[0] = 0;
            fclose(fp);
        }

    }
}



void store_note(s_db_entry *note)
{

    FILE *fp;
    char filename[30];
    //%s%02d_%d would be better. Spaces in file_name is evel
    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon, note->due_time.tm_year);


    fp = fopen(filename, "r+");
     if(fp == NULL){
        perror("Error opening file");
        exit(0);
    }

    char *out = hash_md5(near_notes);
    printf("UPDATE hash %s", out);

    
    rewind(fp);
    // printf("%ld", ftell(fp));
    // if(strlen(out) > 0){
    fputs(out, fp);
    // }
    fclose(fp);
    OPENSSL_free(out);
    
    
    
    fp = fopen(filename, "a");
    if(fp == NULL){
        perror("Error opening file");
        exit(0);
    }


    if(strlen(note->title) == 0 || strlen(note->body) == 0 || note->due_time.tm_year == 0){
        printf("Error: Data not written to file - empty fields.\n");
        fclose(fp);
        return;
    }

    // printf("%s", out);

    fputs(note->title, fp);
    fputs(note->body, fp);
    fprintf(fp, "%02d.%02d.%04d",  note->due_time.tm_mday, note->due_time.tm_mon, note->due_time.tm_year);

    //Bring back this line. It is needed for us
    fputs("\n", fp);
    fclose(fp);


    
    

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




// s_db_entry *get_note_by_date(struct tm *local_time, struct tm *time)
// {

    

//     char filename[50];
//     sprintf(filename, DB_DIR, "%s%02d_%04d", DB_DIR, mon, year);

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

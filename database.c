#include "database.h"


#define READ_BUF_SIZE 1024

s_db_entry *near_notes = NULL;


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
            sprintf(filename, "%s%02d_%d", DB_DIR, month + 1, year + 1900);
            printf("%s\n", filename);

            char *hash_key = hash_md5(filename);
            char hash_key_file[READ_BUF_SIZE];

            FILE *fp = fopen(filename, "r");
            if(fp == NULL){
                perror("Error opening file");
		//Why exit there? Unintended exit!
                // break;
            }


            fread(hash_key_file, 1, READ_BUF_SIZE, fp);
            // strtok(hash_key_file, "\n");

            printf("Press key: %s\n", hash_key);
            printf("Press key file: %s\n", hash_key_file);

            if(strcmp(hash_key, hash_key_file) == 0){
                printf("Yes\n");
            }


            // fseek(fp, pos, SEEK_CUR);
            int day, mon, year;
            char line[MAX_BODY_SYMBOLS];
            while(fgets(line, MAX_TITLE_SYMBOLS, fp)){
                
                
                
                s_db_entry *note = malloc(sizeof(s_db_entry));
                // fgets(line, MAX_TITLE_SYMBOLS, fp);
                // printf("%s\n", line);
                strtok(line, "\n");
                strcpy(note->title, line);



                fgets(line, MAX_BODY_SYMBOLS, fp);
                strtok(line, "\n");
                strcpy(note->body, line);
                
                // printf("\033[0;31m Not end of file\033[0m\n");

                //Why do you malloc tmp here?
                

                char data_time_str[50];
                fgets(data_time_str, sizeof(data_time_str), fp);
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

            fclose(fp);
        }

    }
}



void store_note(s_db_entry *note)
{


    char filename[30];
    //%s%02d_%d would be better. Spaces in file_name is evel
    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon + 1, note->due_time.tm_year + 1900);

    // int count = strlen(out);
    // printf("%s", out);

    FILE *fp = fopen(filename, "a");
    if(fp == NULL){
        perror("Error opening file");
        exit(0);
    }


    if(strlen(note->title) == 0 || strlen(note->body) == 0 || note->due_time.tm_year == 0){
        printf("Error: Data not written to file - empty fields.\n");
        fclose(fp);
        return;
    }

    printf("%ld\n", ftell(fp));
    if(ftell(fp) == 0){
        fputs("hash\n", fp);
        printf("LOL");
    }

    fputs(note->title, fp);
    fputs(note->body, fp);

    char date_time_str[50];
    strftime(date_time_str, sizeof(date_time_str), "%d.%m.%Y", &note->due_time);
    fputs(date_time_str, fp);
    //Bring back this line. It is needed for us
    fputs("\n", fp);
    printf("%ld\n", ftell(fp));
    fclose(fp);


    char *out = hash_md5(filename);
    
    fp = fopen(filename, "r+");
    rewind(fp);

    if(strlen(out) > 0){
        fputs(out, fp);
    } else
        printf("Error: Data not written to file - empty hash.\n");
    // printf("%ld\n", ftell(fp));
    // printf("%ld\n", ftell(fp));
    fclose(fp);

    OPENSSL_free(out);

}


char *hash_md5(char *filename){

    EVP_MD_CTX *mdctx;
    FILE *file;
    unsigned char buf[READ_BUF_SIZE];
    size_t bytes_read;



    // MD5_Init
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

    file = fopen(filename, "r");
    if(file == NULL){
        perror("Openong error file");
        exit(1);
    }

    // int pos;
    // if((pos = fread(buf, 1, sizeof(buf), file)) > MAX_TITLE_SYMBOLS){
    //     printf("Pos <%d>\n", pos);
    //     printf("!!!%s\n", buf);
    //     fseek(file, pos, SEEK_CUR);
    // }

    while((bytes_read = fread(buf, 1, sizeof(buf), file)) > 0){
        // printf("%s", buf);
        // printf("%ld", bytes_read);
        EVP_DigestUpdate(mdctx, buf, bytes_read);
    }
        // printf("%s", buf);
        // printf("%ld", bytes_read);

    fclose(file);

    unsigned char md5_sum[MD5_DIGEST_LENGTH];
    unsigned int md5_sum_len = 0;

    EVP_DigestFinal_ex(mdctx, md5_sum, &md5_sum_len);
    EVP_MD_CTX_free(mdctx);
    char *hash_str = (char *)OPENSSL_malloc(md5_sum_len * 2);

    for(int i = 0; i < md5_sum_len; i++){
        sprintf(&hash_str[i * 2], "%02x", md5_sum[i]);
        // fprintf(file, "%d", hash_str[i]);
    }

    return hash_str;
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

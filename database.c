#include <assert.h>
#include "database.h"

#define YEAR 1900
#define MONTH 1
#define START_INIT_DAY -5
#define END_INIT_DAY 15
#define SIZE_HASH 33

// #define SIZE_FILENAME 50

// struct tm global_tm = getCurrentDateTime();
s_db_entry *near_notes = NULL;
s_db_entry *buffer_notes;


struct tm real_date;
struct tm start_date;
struct tm end_date;
    
#define RED "\033[0;31m"
#define NC "\033[0m"
#define PURPLE "\033[0;35m"
#define BLUE "\033[0;34m"




struct tm db_modify_date(struct tm date, int day){
    struct tm tm_date = { 0 };
    tm_date.tm_mday = date.tm_mday;
    tm_date.tm_mon = date.tm_mon;
    tm_date.tm_year = date.tm_year;

    time_t time_date_sec = mktime(&tm_date);
    time_date_sec += day * 24 * 60 * 60;

    return tm_date = *localtime(&time_date_sec);
}


void db_init_date()
{    
    time_t local_time = time(NULL);

    real_date = *localtime(&local_time);
    real_date.tm_mon += 1;
    real_date.tm_year += 1900;

    start_date = db_modify_date(real_date, START_INIT_DAY);
    end_date = db_modify_date(real_date, END_INIT_DAY);


    // return real_date;
    // sprintf(out, "%02d.%02d.%04d", timeinfo->tm_mday, timeinfo->tm_mon + MONTH, timeinfo->tm_year + YEAR);
}



int return_Size_filename(char *filename)
{
    struct stat st;
    // stat(filename, &st);
    if ((stat(filename, &st) == -1)){
        return -1;
    }
    return st.st_size;
}




int db_check_hash (FILE *fp, s_db_entry *notes)
{
    // s_db_entry *notes = NULL;
    char *out = NULL;
    char infohash[SIZE_HASH];
    // char infohash[SIZE_HASH];


    rewind(fp);
    fgets(infohash, sizeof(infohash), fp);
    // fseek(fp, 33, SEEK_SET);
    // notes = get_note_list(fp);

    out = hash_md5(notes);
    assert(out);

    if(strcmp(infohash, out) != 0){
        printf("The file is corrupted\n");

        free_list(notes);
        notes = NULL;
        OPENSSL_free(out);
        // out = NULL;
        return -1;
    }


    // OPENSSL_free(out);
    return 0;
}



void loading_near_notes()
{
    // fill near_notes; localtime - 5 days < ---- > local_time + 15 days

    db_init_date();

    DIR *dir = opendir(DB_DIR);
    if(dir == NULL){
        perror("Error opening dir");
        exit(0);
    }
    closedir(dir);


    int size ;
    char filename[SIZE_FILENAME];
    s_db_entry *buffer = NULL;




    for(int month = start_date.tm_mon; month <= end_date.tm_mon; month++){
        for(int year = start_date.tm_year; year <= end_date.tm_year; year++){
            sprintf(filename, "%s%02d_%d", DB_DIR, month, year);
            // printf("%s%s%s\n", RED, filename, NC);
            FILE *fp;


            fp = fopen(filename, "r");
            if(fp == NULL){
                // printf("FILE ERROR\n");
                continue;
            }
            size = return_Size_filename(filename);
            if(size == 0){
                // printf("SIZE NULL\n");
                continue;
            }
            
            fseek(fp, 33, SEEK_SET);
            buffer = get_note_list(fp);

           if (db_check_hash(fp, buffer) != 0){
                // free_list(buffer);
                // free_list(near_notes);
                // near_notes = NULL;
                remove(filename);
                fclose(fp);
                return;
            }

            fclose(fp);

            while(buffer){

                db_update_near_notes(buffer);
                buffer = buffer->next;
            }

            // display_list(near_notes);

            // free_list(buffer);
            // OPENSSL_free(out);
        }
    }
}


void db_update_near_notes(s_db_entry *note)
{   


    // printf("%d.%d.%d\n", start_date.tm_mday, start_date.tm_mon, start_date.tm_year);


    // printf("%d.%d.%d\n", end_date.tm_mday, end_date.tm_mon, end_date.tm_year);

    // while (note){
        if(mktime(&note->due_time) >= mktime(&start_date) && mktime(&note->due_time) <= mktime(&end_date)){
            // printf("%d.%d.%d\n", starte_date.tm_mday, starte_date.tm_mon, starte_date.tm_year);
            // printf("%d.%d.%d\n", ende_date.tm_mday, ende_date.tm_mon, ende_date.tm_year);
            s_db_entry *tmp = note_dup(note);
            append(&near_notes, tmp);
        }
        // note = note->next;
    // }

}




void store_note(s_db_entry *note)
{

    // db_update_near_notes(note);
    

    char *out;
    // char infohash[SIZE_HASH];
    char filename[SIZE_FILENAME];
    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon, note->due_time.tm_year);
    s_db_entry *buffer = NULL;
    // printf("%s\n", filename);

    
    // memset(out, 0, SIZE_HASH);
    int size = return_Size_filename(filename);
    FILE *fp = fopen(filename, "r+");
    if (fp == NULL){
        fp = fopen(filename, "w+");
        if(fp == NULL){
            perror("Помилка створення файлу!");
            exit(EXIT_FAILURE);
        }
    }

    // assert(out);


    if(size == 0){
        out = hash_md5(note);
        fputs(out, fp);
        fputs("\n", fp);
        save_file(note, fp);
        db_update_near_notes(note);
        // memset(out, 0, SIZE_HASH);
        // OPENSSL_free(out);
        // out = NULL;
        // free(note);
        note = NULL;
        fclose(fp);
        return;
    }

    fseek(fp, 33, SEEK_SET);
    buffer = get_note_list(fp);
    // out = hash_md5(buffer);
    // printf("HASH NEW FILES:  %s\n", out);
    // printf("HASH SAVE FILES: %s\n", file_hash);


    if(db_check_hash(fp, buffer) == 0){
        // printf("ADD NEW NOTE\n");

        s_db_entry *temp = note_dup(note);

        // printf("ADD NEW NOTE2\n");

        append(&buffer, temp);

        // printf("ADD NEW NOTE3\n");

        db_update_near_notes(note);

        // printf("ADD NEW NOTE4\n");

        // OPENSSL_free(out);
        // if (out != NULL){
        //     printf("%s\n", out);
        //     OPENSSL_free(out);
        //     memset(out, 0, SIZE_HASH);
        // }

    } else {
        unlink(filename);
        fclose(fp);
        printf("FILE VBYT\n");


        fp = fopen(filename, "w+");
        if (fp == NULL){
            printf("EROR OPEN FILE\n");
        } else {
            printf("GOOD!\n");
        }



        out = hash_md5(note);
        fputs(out, fp);
        fputs("\n", fp);
        save_file(note, fp);
        db_update_near_notes(note);
        // free(note);
        fclose(fp);
        return;
    }



    // out = NULL;
    out = hash_md5(buffer);
    rewind(fp);
    fputs(out, fp);
    fputs("\n", fp);

    

    s_db_entry *tmp = buffer;
    while(tmp){
        save_file(tmp, fp);
        tmp = tmp->next;
    }

    free_list(buffer);
    // free(note);
    // buffer = NULL;
    // OPENSSL_free(out);
    // if (out != NULL){
    //     printf("%s\n", out);
    //     // OPENSSL_free(out);
    //     // memset(out, 0, SIZE_HASH);
    //     // out = NULL;
    // }
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
    memset(date_time_str, 0, SIZE_FILENAME);
    if(note == NULL){
        // printf("hash NOTE empty!");
        //XXX return NULL here probably! At least no need to continue here!
        return 0;
    }
    if(strlen(date_time_str) > 0){
        printf("SOOOOOS\n");
    }
    tmp = note;
    while(tmp){
        sprintf(date_time_str, "%02d.%02d.%04d", tmp->due_time.tm_mday, 
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



s_db_entry *get_note_by_date(s_db_entry *date)
{
    s_db_entry *buffer = NULL;
    s_db_entry *tmp = NULL;
    FILE *fp;

    // char infohash[SIZE_HASH];
    char filename[SIZE_FILENAME];
    sprintf(filename, "%s%02d_%d", DB_DIR, date->due_time.tm_mon, date->due_time.tm_year);


    int size = return_Size_filename(filename);
    // printf("%d\n", size);
    if (size <= 0){
        printf("Записів за цей місяць не знайдено");
    }

    fp = fopen(filename, "r");

    fseek(fp, 33, SEEK_SET);
    buffer = get_note_list(fp);
    if (db_check_hash(fp, buffer) != 0){
        // free_list(buffer);
        free_list(near_notes);
        // near_notes = NULL;
        remove(filename);
        fclose(fp);   
    }
    fclose(fp);
    s_db_entry *bufferTmp = buffer;
    s_db_entry *temp = NULL;
    while(bufferTmp){
        if(date->due_time.tm_mday == bufferTmp->due_time.tm_mday){
            temp = note_dup(bufferTmp);
            append(&tmp, temp);
        }
        bufferTmp = bufferTmp->next;
    }
    free_list(buffer);
    
    return tmp;
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


char *db_delete_note(int number, s_db_entry *note)
{
    s_db_entry *buffer = NULL;
    s_db_entry *buffer_del = NULL;
    char filename[SIZE_FILENAME];
    char *get = (char *)malloc(MAX_BODY_SYMBOLS);

    sprintf(filename, "%s%02d_%d", DB_DIR, note->due_time.tm_mon, note->due_time.tm_year);


    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        perror("error opening file");
        // return;
        exit(EXIT_FAILURE);
    }


    fseek(fp, 33, SEEK_SET);
    buffer = get_note_list(fp);

    fclose(fp);

    int count = 0;
    s_db_entry *tmp = buffer;
    while(tmp){
        if (tmp->due_time.tm_mday == note->due_time.tm_mday){
            s_db_entry *temp = note_dup(tmp);
            append(&buffer_del, temp);
            count++;
        }
        tmp = tmp->next;
    }

    if(buffer_del == NULL){
        // printf("За цією датую запису не знайдено\n");
        free_list(buffer);
        sprintf(get, "%s", "За цією датую запису не знайдено");
        return get;
    }

    if(count == 1){
            // printf("%s%s%s", BLUE, buffer->title, NC);
            // printf("%s%s%s", BLUE, near_notes->title, NC);
            delete_note_by_date(&near_notes, buffer_del);
            delete_note_by_date(&buffer, buffer_del);

    } else {
        // printf("Обереть нагадування для видалення: ");
        // scanf("%d", &choice);
        tmp = buffer_del;
        for(int i = 1; i < number; i++){
            tmp = tmp->next;
        }
        delete_note_by_date(&near_notes, tmp);
        delete_note_by_date(&buffer, tmp);
    }

    // if (buffer == NULL){
    //     // printf("zfwhauieqlasdfl\n");
    //     remove(filename);
    //     free_list(buffer);
    //     return;
    // }


    char *hash;
    hash = hash_md5(buffer);
    fp = fopen(filename, "w+");
    fputs(hash, fp);
    fputs("\n", fp);
    tmp = buffer;
    while(tmp){
        save_file(tmp, fp);
        tmp = tmp->next;
    }
    fclose(fp);

    OPENSSL_free(hash);
    free_list(buffer);
    
    // buffer_del = NULL;
    // buffer = NULL;
    sprintf(get, "%s", "Запис видалено!");
    return get;
}


/*

void db_delete_note(struct tm *date){
    // char *hash;
    // char infohash[33];
    char filename[SIZE_FILENAME];
    s_db_entry *buffer = NULL;
    s_db_entry *buffer_del = NULL;

    // printf("%s%d:%d%s\n", RED, date->tm_mon, date->tm_year, NC);
    sprintf(filename, "%s%02d_%d", DB_DIR, date->tm_mon, date->tm_year);
    // printf("OpEN filename is delete %s\n", filename);

    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        perror("error opening file");
        return;
    }
    
    // fgets(infohash, sizeof(infohash), fp);
    fseek(fp, 33, SEEK_SET);
    buffer = get_note_list(fp);
    if (db_check_hash(fp, buffer) != 0){
        free_list(near_notes);
        // near_notes = NULL;
        remove(filename);
        // free_list(buffer);
        fclose(fp);
        return;
    }
    fclose(fp);



    int choice;
    int count = 0;

    s_db_entry *tmp = buffer;
    while(tmp){
        if (tmp->due_time.tm_mday ==  date->tm_mday){
                printf("%d. %s%s%02d:%02d:%04d\n", count + 1, tmp->title, tmp->body, tmp->due_time.tm_mday, tmp->due_time.tm_mon, tmp->due_time.tm_year);
                printf("\n");
                s_db_entry *temp = note_dup(tmp);
                append(&buffer_del, temp);
                count++;
                // delete_note_by_date();
        }
        tmp = tmp->next;
    }


    if(buffer_del == NULL){
        printf("За цією датую запису не знайдено\n");
        free_list(buffer);
        return;
    } 

    // free_list(buffer);

    if(count == 1){
            // printf("%s%s%s", BLUE, buffer->title, NC);
            // printf("%s%s%s", BLUE, near_notes->title, NC);
            delete_note_by_date(&near_notes, buffer_del);
            delete_note_by_date(&buffer, buffer_del);

    } else {
        printf("Обереть нагадування для видалення: ");
        scanf("%d", &choice);
        tmp = buffer_del;
        for(int i = 1; i < choice; i++){
            tmp = tmp->next;
        }
        delete_note_by_date(&near_notes, tmp);
        delete_note_by_date(&buffer, tmp);
    }

    if (buffer == NULL){
        // printf("zfwhauieqlasdfl\n");
        remove(filename);
        free_list(buffer);
        return;
    }


    char *hash;
    hash = hash_md5(buffer);
    fp = fopen(filename, "w+");
    fputs(hash, fp);
    fputs("\n", fp);
    tmp = buffer;
    while(tmp){
        save_file(tmp, fp);
        tmp = tmp->next;
    }
    fclose(fp);

    OPENSSL_free(hash);
    free_list(buffer);
    
    // buffer_del = NULL;
    // buffer = NULL;
}

*/
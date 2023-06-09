#include "notebook.h"
#include  <stdlib.h>



struct tm global_time;


/* XXX My consideration (need to be moved to log.c) Do not forget to update .gitignore file
#define LOG_FILE "logs.txt"
FILE *flog;
void init_logger()
{
	flog = fopen(LOG_FILE, "a+");
}
#define LOG(fmt, ...) do { fprintf(flog, "%s %s %s:%d: " fmt "\n", __DATE__, __TIME__, __FILE__, __LINE__, ##__VA_ARGS__); } while (0)
*/

void add_note(){

    //XXX BAD APPROACH CONSTRUCTING LOGS! Consider using define (above)
    //XXX LOG("What a fuck is going on?");
    struct save_log save_log;// = malloc(sizeof(save_log));

    s_db_entry *new_note = malloc(sizeof(s_db_entry));
    printf("Введіть заголовок: ");
    fgets(new_note->title, sizeof(new_note->title), stdin);


    printf("Введіть текст запису: ");
    fgets(new_note->body, sizeof(new_note->body), stdin);

    //XXX LOG("What are you doing here? %s", new_note->body);
    printf("Введіть дату та час, коли потрібно нагадати (у форматі \"dd.mm.yyyy\"): ");
    char input[MAX_NUMBER_DATE];

    scanf("%10[0-9.]", input);

    if(sscanf(input, "%d.%d.%d", &new_note->due_time.tm_mday, &new_note->due_time.tm_mon, &new_note->due_time.tm_year) == 3){
        if (new_note->due_time.tm_mday >= 1 && new_note->due_time.tm_mday <= 31 &&
            new_note->due_time.tm_mon >= 1 && new_note->due_time.tm_mon <= 12 &&
            new_note->due_time.tm_year >= 2000 && new_note->due_time.tm_year <= 2100){
                // printf("SUPER");
            } else {
                printf("Error enter date\n");
                return;
            }
    } else {
        printf("format is not correct\n");
        // fgets(input, sizeof(input), stdin);
        int c;
        while((c = getchar()) != '\n'){}
        return;
    }


    strcpy(save_log.title, new_note->title); 
    strcpy(save_log.body, new_note->body);
    save_log.due_time = new_note->due_time;


    // write_log(&save_log);
    // store_note(new_note);
    

    // free(new_note);
    printf("Запис додано!\n");
}




void show_plansCertainDay(){
    struct tm date; //= { 0 };
    printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");

    if(scanf("%d.%d.%d", &date.tm_mday, &date.tm_mon, &date.tm_year) != 3)
        printf("Помилка введення дати. Спробуйте ще раз.\n");
    
    // char filename[SIZE_FILENAME];
    // sprintf(filename, "%02d.%02d.%04d", date.tm_mday, date.tm_mon, date.tm_year);
    // printf("%s\n", filename);



    // get_note_by_date(&date);
    
}






// void change_notes(){
//     char input[SIZE_FILENAME];
//     char filename[SIZE_FILENAME];
//     s_db_entry *new_note = malloc(sizeof(s_db_entry));
//     printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");
//     fgets(input, sizeof(input), stdin);

//     int day, mon, year;
//     sscanf(input, "%d.%d.%d", &day, &mon, &year);
//     sprintf(filename, "%s02%d_%d", DB_DIR, mon, year);
//     printf("%s\n", filename);

//     printf("Введіть новий заголовок: ");
//     fgets(near_notes->title, sizeof(MAX_TITLE_SYMBOLS), stdin);

//     printf("Введіть новий текст: ");
//     fgets(new_note->body, sizeof(MAX_BODY_SYMBOLS), stdin);

//     printf("Введіть нову дату та час, коли потрінбно нагадати (у форматі \"dd.mm.yyyy\"): ");
//     // char input[50];
//     // fgets(input, sizeof(input), stdin);

//     // int day, mon, year;
    
//     if(scanf("%d.%d.%d", new_note->due_time.tm_mday, new_note->due_time.tm_mon, new_note->due_time.tm_year) != 3){
//         printf("Помилка введення дати. Спробуйте ще раз.\n");
//         // break;
//         return;
//     }



//     printf("Нотатка переписана");    
// }

// Use DB for this purpose
void deleteNote(){
    struct tm date;


    printf("Введіть дату нотатки яку хочете видалити (у форматі \"dd.mm.yyyy\"): ");
    
    if(scanf("%02d.%02d.%04d", &date.tm_mday, &date.tm_mon, &date.tm_year) != 3){
        perror("Погано введена дата!");
        return;
    }
    // char filename[SIZE_FILENAME];
    // sprintf(filename, "%02d.%02d.%04d", date.tm_mday, date.tm_mon, date.tm_year);
    // printf("%s\n", filename);


    //XXX MICHAEL pass struct tm * here
    // db_delete_note(&date);
    printf("Нотатка видалена\n");
}





void display(){
    printf("Оберіть дію:\n");
    printf("1: Додати нагадування\n");
    printf("2: Показати плани на певний день\n");
    printf("3: Змінити нагадування\n");
    printf("4: Видалити нагадування\n");
    printf("5: Вийти!\n");
}


void make_print(){
    int choice;
    display();
    scanf("%d%*c", &choice);
    switch(choice){
        case 1:
            add_note();
            printf("\n");
            break;
        case 2:
            show_plansCertainDay();
            printf("\n");
            break;
        case 3:
            // change_notes();
            printf("\n");
            break;
        case 4:
            deleteNote();
            break;
        case 5:
            printf("Good bay!\n");
            // free_list(buffer_notes);
            free_list(near_notes);
            exit(0);
        default:
            printf("ERRor");
    }
}


void date_init(){
    db_init_date();

    //XXX WHY? ONLY FUNCTIONS! NO VARIABLES! notebook.c and database.c should communicate via function ONLY!
    global_time = real_date;
    global_time.tm_mon -= 1;
    global_time.tm_year -= 1900;


    char date_time[100];
    strftime(date_time, sizeof(date_time), "Date: %Y-%m-%d\nTime: %H:%M:%S\n", &global_time);

    printf("%s", asctime(&global_time));
    printf("%s\n ******\n", date_time);
}



// int main(){
//     //XXX This function is redundant. All off the logic may be done in the init_db function
//     date_init();
//     //init_log_file();
    
//     run_server();
//     init_db(&global_time);
//     // init_logger();
//     while(1){
//         display_list(near_notes);
//         // display_list(buffer_notes);
//         make_print();
    
//     }
    

//     // free_list(near_notes);
//     return 0;
// }
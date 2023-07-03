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

s_db_entry *add_note(){

    //XXX BAD APPROACH CONSTRUCTING LOGS! Consider using define (above)
    //XXX LOG("What a fuck is going on?");
    // struct save_log save_log;// = malloc(sizeof(save_log));

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
                exit(EXIT_FAILURE);
            }
    } else {
        printf("format is not correct\n");
        // fgets(input, sizeof(input), stdin);
        int c;
        while((c = getchar()) != '\n'){}
        exit(EXIT_FAILURE);
    }
    

    // free(new_note);
    // printf("Запис додано!\n");
    return new_note;
}




void show_plansCertainDay(cli_req *request){
    printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");

    if(scanf("%d.%d.%d", &request->note->due_time.tm_mday, &request->note->due_time.tm_mon, &request->note->due_time.tm_year) != 3)
        printf("Помилка введення дати. Спробуйте ще раз.\n");
    
    
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
void deleteNote(cli_req *request){


    printf("Введіть дату нотатки яку хочете видалити (у форматі \"dd.mm.yyyy\"): ");
    // printf("Введіть назву нотатки: ");
    if(scanf("%02d.%02d.%04d", &request->note->due_time.tm_mday, &request->note->due_time.tm_mon, &request->note->due_time.tm_year) != 3){
        perror("Погано введена дата!");
        // return;
        exit(EXIT_FAILURE);
    }

    // char *filename = malloc(SIZE_FILENAME);
    // if(scanf("%s", filename) != 1){
    //     perror("Погано введена нотатка!");
    //     // return;
    //     exit(EXIT_FAILURE);
    // }

    // int lenFilename = strlen(filename);
    // filename[lenFilename] = '\n';

    // sprintf(request, "%s")
    // sprintf(filename, "%s", note->title);
    // printf("%s\n", filename);


    //XXX MICHAEL pass struct tm * here
    // db_delete_note(&date);
    // printf("Нотатка видалена\n");
}


void display(){
    printf("Оберіть дію:\n");
    printf("1: Додати нагадування\n");
    printf("2: Показати плани на певний день\n");
    printf("3: Змінити нагадування\n");
    printf("4: Видалити нагадування\n");
    printf("5: Вийти!\n");
}



int delete_handler(s_db_entry *list) {
    //display the list to the screen
    //return the index of the selected entry to the clie
    return 0;
}


cli_req *make_print(){
    int choice;
    // s_db_entry *note; // = (s_db_entry *)malloc(sizeof(s_db_entry));

    cli_req *request = malloc(sizeof(cli_req));
    request->note = malloc(sizeof(s_db_entry));

    // char *getSprintNote = (char *)malloc(sizeof(2048));
    // char *request;

    display();
    scanf("%d%*c", &choice);
    switch(choice){
        case 1:
            request->note = add_note();
            request->method = STORE;
            // printf("\n");
            break;
        case 2:
            show_plansCertainDay(request);
            request->method = GET;
            break;
        case 3:
            // change_notes();
            printf("\n");
            break;
        case 4:
            deleteNote(request);
            request->method = DELETE;
            //MYKHAILO create a struct with two fields:
            // 1. User input (date of the note in this case)
            // 2. Command from user (DELETE in this case)
            // sprintf(getSprintNote, "%s%s%s%s", "DELETE\n", request, "NULL\n", "NULL\n");
            // free(request);
            break;
        case 5:
            printf("Good bay!\n");
            // free_list(buffer_notes);
            // free_list(near_notes);
            exit(0);
        default:
            printf("ERRor");
    }

    // free(request);
    //MYKHAILO: return user_input struct (see line 208)
    return request;
}


void date_init(){
    time_t local_time = time(NULL);

    global_time = *localtime(&local_time);
    //XXX WHY? ONLY FUNCTIONS! NO VARIABLES! notebook.c and database.c should communicate via function ONLY!
    // global_time = real_date;
    global_time.tm_mon -= 1;
    global_time.tm_year -= 1900;


    char date_time[100];
    strftime(date_time, sizeof(date_time), "Date: %Y-%m-%d\nTime: %H:%M:%S\n", &global_time);

    printf("%s", asctime(&global_time));
    printf("%s\n ******\n", date_time);
}
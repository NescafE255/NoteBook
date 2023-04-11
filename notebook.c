#include "database.h"
// #include <time.h>
// #include <unistd.h>

#define MAX_NOTES 100
#define MAX_TITLdue_timeE_SYMBOLS 50
#define MAX_BODY_SYMBOLS 1000
#define DB_DIR "database/"
#define NAME_FILE DB_DIR"notes.txt"
#define FILE_NAME 50

struct note{
    char title[MAX_TITLE_SYMBOLS];
    char body[MAX_BODY_SYMBOLS];
    // char file_name[FILE_NAME];
    struct tm due_time;
};


struct note notes[MAX_NOTES];
int num_notes = 0;


void add_note(){

    s_db_entry *new_note = malloc(sizeof(s_db_entry));

    printf("Введіть заголовок: ");
    fgets(new_note->title, sizeof(new_note->title), stdin);


    printf("Введіть текст запису: ");
    fgets(new_note->body, sizeof(new_note->body), stdin);

    printf("Введіть дату та час, коли потрібно нагадати (у форматі \"dd.mm.yyyy\"): ");
    // char input[50];
    // fgets(input, sizeof(input), stdin);

    // int day, month, year;

    // if (sscanf(input, "%02d.%02d.%04d", &day, &month, &year) != 3) {
    //     printf("Помилка введення дати. Спробуйте ще раз.\n");
    //     return;
    // }


    if(scanf("%02d.%02d.%04d", &new_note->due_time.tm_mday, &new_note->due_time.tm_mon, &new_note->due_time.tm_year) == 3){
        if (new_note->due_time.tm_mday >= 1 && new_note->due_time.tm_mday <= 31 &&
            new_note->due_time.tm_mon >= 1 && new_note->due_time.tm_mon <= 12 &&
            new_note->due_time.tm_year >= 2000 && new_note->due_time.tm_year <= 2100){
                // printf("SUPER");
            } else {
                printf("Error enter date");
                return;
            }
    } else {
        printf("format is not correct");
        return;
    }


    // scanf("%02d.%02d.%04d", &new_note->due_time.tm_mday, &new_note->due_time.tm_mon, &new_note->due_time.tm_year);
    // if(new_note->due_time.tm_mday && new_note->due_time.tm_mon && new_note->due_time.tm_year != 1){
    //     printf("LOL");        
    // }
    // strtok(new_note->title, "\n");
    // strtok(new_note->body, "\n");
    // new_note->due_time.tm_mday = day;
    // new_note->due_time.tm_mon = month;
    // new_note->due_time.tm_year = year;
    // printf("%s\n", new_note->title);
    // printf("%s\n", new_note->body);
    // printf("%02d%02d.%04d", new_note->due_time.tm_mday, new_note->due_time.tm_mon, new_note->due_time.tm_year);

    store_note(new_note);
    // add_hash_file(new_note);
    

    free(new_note);
    printf("Запис додано!\n");
}


//Move logic or entire function to DB
void show_noteToday(){
	//XXX: Could be simplified. localtime(NULL);
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    int count = 1;
    for(int i = 0; i < num_notes; i++){
        if(notes[i].due_time.tm_year == local_time->tm_year 
        && notes[i].due_time.tm_mon == local_time->tm_mon
        && notes[i].due_time.tm_mday == local_time->tm_mday){
            printf("Сьогодні в тебе такі то справи\n [%d] - %s\n%s", count, notes[i].title, notes[i].body);
            printf("--------\n");
            count++;
        }

    }    
}


//Logic if this function should be passed to DB.
void show_plansCertainDay(){
    printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");

    char input[50];
    fgets(input, sizeof(input), stdin);

    int day, mon, year;

    if(sscanf(input, "%d.%d.%d", &day, &mon, &year) != 3)
        printf("Помилка введення дати. Спробуйте ще раз.\n");
    
    char filename[20];
    sprintf(filename, "%02d %d", mon, year);
    printf("%s", filename);

    // load_notes(filename);
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    tm.tm_mday = day;
    tm.tm_mon = mon - 1;
    tm.tm_year = year - 1900;

    for(int i = 0; i < num_notes; i++){
        if(notes[i].due_time.tm_year == tm.tm_year 
        && notes[i].due_time.tm_mon == tm.tm_mon
        && notes[i].due_time.tm_mday == tm.tm_mday){
            printf("Ваші плани на %s\n", input);
            printf("[%d] - %s\n%s", i+1, notes[i].title, notes[i].body);
            printf("--------\n");
        }
    }
    //XXX Consider stopping program while user examining notes for the certain day
}


void change_notes(){
	//XXX Consider number here. Title is not userfrienldy
    int flag = 0;
    char name_changeNote[MAX_TITLE_SYMBOLS];
    
    printf("Введіть заголовок який хочете змінити\n");
    fgets(name_changeNote, sizeof(name_changeNote), stdin);

    for(int i = 0; i < num_notes; i++){
	    //XXX Be careful! Case sensitive! consider using strcasecmp
        if(strcmp(notes[i].title, name_changeNote) == 0){
            char tmpTitle[MAX_TITLE_SYMBOLS];
            printf("Введіть новий заголовок: ");
            fgets(tmpTitle, sizeof(tmpTitle), stdin);

            // if(chek_titel(name_changeNote) > 0){
            //     printf("Перевищено кількість спроб");
            //     sleep(1);
            //     return;
            // } else
            //     strcpy(notes[i].title, name_changeNote);


            printf("Введіть текст: ");
            fgets(notes[i].body, sizeof(notes->body), stdin);

            printf("Введіть нову дату та час, коли потрінбно нагадати (у форматі \"dd.mm.yyyy\"): ");
            char input[50];
            fgets(input, sizeof(input), stdin);


            int day, mon, year;
            
            if(sscanf(input, "%d.%d.%d", &day, &mon, &year) != 3){
                printf("Помилка введення дати. Спробуйте ще раз.\n");
                break;
            }

	    //Move note change functionality to DB
            notes[i].due_time.tm_mday = day;
            notes[i].due_time.tm_mon = mon - 1;
            notes[i].due_time.tm_year = year - 1900;
	    //XXX flag is redundand. Just return here and that's it
            flag = 1;
            printf("Нотатка переписана");
        } 
    }
    //XXX Remove next line
    if(flag == 0)
        printf("Запис не знайдено!\n");
}

//Use DB for this purpose
void deleteNote(){
    printf("Введіть назву нотатки яку хочете видалити: ");
    char name_delete_note[50];
    fgets(name_delete_note, sizeof(name_delete_note), stdin);
    for(int i = 0; i < num_notes; i++){
	    //XXX NICE! I would not be so smart :)
        if(strcmp(notes[i].title, name_delete_note) == 0){
            notes[i] = notes[num_notes - 1];
        }
    }
    num_notes--;
    printf("Нотатка видалена");
}

//XXX Comment out unused code. #if 0 for example
// void delete_note(){
//     printf("Введіть назву нотатки яку хочете видалити: ");
//     char name_delete_note[50];
//     fgets(name_delete_note, sizeof(name_delete_note), stdin);
//     for(int i = 0; i < num_notes; i++){
//         if(strcmp(notes[i].title, name_delete_note) == 0){
//             for(int j = i; j < num_notes - 1; j++){
//                 printf("%d ", num_notes);
//                 notes[j] = notes[j+1];
//             }
//             num_notes--;
//         }
//     }
//     printf("Нотатка видалена");
// }


void show_note(){
    for(int i = 0; i < num_notes; i++){
        printf("*****\n");
        printf("Заголовок: %s", notes[i].title);
        printf("Сам текст: %s", notes[i].body);
        printf("Date: %02d.%02d.%04d\n",  notes[i].due_time.tm_mday,
                                    notes[i].due_time.tm_mon + 1,
                                    notes[i].due_time.tm_year + 1900);

        printf("*****\n");
    }
}


void getCurrentDateTime(){
    const time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);


    char date_time[100];
    strftime(date_time, sizeof(date_time), "Date: %Y-%m-%d\nTime: %H:%M:%S\n", local_time);

    printf("%s", date_time);
}


void display(){
    printf("Оберіть дію:\n");
    printf("1: Додати нагадування\n");
    printf("2: Переглянути нагадування\n");
    printf("3: Показати плани на певний день\n");
    printf("4: Змінити нагадування\n");
    printf("5: Видалити нагадування\n");
    printf("6: Вийти!\n");
}


void make_print(){
    const time_t curr_time = time(NULL);

    struct tm *local_time;
    local_time = localtime(&curr_time);
    local_time->tm_mon += 1;
    local_time->tm_year += 1900;
    // printf("%02d.%04d\n", local_time->tm_mon, local_time->tm_year);
    int choice;
    display();
    scanf("%d%*c", &choice);
    switch(choice){
        case 1:
            add_note();
            printf("\n");
            break;
        case 2:
	    //init_db should be called before main loop. It is non-sence to do such
            // show_note();
            init_db(local_time);
            // printf("\n");
            break;
        case 3:
            show_plansCertainDay();
            printf("\n");
            break;
        case 4:
            change_notes();
            printf("\n");
            break;
        case 5:
            deleteNote();
            break;
        case 6:
            printf("Good bay!\n");
            exit(0);
        default:
            printf("ERRor");
    }
}


int main(){
    // int chek_num_notes = num_notes;

    //If DIR not exist - ERROR and exit
    //АБО!!!!
    //mkdir(DB_DIR)

    while(1){
        // load_notes();
        getCurrentDateTime();
        show_noteToday();

        make_print();
        // save_notes();

	//Not good. It would be better to make a DB call (function) for such notes.
        // while(near_notes != NULL){
        //     printf("%s\n", near_notes->title);
        //     printf("%s\n", near_notes->body);
        //     printf("%02d.%02d.%04d\n", near_notes->due_time.tm_mday,
        //                             near_notes->due_time.tm_mon,
        //                             near_notes->due_time.tm_year);

        //     near_notes = near_notes->next;
        // }
    }

    return 0;
}

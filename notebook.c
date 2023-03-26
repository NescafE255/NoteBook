#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOTES 100


struct note{
    char title[50];
    char body[1000];
    struct tm due_time;
    // time_t due_date1;
};

struct note notes[MAX_NOTES];
int num_notes = 0;




void add_note(){
    if(num_notes >= MAX_NOTES){
        printf("Досягнуто максимальну кількість записів\n");
        return;
    }

    struct note new_note;
    printf("Введіть заголовок: ");
    fgets(new_note.title, sizeof(new_note.title), stdin);

    printf("Введіть текст запису: ");
    fgets(new_note.body, sizeof(new_note.body), stdin);

    printf("Введіть дату та час, коли потрібно нагадати (у форматі \"dd.mm.yyyy\"): ");
    char input[50];
    fgets(input, sizeof(input), stdin);

    int day, month, year;

    if (sscanf(input, "%d.%d.%d", &day, &month, &year) != 3) {
        printf("Помилка введення дати. Спробуйте ще раз.\n");
        return;
    }


    // struct tm tm;
    // memset(&note, 0, sizeof(struct tm));
    // new_note.due_time->tm_hour;
    new_note.due_time.tm_mday = day;
    new_note.due_time.tm_mon = month - 1;
    new_note.due_time.tm_year = year - 1900;
    new_note.due_time.tm_hour = 0;
    new_note.due_time.tm_min = 0;
    new_note.due_time.tm_sec = 0;
    
    
    // new_note.due_date = mktime(&tm);


    notes[num_notes] = new_note;
    num_notes++;
    printf("Запис додано!\n");
}


// time_t time_now(){
//     const time_t current_time = time(NULL);
//     struct tm *local_time = localtime(&current_time);
//     // local_time->tm_hour = 0;
//     // local_time->tm_min = 0;
//     // local_time->tm_sec = 0;
//     // struct tm tm;
//     // tm.tm_mday = local_time->tm_mday;
//     // tm.tm_mon = local_time->tm_mon;
//     // tm.tm_year = local_time->tm_year;
//     // tm.tm_hour = 0;
//     // tm.tm_min = 0;
//     // tm.tm_sec = 0;

//     return mktime(local_time);
// }

void show_noteToday(){
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

void show_plansCertainDay(){
    printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");

    char input[50];
    fgets(input, sizeof(input), stdin);

    int day, mon, year;

    if(sscanf(input, "%d.%d.%d", &day, &mon, &year) != 3)
        printf("Помилка введення дати. Спробуйте ще раз.\n");
    
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    tm.tm_mday = day;
    tm.tm_mon = mon - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    // time_t time_second = mktime(&tm);

    for(int i = 0; i < num_notes; i++){
        if(notes[i].due_time.tm_year == tm.tm_year 
        && notes[i].due_time.tm_mon == tm.tm_mon
        && notes[i].due_time.tm_mday == tm.tm_mday){
            printf("Ваші плани на %s\n", input);
            printf("[%d] - %s\n%s", i+1, notes[i].title, notes[i].body);
            printf("--------\n");
        }
    }
}

void change_notes(){
    printf("Введіть заголовок який хочете змінити\n");
    char name_titel[50];
    // fgets(name_titel, sizeof(name_titel), stdin);
    scanf("%s", name_titel);

    for(int i = 0; i < num_notes; i++){
        printf("%s", notes[i].title);
        printf("%s", name_titel);
        if(notes[i].title == name_titel){
            printf("%s", notes[i].body);
            break;
        } else
            printf("LOL");
    }
}


void show_note(){
    for(int i = 0; i < num_notes; i++){
        printf("*****\n");
        printf("Заголовок: %s", notes[i].title);
        printf("Сам текст: %s", notes[i].body);
        // struct tm *local_time = localtime(&notes[i].due_time);

        // char data_time[50];
        // strftime(data_time, sizeof(data_time), "Date: %d.%m.%Y\n", local_time);
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

    // printf("Current date and time: %s", asctime(local_time));
    // printf("%ld", current_time);
    printf("%s", date_time);
}

int main(){
    int choice;
    // time_t current_time = time_now();
    // printf("%ld", current_time);


    while(1){
        getCurrentDateTime();
        show_noteToday();
        printf("Оберіть дію:\n");
        printf("1: Додати нагадування\n");
        printf("2: Переглянути нагадування\n");
        printf("3: Показати плани на певний день\n");
        printf("4: Змінити нагадування\n");
        printf("5: Вийти!\n");
        scanf("%d%*c", &choice);
        switch(choice){
            case 1:
                add_note();
                printf("\n");
                break;
            case 2:
                show_note();
                if(num_notes == 0)
                    printf("Empty\n");
                printf("\n");
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
                printf("Good bay!\n");
                exit(0);
            default:
                printf("ERRor");
        }
    }
    return 0;
}
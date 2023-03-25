#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOTES 100

struct note{
    char title[50];
    char body[1000];
    time_t due_date;
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

    printf("Введіть дату та час, коли потрібно нагадати (у форматі \"dd.mm.yyyy hh:mm\"): ");
    char input[50];
    fgets(input, sizeof(input), stdin);

    int day, month, year, hour, minute;

    if (sscanf(input, "%d.%d.%d %d:%d", &day, &month, &year, &hour, &minute) != 5) {
        printf("Помилка введення дати. Спробуйте ще раз.\n");
        return;
    }


    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    
    new_note.due_date = mktime(&tm);


    notes[num_notes] = new_note;
    num_notes++;

    printf("%ld", notes->due_date);
    printf("Запис додано!\n");
}


void show_note(){
    for(int i = 0; i < num_notes; i++){
        printf("*****\n");
        printf("Заголовок: %s", notes->title);
        printf("Сам текст: %s", notes->body);
        struct tm *local_time = localtime(&notes->due_date);

        char data_time[50];
        strftime(data_time, sizeof(data_time), "Date: %Y-%m-%d\nTime: %H:%M:%S\n", local_time);
        printf("%s", data_time);
        printf("*****");
    }


}


void getCurrentDateTime(){
    const time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    char date_time[100];

    strftime(date_time, sizeof(date_time), "Date: %Y-%m-%d\nTime: %H:%M:%S\n", local_time);

    // printf("Current date and time: %s", asctime(local_time));
    printf("%s", date_time);
}

int main(){
    int choice;


    while(1){
        printf("\nОберіть дію:\n");
        printf("1: Додати нагадування\n");
        printf("2: Переглянути нагадування\n");
        printf("3: Показати теперішній час\n");
        printf("4: Вийти!\n");

        scanf("%d%*c", &choice);
        switch(choice){
            case 1:
                add_note();
                break;
            case 2:
                show_note();
                if(num_notes == 0)
                    printf("Empty");
                break;
            case 3:
                getCurrentDateTime();
                break;
            case 4:
                printf("Good bay!");
                exit(0);
            default:
                printf("ERRor");
        }
    }
    return 0;
}
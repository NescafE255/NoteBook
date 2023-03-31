#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_NOTES 100
#define MAX_TITLE_SYMBOLS 50
#define MAX_BODY_SYMBOLS 1000
#define NAME_FILE "notes.txt"


struct note{
    char title[MAX_TITLE_SYMBOLS];
    char body[MAX_BODY_SYMBOLS];
    struct tm due_time;
};


struct note notes[MAX_NOTES];
int num_notes = 0;


void save_notes(int chek_num_notes){
    FILE  *fp = fopen(NAME_FILE, "w+");
    if(fp == NULL){
        printf("Error\n");
        return;
    }

    for(int i = 0; i < num_notes; i++){
        fputs(notes[i].title, fp);
        fputs(notes[i].body, fp);

        char date_time_str[50];
        strftime(date_time_str, sizeof(date_time_str), "%d.%m.%Y", &notes[i].due_time);
        fputs(date_time_str, fp);
        fputs("\n", fp);
    }
    fclose(fp);
}


void load_notes(){
    FILE *fp = fopen(NAME_FILE, "r");
    if(fp == NULL){
        printf("Error\n");
        return;
    }


    num_notes = 0;

    int day, mon, year;
    char line_note[1050];
    while(fgets(line_note, sizeof(line_note), fp)){
        strcpy(notes[num_notes].title, line_note);

        fgets(line_note, sizeof(line_note), fp);
        strcpy(notes[num_notes].body, line_note);

        fgets(line_note, sizeof(line_note), fp);
        sscanf(line_note, "%d.%d.%d", &day, &mon, &year);
        notes[num_notes].due_time.tm_mday = day;
        notes[num_notes].due_time.tm_mon = mon - 1;
        notes[num_notes].due_time.tm_year = year - 1900;

        num_notes++;
    }
    fclose(fp);
}


int chek_titel(char *line, char *print){
    
    int match_found = 0;
    while(match_found < 3){
        printf("%s", print);
        fgets(line, sizeof(line), stdin);

        for(int i = 0; i < num_notes; i++){
            if(strcmp(notes[i].title, line) == 0){
                printf("Запис із таким заголовком існує\n");
                match_found++;
            }
        }
    }
    return match_found;
}


void add_note(){
    if(num_notes >= MAX_NOTES){
        printf("Досягнуто максимальну кількість записів\n");
        return;
    }

    struct note new_note;
    char new_title[MAX_TITLE_SYMBOLS];
    char *print = "Введіть заголовок: ";

    //XXX Could be simplified to if (check_title(...))

    if(chek_titel(new_title, print) > 0){
        sleep(1);
        return;
    } else
        strcpy(new_note.title, new_title);




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

    new_note.due_time.tm_mday = day;
    new_note.due_time.tm_mon = month - 1;
    new_note.due_time.tm_year = year - 1900;

    notes[num_notes] = new_note;
    num_notes++;
    printf("Запис додано!\n");
}


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
    char name_changeNote[50];
    
    printf("Введіть заголовок який хочете змінити\n");
    fgets(name_changeNote, sizeof(name_changeNote), stdin);

    for(int i = 0; i < num_notes; i++){
	    //XXX Be careful! Not case sensitive
        if(strcmp(notes[i].title, name_changeNote) == 0){
            char *print = "Введіть новий заголовок: ";
            char tmpTitle[MAX_TITLE_SYMBOLS];

            if(chek_titel(tmpTitle, print) > 0){
                sleep(1);
                return;
            } else
                strcpy(notes[i].title, tmpTitle);


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



int main(){
    int choice;
    int chek_num_notes = num_notes;

    while(1){
        load_notes();
        getCurrentDateTime();
        show_noteToday();
        printf("Оберіть дію:\n");
        printf("1: Додати нагадування\n");
        printf("2: Переглянути нагадування\n");
        printf("3: Показати плани на певний день\n");
        printf("4: Змінити нагадування\n");
        printf("5: Видалити нагадування\n");
        printf("6: Вийти!\n");
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
                deleteNote();
                break;
            case 6:
                printf("Good bay!\n");
                exit(0);
            default:
                printf("ERRor");
        }
        save_notes(chek_num_notes);
    }

    return 0;
}

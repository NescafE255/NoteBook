#include "notebook.h"


struct tm *global_time;
// s_db_entry *buffer_notes;



void add_note(){

    s_db_entry *new_note = malloc(sizeof(s_db_entry));
    // global_time = getCurrentDateTime();
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

    store_note(new_note, global_time);
    

    // free(new_note);
    printf("Запис додано!\n");
}




void show_plansCertainDay(){
    printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");

    char input[SIZE_FILENAME];
    fgets(input, sizeof(input), stdin);

    int day, mon, year;

    if(sscanf(input, "%d.%d.%d", &day, &mon, &year) != 3)
        printf("Помилка введення дати. Спробуйте ще раз.\n");
    
    char filename[SIZE_FILENAME];
    sprintf(filename, "%s%02d_%d", DB_DIR, mon, year);
    printf("%s\n", filename);



    if(buffer_notes == NULL){
        buffer_notes = get_note_by_date(filename);
        add_note_bufferOFmon(day, mon, year);
    } else {
        if(buffer_notes->due_time.tm_mon != mon){
            free_list(buffer_notes);
            buffer_notes = get_note_by_date(filename);
        }
        add_note_bufferOFmon(day, mon, year);
    }
    // free_list(buffer_notes);
    
}

void getCurrentDateTime(){
    const time_t current_time = time(NULL);
    global_time = localtime(&current_time);   
}



// void change_notes(){
//     char input[SIZE_FILENAME];
//     char filename[SIZE_FILENAME];

//     printf("Введіть дату (у форматі \"dd.mm.yyyy\"): ");
//     sprintf

//     int day, mon, year;
//     sscanf(input, "%d.%d.%d", &day, &mon, &year);




//             printf("Введіть текст: ");
//             fgets(notes[i].body, sizeof(notes->body), stdin);

//             printf("Введіть нову дату та час, коли потрінбно нагадати (у форматі \"dd.mm.yyyy\"): ");
//             char input[50];
//             fgets(input, sizeof(input), stdin);


//             int day, mon, year;
            
//             if(sscanf(input, "%d.%d.%d", &day, &mon, &year) != 3){
//                 printf("Помилка введення дати. Спробуйте ще раз.\n");
//                 break;
//             }

// 	    //Move note change functionality to DB
//             notes[i].due_time.tm_mday = day;
//             notes[i].due_time.tm_mon = mon - 1;
//             notes[i].due_time.tm_year = year - 1900;
// 	    //XXX flag is redundand. Just return here and that's it
//             flag = 1;
//             printf("Нотатка переписана");
//         } 
//     }
//     //XXX Remove next line
//     if(flag == 0)
//         printf("Запис не знайдено!\n");
// }

//Use DB for this purpose
// void deleteNote(){
//     printf("Введіть назву нотатки яку хочете видалити: ");
//     char name_delete_note[50];
//     fgets(name_delete_note, sizeof(name_delete_note), stdin);
//     for(int i = 0; i < num_notes; i++){
// 	    //XXX NICE! I would not be so smart :)
//         if(strcmp(notes[i].title, name_delete_note) == 0){
//             notes[i] = notes[num_notes - 1];
//         }
//     }
//     num_notes--;
//     printf("Нотатка видалена");
// }

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
            // deleteNote();
            break;
        case 5:
            printf("Good bay!\n");
            free_list(buffer_notes);
            free_list(near_notes);
            exit(0);
        default:
            printf("ERRor");
    }
}


int main(){
    // global_time = getCurrentDateTime();
    getCurrentDateTime();
    char date_time[100];
    strftime(date_time, sizeof(date_time), "Date: %Y-%m-%d\nTime: %H:%M:%S\n", global_time);

    printf("%s", asctime(global_time));
    printf("%s\n ******\n", date_time);
    init_db(global_time);
    while(1){
        // display_list(near_notes);
        // display_list(buffer_notes);
        make_print();
        


    }
    

    free_list(near_notes);
    return 0;
}
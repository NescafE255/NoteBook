#include "server.h"
// #include "notebook.h"


enum requestMethod getRequestMethod(char *request){
    if (strncmp(request, "STORE", 5))
        return STORE;
    else if (strncmp(request, "GET", 3))
        return GET;
    else if (strncmp(request, "RENAME", 6))
        return RENAME;
    else if (strncmp(request, "DELETE", 6))
        return DELETE;
    else
        return UNKNOWN;
}

void handleRequest(char *request){
    char words[4][MAX_BODY_SYMBOLS];
    int numWords = 0;
    s_db_entry *note = (s_db_entry *)malloc(sizeof(s_db_entry));

    char *str = strtok(request, " ");
    printf("STR: %s\n", str);
    enum requestMethod method = getRequestMethod(request);
    printf("method %d\n", method);

    while(str != NULL && numWords <= 3){
        int lenWord = strlen(str);
        memset(words[numWords], 0, MAX_BODY_SYMBOLS - 1);
        if (lenWord > MAX_BODY_SYMBOLS)
            strncpy(words[numWords], str, MAX_BODY_SYMBOLS - 1);
        else
            strncpy(words[numWords], str, lenWord);
        
        numWords++;
        str = strtok(NULL, " ");
    }


    int count = 0;
    while(count <= 3){
        printf("%s\n", words[count]);
        count++;
    }


    strncpy(note->title, words[1], MAX_TITLE_SYMBOLS - 1);
    strcat(note->title, "\n");

    strncpy(note->body, words[2], MAX_BODY_SYMBOLS - 1);
    strcat(note->body, "\n");
    sscanf(words[3], "%d.%d.%d", &note->due_time.tm_mday, &note->due_time.tm_mon, &note->due_time.tm_year);
    note->next = NULL;

    display_list(note);

    switch (method) {
        case STORE:
            // add_note();
            printf("case: STORE\n");
            store_note(note);
            break;
        case GET:
            // show_plansCertainDay();
            struct tm *data = malloc(sizeof(data));
            data->tm_mday = note->due_time.tm_mday;
            data->tm_mon = note->due_time.tm_mon;
            data->tm_year = note->due_time.tm_year;

            get_note_by_date(data);
            free(data);
            break;
        case RENAME:
            break;
        case DELETE:
            // deleteNote();
            struct tm *date = malloc(sizeof(date));
            date->tm_mday = note->due_time.tm_mday;
            date->tm_mon = note->due_time.tm_mon;
            date->tm_year = note->due_time.tm_year;

            db_delete_note(date);
            free(date);
            break;
        case UNKNOWN:
            perror("request bad");
            break;
    }

    free(note);
}



void *clientHandler(void *arg){
    int clientSocket = *((int *) arg);
    char buffer[2048] = {0};

    int bytesRead;
    while((bytesRead = read(clientSocket, buffer, sizeof(buffer)))){
        if (bytesRead <= 0){
            // close(clientSocket);
            break;
        }
        // char *methodrequ = buffer;
        // char *meth = strtok(methodrequ, " ");
        // printf("BUFFER: %s\n", meth);
        // printf("BUFFER: %s\n", buffer);
        

        // enum requestMethod method = getRequestMethod(buffer);
        handleRequest(buffer); 
    }
    
    close(clientSocket);
    return NULL;
}




void run_server(){
    int socket_server, socket_client;
    struct sockaddr_in server_addres, client_addres;
    socklen_t len = sizeof(client_addres);
    pthread_t thread;

    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Socked failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }


    server_addres.sin_family = AF_INET;
    server_addres.sin_port = htons(SERVER_PORT);
    server_addres.sin_addr.s_addr = INADDR_ANY;

    if ((bind(socket_server, (struct sockaddr*) &server_addres, sizeof(server_addres)))){
        perror("Faild to bind socket");
        exit(EXIT_FAILURE);
    }

    printf("Starting server\n");

    int clientCount = 0;
    while(1){

        clientCount++;
        if(clientCount >= 5){
            printf("Max size client");
            break;
        }


        if ((listen(socket_server, 5))){
            perror("Faild listen");
            exit(EXIT_FAILURE);
        }

        if((socket_client = accept(socket_server, (struct sockaddr *)&client_addres, &len)) < 0){
            perror("Faild to accept");
            exit(EXIT_FAILURE);
        }


        int result = pthread_create(&thread, NULL, clientHandler, &socket_client);
        if (result == 0){
            result = pthread_detach(thread);
            if (result != 0){
                perror("Faild detach");
                break;
            }

        } else {
            perror("Faild creat potok");
            break;
        }



        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addres.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("connect %s:%d\n", client_ip, ntohs(client_addres.sin_port));


        char server_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &server_addres.sin_addr, server_ip, INET_ADDRSTRLEN);
        printf("connect %s:%d\n", server_ip, ntohs(server_addres.sin_port));




    }

}
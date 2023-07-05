#include "server.h"
#define BUFFER_SIZE 2048


enum requestMethod getRequestMethod(char *request){
    if (strncmp(request, "STORE", 5) == 0)
        return STORE;
    else if (strncmp(request, "GET", 3) == 0)
        return GET;
    else if (strncmp(request, "RENAME", 6) == 0)
        return RENAME;
    else if (strncmp(request, "DELETE", 6) == 0)
        return DELETE;
    else
        return UNKNOWN;
}



char *returnGet(s_db_entry *note){

    char *get = (char *)calloc(2048, sizeof(char));
    char *sprintfNote = (char *)calloc(2048, sizeof(char));
    while (note){
        sprintf(sprintfNote, "%s%s%d.%d.%d", note->title, note->body, note->due_time.tm_mday, note->due_time.tm_mon, note->due_time.tm_year);
        strcat(get, sprintfNote);
        strcat(get, "\n");
        note = note->next;
    }

    // printf("returnGET: %s\n", get);

    free(sprintfNote);
    return get;
}


char *creatingRequest(enum requestMethod method, s_db_entry *note){
    char *get = (char *)calloc(2048, sizeof(char));
    s_db_entry *tmp = NULL;
    char *request = NULL;
    switch (method) {
    case STORE:
        // printf("case: STORE\n");
        store_note(note);
        get = "Запис додано!\n";
        break;
    case GET:
        // printf("case: GET\n");
        tmp = get_note_by_date(note);
        request = returnGet(tmp);
        sprintf(get, "%s%s", "GET\n", request);
        break;
    case RENAME:
        break;
    case DELETE:
        // printf("case: DELETE\n");
        tmp = get_note_by_date(note);
        request = returnGet(tmp);
        sprintf(get, "%s%s", "DELETE\n", request);
        break;
    case UNKNOWN:
        perror("request bad");
        break;
    }


    free(tmp);
    free(request);
    return get;
}


s_db_entry *handleRequest(char *request){
    char words[4][MAX_BODY_SYMBOLS];
    s_db_entry *note = (s_db_entry *)malloc(sizeof(s_db_entry));
    int numWords = 0;


    char *str = strtok(request, "\n");
    // printf("method %d\n", method);

    while(str != NULL && numWords <= 3){
        int lenWord = strlen(str);
        // printf("%s\n", str);
        memset(words[numWords], 0, MAX_BODY_SYMBOLS - 1);
        if (lenWord > MAX_BODY_SYMBOLS)
            strncpy(words[numWords], str, MAX_BODY_SYMBOLS - 1);
        else
            strncpy(words[numWords], str, lenWord);
        
        numWords++;
        str = strtok(NULL, "\n");
    }


    strncpy(note->title, words[1], MAX_TITLE_SYMBOLS - 1);
    strcat(note->title, "\n");

    strncpy(note->body, words[2], MAX_BODY_SYMBOLS - 1);
    strcat(note->body, "\n");
    sscanf(words[3], "%d.%d.%d", &note->due_time.tm_mday, &note->due_time.tm_mon, &note->due_time.tm_year);
    note->next = NULL;
    // if (str != NULL || numWords < 4){
    //     printf("STR == NULL");
    // }
    // printf("HandleRE %d:%d:%d\n", note->due_time.tm_mday, note->due_time.tm_mon, note->due_time.tm_year);
    // free(note);
    return note;
}



void *clientHandler(void *arg){
    int clientSocket = *((int *) arg);
    char buffer[BUFFER_SIZE] = {0};
    s_db_entry *note = NULL;
    char *messange;
    // char *near;
    int number = 0;
    // int byteread;
    int size_byte = 0;

    while(1){

    // display_list(near_notes);
    if(near_notes != NULL){
        messange = returnGet(near_notes);
        size_byte = strlen(messange);
        if (send(clientSocket, messange, size_byte, 0) == -1){
            perror("error recv");
            exit(EXIT_FAILURE);
        }
        // printf("returnGET: %s\n", messange);
        memset(messange, 0, size_byte);
        size_byte = 0;
    }

    int bytesRead;
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        // printf("Read bytes: %d\n", bytesRead);
        buffer[bytesRead] = '\0';
        // printf("Read buffer: %s\n", buffer);
        if (bytesRead <= 0){
            // close(clientSocket);
            break;
        }
        enum requestMethod method = getRequestMethod(buffer);
        note = handleRequest(buffer);
        messange = creatingRequest(method, note);
        printf("masenge: %s\n", messange);

        if(send(clientSocket, messange, strlen(messange) - 1, 0) == -1){
            perror("failed send to client");
            exit(EXIT_FAILURE);
        }


        if (method == DELETE){
           if (recv(clientSocket, &number, sizeof(int), 0) == -1){
                perror("error recv");
                exit(EXIT_FAILURE);
           }
            char *mem = db_delete_note(number, note);
            // printf("NUMBER %d\n", number);

            if (send(clientSocket, mem, strlen(mem), 0) == -1){
                perror("failed send to client number");
                exit(EXIT_FAILURE);
            }

        }
    }

    
    // close(clientSocket);
    // free(messange);
    free(messange);
    free(note);
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

 
        if ((listen(socket_server, 5))){
            perror("Faild listen");
            exit(EXIT_FAILURE);
        }
        
   int clientCount = 0;
    loading_near_notes();

    while(1){

        clientCount++;
        if(clientCount >= 5){
            printf("Max size client");
            break;
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


int main(){
    // date_init();
    // init_log_file();
    // init_db(&global_time);
    // date_init();
    run_server();
    // clientBind();
    

    return 0;
}
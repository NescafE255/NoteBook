#include "client.h"
#define BUFFER_SIZE 2048


s_db_entry *notes = NULL;

enum requestMethod getRequestMethod(char *request)
{
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



void analyzeData(char *request)
{
    enum requestMethod method = getRequestMethod(request);
    char *str = strtok(request, "\n");
    s_db_entry *tmp = (s_db_entry *)malloc(sizeof(s_db_entry));

    memset(tmp, 0, sizeof(*tmp));

    switch (method) {
        case STORE:
            printf("%s\n", request);
            break;
        case GET:

            while (str != NULL) {
                str = strtok(NULL, "\n");

                // memset(tmp->title, 0, strlen(tmp->title));
                strncpy(tmp->title, str, strlen(str));
                strcat(tmp->title, "\n");
                printf("TITLE WHILE: %s", tmp->title);

                str = strtok(NULL, "\n");
                
                // memset(tmp->body, 0, strlen(tmp->body));
                strncpy(tmp->body, str, strlen(str));
                strcat(tmp->body, "\n");
                printf("BODY WHILE: %s", tmp->body);

                str = strtok(NULL, "\n");
                // tmp->due_time.tm_mday = 0;
                // tmp->due_time.tm_mon = 0;
                // tmp->due_time.tm_year = 0;
                sscanf(str, "%d:%d:%d", &tmp->due_time.tm_mday, &tmp->due_time.tm_mon, &tmp->due_time.tm_year);
                // note_dup(tmp);
                printf("DATE WHILE: %d:%d:%d\n\n", tmp->due_time.tm_mday, tmp->due_time.tm_mon, tmp->due_time.tm_year);
                // tmp->next = NULL;
                // s_db_entry *note = note_dup(tmp);
                append(&notes, tmp);
            }
            // display_list(notes);
            break;
        case RENAME:
            break;
        case DELETE:
            break;
        case UNKNOWN:
            break;
    }


}


char *sendRequest(cli_req *request){

    // s_db_entry *note = (s_db_entry *)malloc(sizeof(s_db_entry));
    enum requestMethod method = request->method;

    // printf("%d:%d:%d", request->note->due_time.tm_mday, request->note->due_time.tm_mon, request->note->due_time.tm_year);
    char *getSprintNote = (char *)malloc(BUFFER_SIZE);
    
    switch (method) {
        case STORE:
            // printf()
            // note = add_note();
            sprintf(getSprintNote, "%s%s%s%d.%d.%d", "STORE\n", request->note->title, request->note->body, request->note->due_time.tm_mday, request->note->due_time.tm_mon, request->note->due_time.tm_year);
            break;
        case GET:
            sprintf(getSprintNote, "%s%s%s%d.%d.%d", "GET\n", "NULL\n", "NULL\n", request->note->due_time.tm_mday, request->note->due_time.tm_mon, request->note->due_time.tm_year);
            break;
        case RENAME:
            printf("RENAME\n");
            break;
        case DELETE:
            sprintf(getSprintNote, "%s%s%s%d.%d.%d", "DELETE\n", "NULL\n", "NULL\n", request->note->due_time.tm_mday, request->note->due_time.tm_mon, request->note->due_time.tm_year);
            break;
        case UNKNOWN:
            printf("UNknown\n");
            break;
    }


    // free(request->note);
    // free(request);
    return getSprintNote;
}


void *clientRequest(void *arg) {
    int clientSocket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int byteRecv = 0;
    int bytesSent = 0;
    int lenRecv = 0;

    fd_set readfds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    FD_SET(clientSocket, &readfds);
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int result = select(clientSocket + 1, &readfds, NULL, NULL, &timeout);
    if (result == -1) {
        perror("select");
        exit(EXIT_FAILURE);
    } else if (result == 0) {
        printf("Ближайших нотатків не має!\n");
    } else {
        byteRecv = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (byteRecv == -1) {
            perror("error recv");
            exit(EXIT_FAILURE);
        }
        printf("%.*s\n", byteRecv, buffer);
        memset(buffer, 0, byteRecv);
    }

    cli_req *req = make_print();
    char *request = sendRequest(req);
    int lenSent = strlen(request);

    while (bytesSent < lenSent) {
        int sent = send(clientSocket, request + bytesSent, lenSent - bytesSent, 0);
        if (sent == -1) {
            perror("error send get");
            exit(EXIT_FAILURE);
        }
        bytesSent += sent;
    }
    memset(buffer, 0, BUFFER_SIZE);
    char recvbuffer[BUFFER_SIZE];
    memset(recvbuffer, 0, BUFFER_SIZE);
    while (lenRecv < lenSent) {
        printf("BUFFER DO: %.*s\n", byteRecv, recvbuffer);
        byteRecv = recv(clientSocket, recvbuffer, BUFFER_SIZE, 0);
        if (byteRecv == -1) {
            perror("error recv");
            exit(EXIT_FAILURE);
        } else if (byteRecv == 0) {
            continue;
        }


        printf("BUFFER: %.*s\n", byteRecv, recvbuffer);
        printf("-------------\n");
        analyzeData(recvbuffer);
        lenRecv += byteRecv;
        memset(recvbuffer, 0, byteRecv);
    }

    if (req->method == DELETE) {
        int number;
        printf("Введіть запис, який хочете видалити: ");
        scanf("%d", &number);
        bytesSent = send(clientSocket, &number, sizeof(int), 0);
        if (bytesSent == -1) {
            perror("error send to number of DELETE");
            exit(EXIT_FAILURE);
        }
        memset(request, 0, lenSent);
        byteRecv = recv(clientSocket, request, BUFFER_SIZE, 0);
        if (byteRecv == -1) {
            perror("error recv");
            exit(EXIT_FAILURE);
        }
        printf("%.*s\n", byteRecv, request);
    }

    free(request);
    return NULL;
}





// void *clientRequest(void *arg){
//     int clientSocket = *((int *) arg);
//     // int byte;
//     char buffer[BUFFER_SIZE];
//     int byteRecv;
//     int bytesSent;
//     int lenRecv = 0;
    

//     fd_set fds;
//     struct timeval timeout;

//     FD_ZERO(&fds);
//     FD_SET(clientSocket, &fds);

//     timeout.tv_sec = 2;
//     timeout.tv_usec = 0;


//     int result = select(clientSocket+1, &fds, NULL, NULL, &timeout);
//     if (result == -1){
//         perror("select");
//         exit(EXIT_FAILURE);
//     } else if (result == 0) {
//         printf("Ближайших нотатків не має!\n");
//     } else {
//         if ((byteRecv = recv(clientSocket, buffer, BUFFER_SIZE, 0)) == -1){
//             perror("error recv");
//             exit(EXIT_FAILURE);
//         }
//         printf("%s\n", buffer);
//         memset(buffer, 0, BUFFER_SIZE);
//     }

//     cli_req *req = make_print();
//     char *request = sendRequest(req);
//     int lenSent = strlen(request);

    


//     while (1){

//         if ((bytesSent = send(clientSocket, request, strlen(request), 0)) == -1) {
//             perror("error send get");
//             exit(EXIT_FAILURE);
//         }

//         if (bytesSent == lenSent){
//             memset(request, 0, lenSent);
//             break;
//         }

//     }

//     // free(request);


//     while (1){
//         if (byteRecv == lenRecv){
//             memset(buffer, 0, BUFFER_SIZE);
//             break;
//         }

//         if ((byteRecv = recv(clientSocket, buffer, BUFFER_SIZE, 0)) == -1) {
//             perror("error recv");
//             exit(EXIT_FAILURE);
//         } else if (byteRecv == 0){
//             continue;
//         }
//         printf("BUFFER:%s\n", buffer);
//         analyzeData(buffer);


//         lenRecv += byteRecv;
//     }

//     bytesSent = 0;
//     int number;
//     if (req->method == DELETE){
//         memset(request, 0, BUFFER_SIZE);
//         printf("Введіть запис який хочете видалити: ");
//         scanf("%d", &number);

//         if ((bytesSent = send(clientSocket, &number, sizeof(int), 0)) == -1){
//             perror("eeror send to number of DELETE");
//             exit(EXIT_FAILURE);
//         }
//         memset(request, 0, BUFFER_SIZE);
//         if (recv(clientSocket, request, BUFFER_SIZE, 0) == -1){
//             perror("error recv");
//             exit(EXIT_FAILURE);
//         }

//         printf("%s\n", request);

        
//     }
//     // memset(buffer, 0, BUFFER_SIZE);

//     //MYKHAILO: in case of DELETE do addititonal request to the server
//     //MYKHAILO: send the number to the server
//     // close(clientSocket);

//     free(request);
//     return NULL;
// }



void clientBind(){
    int client_socket;
    struct sockaddr_in server_address;
    // char request[2048] = {0};

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1){
        perror("Error creat socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
        perror("Error conect server");
        exit(EXIT_FAILURE);
    }
    while(1){
        clientRequest(&client_socket);
    }



}


int main(){
    clientBind();
}
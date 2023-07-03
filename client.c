#include "client.h"
#define BUFFER_SIZE 2048

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





void *clientRequest(void *arg){
    int clientSocket = *((int *) arg);
    // int byte;
    

    cli_req *req = make_print();
    char *request = sendRequest(req);
    char buffer[BUFFER_SIZE];
    int bytesSent;
    int byteRecv;
    int lenSent = strlen(request);
    int lenRecv = 0;

    

    if ((byteRecv = recv(clientSocket, buffer, BUFFER_SIZE, 0)) == -1){
        perror("error recv");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);

    while (1){

        if ((bytesSent = send(clientSocket, request, strlen(request), 0)) == -1) {
            perror("error send get");
            exit(EXIT_FAILURE);
        }

        if (bytesSent == lenSent){
            memset(request, 0, lenSent);
            break;
        }

    }

    // free(request);


    while (1){
        if (byteRecv == lenRecv){
            memset(buffer, 0, BUFFER_SIZE);
            break;
        }

        if ((byteRecv = recv(clientSocket, buffer, BUFFER_SIZE, 0)) == -1) {
            perror("error recv");
            exit(EXIT_FAILURE);
        } else if (byteRecv == 0){
            continue;
        }
        printf("%s\n", buffer);

        lenRecv += byteRecv;
    }

    bytesSent = 0;
    int number;
    if (req->method == DELETE){
        memset(request, 0, BUFFER_SIZE);
        printf("Введіть запис який хочете видалити: ");
        scanf("%d", &number);

        if ((bytesSent = send(clientSocket, &number, sizeof(int), 0)) == -1){
            perror("eeror send to number of DELETE");
            exit(EXIT_FAILURE);
        }
        memset(request, 0, BUFFER_SIZE);
        if (recv(clientSocket, request, BUFFER_SIZE, 0) == -1){
            perror("error recv");
            exit(EXIT_FAILURE);
        }

        printf("%s\n", request);

        
    }
    // memset(buffer, 0, BUFFER_SIZE);

    //MYKHAILO: in case of DELETE do addititonal request to the server
    //MYKHAILO: send the number to the server
    // close(clientSocket);

    free(request);
    return NULL;
}



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
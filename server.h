// #ifndef SERVER
// #define SERVER

// #include "database.h"
// #include "list.h"
#include "notebook.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>

#define SERVER_PORT 9002
#define SERVER_IP "127.0.0.0"
#define MAX_CLIENT 5
// #define 

enum requestMethod {
    STORE,
    GET,
    RENAME,
    DELETE,
    UNKNOWN
};

void run_server();
void *clientHandler(void *arg);
void handleRequest(char *requst);
enum requestMethod getRequestMethod(char *requst);



// #endif
// #include "server.h"
#include "notebook.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define SERVER_PORT 9002


void *clientRequest(void *arg);
void clientBind();
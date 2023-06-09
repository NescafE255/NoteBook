#include "server.h"

int main(){
    date_init();
    init_log_file();
    init_db(&global_time);
    date_init();
    run_server();
    

    return 0;
}
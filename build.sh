#/bin/bash

command="gcc -Wall -Werror -g server.c main.c logger.c notebook.c database.c list.c -lcrypto -lssl"


echo ${command}
exec ${command}

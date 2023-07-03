#/bin/bash

command="gcc -Wall -Werror -g server.c logger.c database.c list.c -lcrypto -lssl"


echo ${command}
exec ${command}

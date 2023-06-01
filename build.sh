#/bin/bash

command="gcc -Wall -Werror -g notebook.c database.c list.c -lcrypto -lssl"


echo ${command}
exec ${command}

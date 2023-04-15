#/bin/bash

command="gcc -Wall -Werror -g notebook.c database.c list.c -lcrypto"


echo ${command}
exec ${command}

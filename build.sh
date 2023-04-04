#/bin/bash

command="gcc -Wall -Werror -g notebook.c database.c"


echo ${command}
exec ${command}

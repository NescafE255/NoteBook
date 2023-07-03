#/bin/bash

command="gcc -Wall -Werror -g client.c notebook.c list.c -o client"


echo ${command}
exec ${command}

#include <stdio.h>
#include <stdlib.h>

void run(){

    fwrite("Good... Wait what?\n", 1, 19, stdout);
    system("/bin/sh");
 }

int main (){

    char buff[64];

    gets(buff);

    return 0;
}
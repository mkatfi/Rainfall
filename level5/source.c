
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void o(char *print) {

system("/bin/sh");

    _exit(1);
}


void n() {
    char buff[520];

    fgets(buff, 512, stdin);

    printf(buff);

    exit(1);

    return;
}

int main () {

    n();

    return 0;
}
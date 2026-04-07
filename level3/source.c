
#include <stdio.h>

int m = 0;

void v() {
    char buff[520];

    fgets(buff, 512, stdin);

    printf(buff);

    if (m == 64) {
        fwrite("Wait what?!\n", 12, 1, stdout);
        system("/bin/sh");
    }

    return;
}

int main () {

    v();

    return 0;
}
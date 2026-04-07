#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

char c[200];

void m() {

    printf("%s - %d\n", c, (int)time(0));
    
    return ;
}

int main (int ac, char ** av) {
    char *ptr;
    char *ptr2;

    ptr = malloc(sizeof(char) * 8);

    *(int *)(&ptr[0]) = 1;
    *(void **)(&ptr[4]) = malloc(sizeof(char) * 8);


    ptr2 = malloc(sizeof(char) * 8);

    *(int *)(&ptr2[0]) = 2;
    *(void **)(&ptr2[4]) = malloc(sizeof(char) * 8);

    strcpy(&ptr[4], av[1]);
    strcpy(&ptr2[4], av[2]);

    fgets(c, 68, fopen("/home/user/level8/.pass", "r"));


    puts("~~");

    return 0;
}
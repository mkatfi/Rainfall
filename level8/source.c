
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


char *service;
char *auth;

int main () {

    char            buff[128];

    while(true) {

        printf("%p, %p \n", service, auth);
        
        if (fgets(buff, 128, stdin) != 0)
            break;
        if (strncmp(buff, "auth ", 5) == 0) {
            auth = malloc(sizeof(char) * 4);
            auth[0] = 0;
            if ((unsigned int)strlen(&buff[5]) <= 32) {
                strcpy(auth, &buff[5]);
            }
        }
        if (strncmp(buff, "reset", 5) == 0) {
            free(auth);
        }
        if (strncmp(buff, "service", 6) == 0) {
            service = strdup(&buff[7]);
        }
        if (strncmp(buff, "login", 5) == 0) {
            if (auth[32] != 0) {
                system("/bin/sh");
            }
            else {
                fwrite("Password:\n", 10, 1, stdout);
            }
        }
    }

    return 0;
}
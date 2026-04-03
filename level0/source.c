#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main (int ac, char **av) {
    gid_t gid, uid;
    char *sh_ptr[2];
    int val;

    val = atoi(av[1]);
    if (val == 423) {
        sh_ptr[0] = strdup("/bin/sh");
        sh_ptr[1] = NULL;

        gid = getgid();
        uid = getuid();
        setresgid(gid, gid, gid);
        setresuid(uid, uid, uid);
        execv("/bin/sh", &sh_ptr[0]);
    }
    else  {
        fwrite("No !\n", 1, 5, stderr);
    }
    return 0;
}
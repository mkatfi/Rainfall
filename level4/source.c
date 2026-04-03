#include <stdio.h>
#include <stdlib.h>

int m = 0;   // global variable at address 0x8049810

void p(char *buf) {
    printf(buf);   // ← FORMAT STRING BUG: should be printf("%s", buf)
}

void n(void) {
    char buf[520];
    fgets(buf, 512, stdin);
    p(buf);

    if (m == 0x1025544) {          // 16930116 in decimal
        system("/bin/cat /home/user/level5/.pass");
    }
}

int main(void) {
    n();
    return 0;
}

void n() {
    // THIS is what we want to reach
    system("/bin/cat /home/user/level7/.pass");
}

void m() {
    // This is the default — prints "Nope"
    puts("Nope");
}

int main(int ac, char **av) {
    char *buf1;   // Box 1 — holds your input
    void *ptr;    // Box 2 — holds a function pointer

    buf1 = malloc(sizeof(char) * 64);  // reserve 64 bytes for Box 1
    ptr  = malloc(sizeof(char) * 4);   // reserve 4 bytes for Box 2

    ptr = &m;                  // sticky note says "call m()"
    strcpy(buf1, av[1]);       // copy your input into Box 1 — NO SIZE CHECK!

    ((void(*)())ptr)();        // call whatever the sticky note says
    return 0;
}
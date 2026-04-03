#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void p() {
    char buff[64];       // a box that holds 64 letters
    void *ret_addr;

    fflush(stdout);
    gets(buff);          // ⚠️ DANGEROUS: reads input with NO limit

    // Check: if return address starts with 0xb... → EXIT
    ret_addr = __builtin_return_address(0);
    if (((unsigned int)ret_addr & 0xb0000000) == 0xb0000000) {
        printf("%p\n", ret_addr);
        exit(1);         // blocks stack shellcode!
    }

    puts(buff);
    strdup(buff);        // copies your input to the HEAP
}

int main() {
    p();
    return 0;
}
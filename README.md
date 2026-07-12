# Rainfall

> **Rainfall** — Introductory binary exploitation & reverse engineering wargame.
> Predecessor to [Rainfall](./), focused on building solid fundamentals in memory corruption and Linux binary internals.

<p align="center">
  <img alt="difficulty" src="https://img.shields.io/badge/difficulty-medium-orange">
  <img alt="topic" src="https://img.shields.io/badge/topic-binary%20exploitation-blueviolet">
  <img alt="platform" src="https://img.shields.io/badge/platform-x86%2032--bit-informational">
  <img alt="status" src="https://img.shields.io/badge/status-completed-success">
</p>

---

## Table of Contents

- [About](#about)
- [Overview](#overview)
- [Objective](#objective)
- [Table of Content](#table-of-content)
- [Getting Started](#getting-started)
- [Reverse-Engineered Binaries](#reverse-engineered-binaries)
- [Levels](#levels)
- [Methodology](#methodology)
- [Resources](#resources)
- [GDB Cheat Sheet](#gdb-cheat-sheet)
- [Write-ups](#write-ups)
- [Disclaimer](#disclaimer)
- [Team](#team)


---

## About

**Rainfall** is a bootable ISO wargame built around a series of protected Linux binaries (`level0` → `level9`). Each level embeds a distinct vulnerability, and progressing requires:

1. Locating the flaw in the binary through static/dynamic analysis.
2. Building an exploit to trigger the intended (unintended) behavior.
3. Reading the next level's password from its environment, a file, or its output, granting access to the next user.

Rainfall is designed as a first deep dive into stack layout, calling conventions, and the mindset needed for exploit development — laying the groundwork for harder wargames such as Override.



## Overview

In the folder for each level you will find:

- **`flag`** — password for the next level
- **`README.md`** — how to find the password
- **`source.c`** — the reverse-engineered binary

See each level's subject for more details.



## Objective

For each level, the goal is to escalate from the current user to the next by exploiting a specific weakness in a provided SUID binary, ultimately recovering that level's password.

| N° | Level | Topic |
|:--:|-------|-------|
| 0 | `level0` | Basic logic flaw / environment inspection |
| 1 | `level1` | Stack-based buffer overflow (no protections) |
| 2 | `level2` | Buffer overflow with function pointer overwrite |
| 3 | `level3` | Format string vulnerability (read) |
| 4 | `level4` | Format string vulnerability (write) |
| 5 | `level5` | Return-into-libc / partial protections |
| 6 | `level6` | Shellcode injection with restricted characters |
| 7 | `level7` | Integer overflow / signedness bug |
| 8 | `level8` | Heap-based exploitation |
| 9 | `level9` | Combined techniques (final challenge) |

> Passwords/flags are intentionally **not reproduced** in this README — see [Write-ups](#write-ups) for detailed solutions per level.
> Exact topics may vary slightly depending on your ISO revision; update this table to match your actual binaries.


## Table of Content

- [Getting Started](#getting-started)
  - [Virtual Machine Setup](#virtual-machine-setup)
  - [SSH Connect](#ssh-connect)
  - [Level Up](#level-up)
- [Reverse-Engineered Binaries](#reverse-engineered-binaries)
- [Levels Overview](#levels-overview)
- [Team](#team)

---

## Getting Started

First, download the ISO: **[RainFall.iso](https://cdn.intra.42.fr/isos/RainFall.iso)**

### Virtual Machine Setup

On macOS, install [VirtualBox](https://www.virtualbox.org/).

1. In VirtualBox, create a new VM (**New**).
2. **Name and operating system** — Type: `Linux`, Version: `Oracle (64-bit)`.
3. Continue through the next steps with the default settings:
   - **Memory size:** 4 MB
   - **Hard disk:** Create a disk now
   - **Hard disk file type:** VDI (VirtualBox Disk Image)
   - **Storage on physical hard disk:** Dynamically allocated
   - **File size:** 12.00 GB
4. Click **Settings → Network → Adapter 1 → Attached to:** `Bridged Adapter`.
5. Still in **Settings**, click **Storage** → next to "Controller: IDE" click the CD icon with a `+` (Add Optical Drive) → **Add Disk Image** → select `RainFall.iso`.
6. Click **Start**. Once running, the VM will display its IP address and prompt for login.

### SSH Connect

Log in from a separate shell as user `level0` with password `level0`:

```bash
ssh level0@{VM_IP} -p 4242
```

### Level Up

As user `level0`, the goal is to read the password for user `level1`, found at `/home/user/level1/.pass`. However, `level0` does not have permission to read this file.

In `level0`'s home folder is a binary named `level0` with the **SUID bit set**, owned by `level1`. This means that when we execute `level0`, it runs with the permissions of user `level1`.

We must find a vulnerability in the `level0` binary using GDB, then exploit it to run `system("/bin/sh")`, opening a shell as `level1` — where we have permission to read the password:

```bash
cat /home/user/level1/.pass
```

Then log in as `level1`:

```bash
su level1
```

Repeat for each subsequent level.

## Reverse-Engineered Binaries

For each level, the original `source.c` was reconstructed by examining the binary's GDB disassembly.

Each level's `README.md` details how to:

1. Compile `source.c`
2. Set the SUID bit
3. Run it with the exploit

...demonstrating the same vulnerability as the original binary.

## Levels

| Level | Binary | Vulnerability Class | Key Skill |
|:-----:|--------|----------------------|-----------|
| `level0` | logic/env | Trivial logic flaw, environment/file inspection | Basic recon |
| `level1` | overflow | Unbounded stack buffer overflow | Return address control |
| `level2` | overflow | Buffer overflow overwriting a function pointer | Redirecting execution flow |
| `level3` | fmt string | `%x`/`%s` based stack/memory read | Format string leaks |
| `level4` | fmt string | `%n` based arbitrary memory write | Format string writes |
| `level5` | libc | Bypassing NX via return-oriented techniques | Return-into-libc |
| `level6` | shellcode | Injecting shellcode under character restrictions | Shellcode encoding |
| `level7` | integer bug | Signed/unsigned comparison bypass | Integer overflow logic |
| `level8` | heap | Heap chunk metadata corruption | Heap exploitation basics |
| `level9` | combined | Chaining multiple primitives | Full exploit chain |

Each level's binary should be disassembled and traced under GDB before attempting exploitation — see the [Methodology](#methodology) section.

## Methodology

A consistent approach used throughout this project:

1. **Static analysis** — `file`, `checksec`, `objdump -d`, `strings` to identify protections (NX, ASLR, canaries, RELRO) and obvious logic.
2. **Dynamic analysis** — step through execution in GDB, inspect registers, stack, and memory layout at each breakpoint.
3. **Offset discovery** — use a cyclic pattern (see [Buffer Overflow Pattern Generator][offset-tool]) to find exact overflow offsets.
4. **Exploit development** — craft the payload (shellcode, format string, return address, etc.) in Python or raw bytes.
5. **Validation** — test locally, then against the target binary to confirm the password/shell is obtained.

[offset-tool]: https://wiremask.eu/tools/buffer-overflow-pattern-generator/

## Resources

| Subject | Link |
|:-------:|------|
| How the stack works | [hackndo — Stack Introduction](https://beta.hackndo.com/stack-introduction/) |
| Assembly basics | [hackndo — Assembly Basics](https://beta.hackndo.com/assembly-basics/) |
| Buffer offset generator | [Wiremask — BOF Pattern Generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/) |

## GDB Cheat Sheet

Quality-of-life GDB configuration used throughout this project — Intel syntax, unlimited output, and an automatic status dump (disassembly, frame info, registers, and stack) on every stop.

**`(gdb)` (persistent):**

```gdb
set disassembly-flavor intel
set height 0
info fun
```

## Write-ups

Detailed, level-by-level write-ups (vulnerability analysis and exploit code) live under [`/writeups`](./writeups):

```
writeups/
├── level0/
├── level1/
├── level2/
├── level3/
├── level4/
├── level5/
├── level6/
├── level7/
├── level8/
└── level9/
```

Each folder contains a `README.md` explaining the vulnerability, the exploit, and the steps to reproduce it.

## Disclaimer

This project was completed for **educational purposes** as part of a security curriculum. All techniques documented here target intentionally vulnerable binaries provided within the Rainfall ISO and are **not** intended for use against systems you do not own or have explicit authorization to test.


## Team

This project was written as a team with the awesome **[@onaciri](https://github.com/onaciri)**.

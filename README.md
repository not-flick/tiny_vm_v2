
<div align="center">

# TinyVM

### *A tiny virtual computer built from the ground up.*

*A modern C++ virtual machine designed for learning low-level systems by building an entire computer from scratch.*

![Status](https://img.shields.io/badge/status-active_development-orange)
![Language](https://img.shields.io/badge/C%2B%2B-20-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-success)
![License](https://img.shields.io/badge/license-Apache_2.0-green)

</div>

---

## Overview

TinyVM is an educational virtual machine that recreates an entire computer in software.

Instead of emulating x86, ARM, or RISC-V, TinyVM introduces its own architecture called **TVM_64** complete with its own CPU, memory system, instruction set, assembler, executable format, and operating environment.

The objective isn't simply to execute code.

It's to understand **how computers actually work**.

---

## Features

| | |
|:--|:--|
| 🖥️ | Custom **64-bit TVM_64** architecture |
| ⚡ | Virtual CPU |
| 💾 | Virtual RAM |
| 🧠 | Custom ISA |
| 🐚 | TinyShell |
| 🖥 | SDL3-based console |
| 🌍 | Cross-platform (Windows & Linux) |
| 📚 | Modular, educational codebase |

---

## Architecture

```
          +----------------+
          |    TinyShell   |
          +----------------+
                   │
                   ▼
          +----------------+
          |   TinyKernel   |
          +----------------+
                   │
                   ▼
          +----------------+
          |      TVM_64    |
          |  Virtual CPU   |
          +----------------+
             │         │
      +------+         +------+
      ▼                       ▼
+------------+        +----------------+
| Virtual RAM|        |  Syscall Layer |
+------------+        +----------------+
             │
             ▼
      Host Operating System
```

---

## Project Structure

```text
TinyVM/
│
├── Core/          # VM core
├── Console/       # SDL3 console
├── ISA/           # TVM_64 instruction set
├── Kernel/        # TinyKernel
├── Shell/         # TinyShell
├── Platform/      # Platform abstraction
├── Assembler/     # FLASM (planned)
└── ...
```

---

## Philosophy

TinyVM follows one simple idea:

> **Learn computers by building one.**

Every component is built from scratch.

- CPU
- Memory
- Instructions
- System Calls
- Executable Format
- Shell
- Operating Environment

No shortcuts.

No existing architecture.

Just a tiny computer built one layer at a time.

---

## Roadmap

### Core

- [x] Project architecture
- [x] SDL3 Console
- [x] Platform layer
- [x] Virtual memory
- [ ] CPU execution
- [ ] Instruction decoder

### Tools

- [ ] FLASM assembler
- [ ] Executable format
- [ ] Debugger
- [ ] Disassembler

### System

- [ ] TinyKernel
- [ ] TinyShell
- [ ] Virtual filesystem
- [ ] Networking

---

## Example

```asm
mov r1, 10
mov r2, 20

add r1, r2

syscall PRINT_INT, r1

halt
```

---

## Building

### Requirements

- C++20
- CMake
- SDL3
- SDL3_ttf

```bash
git clone https://github.com/not-flick/tiny_vm.git

cd tiny_vm

cmake -B build

cmake --build build
```

---

## Why TinyVM?

Most emulators teach you **how an existing CPU works.**

TinyVM teaches you **how a CPU is built.**

It is designed as a playground for experimenting with computer architecture, instruction sets, operating systems, compilers, and virtual hardware.

---

## Current Status

> 🚧 **Work in Progress**

The architecture is evolving rapidly as new components are implemented.

Expect frequent changes while TinyVM grows into a complete virtual computer platform.

---

## License

Licensed under the **Apache 2.0 License**.

---

<div align="center">

*"Every computer began as an idea."*

**TinyVM is no different.**

</div>

## Third-Party Assets

TinyVM bundles JetBrains Mono Nerd Font.

- JetBrains Mono © JetBrains
- Licensed under the SIL Open Font License 1.1

Nerd Font patches © Nerd Fonts Project
https://github.com/ryanoasis/nerd-fonts


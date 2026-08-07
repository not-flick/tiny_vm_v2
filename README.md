<div align="center">

# TinyVM

*A small virtual computer, built from first principles.*

[![License](https://img.shields.io/badge/license-Apache%202.0-green.svg)](#license)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![Rust](https://img.shields.io/badge/rust-stable-orange)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-success)
![Status](https://img.shields.io/badge/status-under%20development-orange)

</div>

---

## Overview

TinyVM is a software implementation of a complete computer.

Unlike a traditional emulator, TinyVM does **not** emulate an existing architecture
such as x86, ARM, or RISC-V. Instead, it implements its own machine architecture,
**TVM_64**, together with the kernel, shell, and console needed to run it.

The project exists for one purpose:

> **Build a computer by building every layer yourself.**

Everything above the host operating system — memory, kernel, process management,
shell, and terminal — is implemented as part of TinyVM.

For a deep technical walkthrough of every subsystem, see **[DOCS.md](DOCS.md)**.

---

## Design Goals

- Independent 64-bit architecture (no existing ISA reused)
- Simple and readable implementation
- Platform-independent codebase (Windows / Linux / macOS)
- Educational by design
- Modular components with narrow, well-documented interfaces
- Minimal external dependencies

---

## Components

| Component            | Status              |
|-----------------------|----------------------|
| Virtual RAM (Rust)     | ✔ Implemented        |
| Platform Layer         | ✔ Implemented        |
| SDL3 Console           | ✔ Implemented        |
| TinyShell               | ✔ Implemented        |
| TinyKernel (boot)       | 🚧 In Development     |
| Process Manager         | 🚧 In Development     |
| TVM_64 Architecture     | 🚧 In Development     |
| Virtual CPU             | 🚧 In Development     |
| Assembler (FLASM)       | Planned               |
| Executable Format       | Planned               |
| Virtual Filesystem      | Planned               |
| Networking               | Planned               |

---

## Architecture

```text
                     TinyShell
                         │
                         ▼
                    TinyKernel  ← boots RAM, console, pages
                         │
            ┌────────────┴────────────┐
            │                         │
            ▼                         ▼
      TVM_64 CPU                System Calls
     (planned)                  (planned)
            │
            ▼
   Virtual Memory (Rust crate, C ABI)
            │
            ▼
    Host Operating System
```

TinyKernel owns boot-up: it resolves how much RAM to give the VM, creates that
RAM through the Rust memory library, opens the SDL3-backed console, and will
eventually initialize the CPU, process manager, and syscall table before
handing off to TinyShell.

---

## Project Layout

```text
tiny_vm/
│
├── Src/
│   ├── TinyKernel/       # Boot sequence, process manager, paging
│   │   └── memory/       # Rust crate: VM RAM, exposed via C ABI
│   ├── TinyVM/           # Entry point: builds the virtual filesystem tree, launches TinyConsole
│   ├── console/          # SDL3-based terminal (rendering, fonts, boot logo)
│   ├── shell/            # TinyShell: parser, executor, history, completion
│   └── Platform/         # OS abstraction (paths, fonts, file I/O)
│
├── Assets/               # Fonts, boot logo, banner
├── CMakeLists.txt
├── README.md
└── DOCS.md
```

---

## Philosophy

TinyVM intentionally avoids implementing an existing ISA.

Instead, every subsystem is designed and implemented specifically for this project:

- Instruction Set Architecture
- CPU
- Memory Manager
- System Calls
- Kernel
- Shell
- Executable Format
- Toolchain

The objective is understanding the implementation of a computer — not merely using one.

---

## Example (target syntax)

The TVM_64 instruction set and assembler are still in development. The intended
assembly syntax looks like this:

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

- C++20 compiler and C17 compiler
- CMake ≥ 3.20
- Rust toolchain (Cargo)
- SDL3, SDL3_ttf, SDL3_image

```bash
git clone https://github.com/not-flick/tiny_vm.git
cd tiny_vm

cmake -B build
cmake --build build
```

This builds three targets:

- `TinyKernel` — static library, boots virtual RAM and the console
- `tiny_shell` — static library, TinyShell command interpreter
- `TinyConsole` — the SDL3 terminal application (run this to use TinyVM)
- `TinyVM` — sets up the on-disk virtual filesystem tree and launches `TinyConsole`

Run it with:

```bash
./build/TinyConsole
```

See **[DOCS.md](DOCS.md)** for the full build pipeline, including how the Rust
memory crate is compiled and linked automatically by CMake.

---

## Roadmap

### Core

- [x] Project structure
- [x] SDL3 console
- [x] Platform abstraction
- [x] Virtual memory backend (Rust)
- [x] TinyShell command interpreter
- [ ] CPU execution engine
- [ ] Instruction decoder
- [ ] Exception handling

### System

- [ ] TinyKernel CPU/syscall boot
- [ ] Process scheduler
- [ ] Virtual filesystem
- [ ] Networking

### Toolchain

- [ ] FLASM assembler
- [ ] Linker
- [ ] Executable format
- [ ] Debugger
- [ ] Disassembler

---

## Project Status

TinyVM is under active development. Internal interfaces are expected to change
as the architecture evolves. Compatibility between revisions is **not**
guaranteed until a stable release is published.

---

## Contributing

At this stage the project is primarily experimental. Bug reports, discussions,
and design suggestions are always welcome.

---

## License

TinyVM is licensed under the **Apache License 2.0**. See the `LICENSE` file for details.

---

## Third-Party Assets

### JetBrains Mono Nerd Font

TinyVM bundles **JetBrains Mono Nerd Font**.

- JetBrains Mono © JetBrains
- Nerd Font patches © Nerd Fonts Project
- Licensed under the SIL Open Font License 1.1

Repositories:

- https://github.com/JetBrains/JetBrainsMono
- https://github.com/ryanoasis/nerd-fonts

### Calligraphy

TinyVM's startup banner is generated using **Calligraphy**, an open-source ASCII
banner generator developed by **Evangelos "GeopJr" Paterakis**.

Repository:

- https://github.com/GeopJr/Calligraphy

<div align="center">

*"Programs execute instructions.*

*Systems execute ideas."*

</div>

<div align="center">

# TinyVM

*A small virtual computer built from first principles.*

[![License](https://img.shields.io/badge/license-Apache%202.0-green.svg)](#license)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-success)
![Status](https://img.shields.io/badge/status-under%20development-orange)

</div>

---

## Overview

TinyVM is a software implementation of a complete computer.

Unlike a traditional emulator, TinyVM does **not** emulate an existing architecture such as x86, ARM, or RISC-V. Instead, it implements its own machine architecture, **TVM_64**, together with its execution environment.

The project exists for one purpose:

> **Build a computer by building every layer yourself.**

Everything above the host operating system is implemented as part of TinyVM.

---

## Design Goals

- Independent 64-bit architecture
- Simple and readable implementation
- Platform-independent codebase
- Educational by design
- Modular components
- Minimal external dependencies

---

## Components

| Component | Status |
|-----------|--------|
| TVM_64 Architecture | 🚧 In Development |
| Virtual CPU | 🚧 In Development |
| Virtual Memory | 🚧 In Development |
| TinyKernel | 🚧 In Development |
| TinyShell | 🚧 In Development |
| SDL3 Console | ✔ Implemented |
| Platform Layer | ✔ Implemented |
| Assembler (FLASM) | Planned |
| Executable Format | Planned |
| Virtual Filesystem | Planned |
| Networking | Planned |

---

## Architecture

```text
                 TinyShell
                     │
                     ▼
                TinyKernel
                     │
        ┌────────────┴────────────┐
        │                         │
        ▼                         ▼
   TVM_64 CPU               System Calls
        │
        ▼
  Virtual Memory
        │
        ▼
 Host Operating System
```

---

## Project Layout

```text
TinyVM/
│
├── Src/
│   ├── TinyKernel/
│   ├── TinyShell/
│   ├── TinyConsole/
│   ├── TinyVM/
│   └── ...
│
├── Assets/
├── CMakeLists.txt
└── README.md
```

---

## Philosophy

TinyVM intentionally avoids implementing an existing ISA.

Instead, every subsystem is designed and implemented specifically for this project.

That includes:

- Instruction Set Architecture
- CPU
- Memory Manager
- System Calls
- Kernel
- Shell
- Executable Format
- Toolchain

The objective is understanding the implementation of a computer—not merely using one.

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
- Rust (Cargo)
- SDL3
- SDL3_ttf

```bash
git clone https://github.com/not-flick/tiny_vm.git

cd tiny_vm

cmake -B build
cmake --build build
```

---

## Roadmap

### Core

- [x] Project structure
- [x] SDL3 console
- [x] Platform abstraction
- [x] Virtual memory backend
- [ ] CPU execution engine
- [ ] Instruction decoder
- [ ] Exception handling

### System

- [ ] TinyKernel
- [ ] TinyShell
- [ ] Process management
- [ ] Virtual filesystem
- [ ] Scheduler
- [ ] Networking

### Toolchain

- [ ] FLASM assembler
- [ ] Linker
- [ ] Executable format
- [ ] Debugger
- [ ] Disassembler

---

## Project Status

TinyVM is under active development.

Internal interfaces are expected to change as the architecture evolves.

Compatibility between revisions is **not** guaranteed until a stable release is published.

---

## Contributing

At this stage the project is primarily experimental.

Bug reports, discussions, and design suggestions are always welcome.

---

## License

TinyVM is licensed under the **Apache License 2.0**.

See the `LICENSE` file for details.

---

# Third-Party Assets

### JetBrains Mono Nerd Font

TinyVM bundles **JetBrains Mono Nerd Font**.

- JetBrains Mono © JetBrains
- Nerd Font patches © Nerd Fonts Project
- Licensed under the SIL Open Font License 1.1

Repositories:

- https://github.com/JetBrains/JetBrainsMono
- https://github.com/ryanoasis/nerd-fonts

---

### Calligraphy

TinyVM's startup banner is generated using **Calligraphy**, an open-source ASCII banner generator developed by **Evangelos "GeopJr" Paterakis**.

The project is used to generate the boot banner displayed by TinyVM.

Repository:

- https://github.com/GeopJr/Calligraphy

<div align="center">

*"Programs execute instructions.*

*Systems execute ideas."*

</div>
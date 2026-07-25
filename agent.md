# memory.md

> This file is maintained by the AI.
> It is the single source of truth for project state.
> Update it after every meaningful coding session.

---

# TinyVM Overview

Project: TinyVM

TinyVM is a custom virtual machine written in modern C++.

Goals:
- Educational
- Modular
- Cross-platform
- Readable
- No unnecessary abstractions

---

# Current Progress

## Completed
- SDL3 Console
- UTF-8 support
- ANSI color support
- Virtual RAM
- Platform abstraction

## In Progress
- ISA implementation

## Next Tasks

- [ ]
- [ ]
- [ ]

---

# Architecture Decisions

Document important decisions here.

Example:

- Registers are 64-bit.
- Little endian.
- VM owns all RAM.
- Instructions are fixed width.

Never remove old decisions.
Instead mark them as superseded.

---

# Current File Structure

Update when new modules are added.

Core/
Console/
CPU/
Memory/
Platform/
Kernel/
Shell/

---

# Current Public API

List important classes and methods.

Example

Console
- write()
- writeLine()
- clear()

Memory
- read()
- write()

CPU
- execute()

---

# Active TODO

Highest priority tasks.

1.
2.
3.

---

# Known Bugs

Keep this updated.

Example

- SDL text wrapping broken.
- Window resizing flickers.

---

# Future Ideas

Ideas that should not be forgotten.

- ELF loader
- TCP/IP stack
- Virtual filesystem
- Debugger
- JIT compiler

---

# Session Log

Newest entries at the top.

## YYYY-MM-DD

### Done

-

### Problems

-

### Next

-

---

# AI Instructions

Every coding session MUST:

1. Read this file first.

2. Update it before finishing.

3. Never delete historical information.

4. Keep architecture decisions consistent.

5. If assumptions are made, record them.

6. When implementing a feature:
   - mark completed tasks
   - add new TODOs
   - update APIs if changed
   - update file structure if changed

7. Never rewrite the entire file.
   Modify only the affected sections.

8. If conflicting information exists,
   newest confirmed information wins.

9. Keep entries concise.

10. This file should always represent the current state of the project.
# Phase 4 — Structs, Headers, and Project Structure

> Your permanent reference for Phase 4.
> Read this cold in six months and everything clicks back instantly.

---

## Table of Contents

1. [📖 Concepts Learned](#-concepts-learned)
2. [🟢 Analogies — Simple Layer](#-analogies--simple-layer)
3. [🟡 How It Works — Real Layer](#-how-it-works--real-layer)
4. [🔴 Deep Dives — Senior Layer](#-deep-dives--senior-layer)
5. [🖥️ How the Computer Did It](#️-how-the-computer-did-it)
6. [🏗️ What We Built](#️-what-we-built)
7. [🌉 The Four-Way Bridge](#-the-four-way-bridge)
8. [☠️ Danger Zone](#️-danger-zone)
9. [🐛 Troubleshooting Log](#-troubleshooting-log)
10. [💡 Interview Q&A](#-interview-qa)
11. [🔗 How It Connects](#-how-it-connects)
12. [📝 Quick Reference](#-quick-reference)

---

## 📖 Concepts Learned

### The Problem Headers Solve

When a C project grows beyond one file, every `.c` file that calls a function from another `.c` file needs to know that function's shape — its name, return type, and parameter types — before the compiler sees the call. Without that information, the compiler either guesses (wrong) or errors out. Header files carry that shared knowledge. They are the contract that makes multi-file C projects possible.

### `#include` Is a Text Paste

The C preprocessor runs before the compiler and does not understand C. When it sees `#include "device.h"`, it finds the file and copies every byte of it into your source file at that exact line. The compiler then sees one big file. It has no idea the content came from two separate files. This is the foundation of how C shares type definitions across files.

### What Goes in `.h` vs `.c`

Headers contain **declarations** — the shape of things. Source files contain **definitions** — the actual memory or machine code. Putting a function body in a `.h` file and including it in two `.c` files causes a linker error: "multiple definition." Knowing this boundary prevents the most common beginner multi-file build error.

### Include Guards

If two headers each include a third, the preprocessor would paste that third header twice, causing "struct X redefined" errors. Include guards (`#ifndef / #define / #endif`) wrap the entire header so the second paste is silently skipped. Every header you write needs one.

### The `const` Qualifier on Pointer Parameters

`const Device *dev` tells the compiler: "I promise not to modify what dev points to." The compiler enforces this — any attempt to write `dev->id` inside that function is a compile error. This is how C functions announce their intent and protect callers from surprise mutations.

### The `Device` Struct — Your First Domain Type

`typedef struct { int id; char name[64]; int online; } Device;` is the core data type of this phase. It groups related fields under one name, can be heap-allocated with `malloc`, and is passed around by pointer. The pattern — create, print, free — is the foundation of every domain type in this project.

### Struct Padding and Alignment

The CPU reads integers most efficiently when they sit at addresses divisible by their size. To satisfy this, the compiler inserts invisible **padding bytes** between struct fields. `sizeof(struct X)` always returns the padded size, not the logical size. This matters for binary I/O, network protocols, and debugging unexpected struct sizes.

### The Multi-File Build Pipeline

Each `.c` file compiles independently into a `.o` (object file) — machine code with blank spots where cross-file calls are. The linker combines all `.o` files and fills in the blanks. The header file participates via copy-paste at each compilation unit but is never compiled on its own.

---

## 🟢 Analogies — Simple Layer

### The Problem Headers Solve

Imagine you are a chef in a restaurant kitchen. You have a sous chef who makes sauces in a separate room. Before you can use a sauce, you need to know: what is it called? What temperature should it be served at? What color should it be? You read a shared recipe card. That card is the header file. The actual sauce recipe — how it is made — is the `.c` file. The card tells you the shape; the recipe does the work.

### `#include` Is a Photocopier

When the preprocessor hits `#include "device.h"`, it walks to the filing cabinet, photocopies every page of `device.h`, and staples those pages into your source file at that line. The compiler then sees one long document. It has no idea the pages came from different places.

### Include Guards

Imagine a bouncer with a clipboard at the door of a conference room. The first time the Device struct tries to enter, the bouncer lets it in and writes "Device struct: here." If it tries to enter a second time (because two headers both included `device.h`), the bouncer checks the clipboard, says "already in the room," and turns it away. No duplicate definitions.

### `const` on a Pointer Parameter

Imagine you hand someone a glass display case containing your prized watch. You are saying: "You can look at it, measure it, describe it — but you may not open the case." `const Device *dev` is that display case. The function can read every field. Writing to any field is a compile error.

### The Struct as a Blueprint

A struct definition in C is like an architect's floor plan. The plan describes rooms, their sizes, and how they connect — but it is not a house. `Device *dev = malloc(sizeof(Device))` is like hiring a construction crew: they build the actual house in memory according to the plan. `device_free(dev)` is demolishing it when you are done.

### Struct Padding

Imagine a library where every book slot on the shelf is exactly the same width. A tiny pamphlet still gets a whole slot — the empty space next to it is wasted. The CPU is that library. Every `int` (4 bytes) needs its slot to start at an address divisible by 4. If the previous field ended at an odd address, the compiler adds empty "padding bytes" to push the next field to the right alignment.

### The Multi-File Build

Building a C program is like building a car from parts made in separate factories. Each factory (each `.c` file) builds its part independently and ships it to the assembly plant. The parts have connector ports (the call sites for functions from other files) that are left open. The assembly plant (the linker) connects all the ports together and ships you a finished car.

---

## 🟡 How It Works — Real Layer

### `.h` vs `.c` — The Exact Rules

```
GOES IN .h:
  - struct and typedef definitions
  - function prototypes (declaration only — no body)
  - #define constants
  - extern variable declarations (rare)

GOES IN .c:
  - function bodies (the implementation)
  - static variables (local to that file)
  - global variable definitions (rare, avoid)
```

Violation: if you put `void device_free(Device *dev) { free(dev); }` in `device.h` and
two `.c` files include it, the linker sees two copies of `device_free` and throws:

```
multiple definition of `device_free'; build/main.o: first defined here
```

### Include Guard Mechanics

```c
#ifndef DEVICE_H     // "if DEVICE_H is not yet #defined..."
#define DEVICE_H     // "...define it now (no value needed)"

// ... contents of the header ...

#endif               // end of the conditional block
```

The first time a translation unit includes `device.h`:
- `DEVICE_H` is not defined → condition is true → entire block is processed
- `DEVICE_H` is now defined

If the same translation unit includes `device.h` again (directly or via another header):
- `DEVICE_H` is already defined → condition is false → entire block is skipped

### `const` — What the Compiler Enforces

```c
// In device.h:
void device_print(const Device *dev);

// In device.c — compiler will reject any of these:
void device_print(const Device *dev) {
    dev->id = 99;          // ERROR: assignment of member 'id' in read-only object
    dev->online = 1;       // ERROR: same
    dev->name[0] = 'X';   // ERROR: same
}
```

`const Device *dev` — the Device the pointer points to is const. The pointer itself can be reassigned.
`Device * const dev` — the pointer itself is const (cannot be reassigned). What it points to can be changed.
`const Device * const dev` — both are const. This is the strongest guarantee.

For function parameters, the first form (`const Device *`) is what you always want.

### The `Device` Struct in Memory

When `device_create` returns, the heap contains:

```
Heap (returned by malloc):
┌──────────────┬──────────────────────────────────────────────┬──────────────┐
│  id (int)    │  name (char[64])                             │  online(int) │
│  4 bytes     │  64 bytes                                    │  4 bytes     │
│  offset 0    │  offset 4                                    │  offset 68   │
└──────────────┴──────────────────────────────────────────────┴──────────────┘
Total: 72 bytes. sizeof(Device) == 72.
```

No padding here because `char` has alignment 1 — it fits anywhere.
The `int` fields sit at offsets 0 and 68, both divisible by 4. Clean.

### Struct Padding — When It Appears

```c
struct Bad {
    char  a;    // 1 byte at offset 0
                // 3 bytes padding (to align b at offset 4)
    int   b;    // 4 bytes at offset 4
    char  c;    // 1 byte at offset 8
                // 3 bytes padding (to round struct size to multiple of 4)
};
// sizeof(struct Bad) == 12, not 6
```

Reordering fields eliminates the padding:

```c
struct Good {
    int   b;    // 4 bytes at offset 0
    char  a;    // 1 byte at offset 4
    char  c;    // 1 byte at offset 5
                // 2 bytes padding (to round to multiple of 4)
};
// sizeof(struct Good) == 8, not 12
```

Rule: put largest fields first, smallest last. Only matters when you have millions of structs in memory or when doing binary I/O.

### The Multi-File Build — Step by Step

```
Step 1: Preprocessor expands each .c file independently

  src/main.c + include/device.h  →  expanded main.c (one big file)
  src/device.c + include/device.h →  expanded device.c (one big file)

Step 2: Compiler turns each expanded file into an object file

  expanded main.c   →  build/main.o   (has a blank: "call ???device_create")
  expanded device.c →  build/device.o (has the real code for device_create)

Step 3: Linker combines the object files

  build/main.o + build/device.o  →  build/sentinel-c
  "call ???device_create" gets filled in with the address of device_create
```

### The Makefile After Phase 4

```makefile
SRCS = src/main.c src/device.c    # ← device.c added
CFLAGS = ... -I include            # ← tells compiler where to find device.h
```

`-I include` is a search path flag. Without it, `#include "device.h"` would only look in the same directory as the `.c` file. With it, the compiler also searches `include/`.

---

## 🔴 Deep Dives — Senior Layer

### Why C Has No Module System

C was designed in 1972 for the PDP-11. Disk I/O was slow, RAM was kilobytes, and "separate compilation" (building one file at a time) was a performance necessity, not a design choice. The header/source split is the consequence: headers are the manual coordination layer that newer languages automate. Zig's `@import` and Rust's `mod` system both do at compile time what C leaves to you by hand. The cost is verbosity. The benefit is that you see exactly what is shared, what is private, and there is no magic.

### Forward Declarations vs Full Includes

```c
// Option A: full include (what we use)
#include "device.h"    // pastes the entire Device definition

// Option B: forward declaration (for pointer-only usage)
typedef struct Device Device;   // "trust me, Device exists somewhere"
void device_free(Device *dev);  // only works because we use Device* (pointer)
```

Forward declarations break circular include chains (`a.h` includes `b.h` includes `a.h`). They are also used to minimize include depth in large codebases — including only what you need reduces compile times. For now, full includes are correct. This matters in Phase 10+.

### `__attribute__((packed))` — Why Not to Use It

```c
struct __attribute__((packed)) Packed {
    char a;
    int  b;
    char c;
};
// sizeof == 6 — no padding
```

Removes all padding. Looks attractive for binary protocols. The problem: accessing `b` now requires reading 4 bytes that are not 4-byte aligned. On x86 this works but is slower (the CPU does two reads and combines). On ARM or RISC-V it can be **undefined behavior** — the CPU may fault or silently read the wrong bytes. Use explicit serialization (field-by-field `fwrite`) instead of packed structs for any data that crosses machines.

### `opaque pointer` — Hiding the Struct Entirely

Large C projects (GTK, OpenSSL) go further than `const`: they hide the struct definition from the header entirely.

```c
// device.h — public API
typedef struct Device Device;   // forward declaration only — no fields visible
Device *device_create(int id, const char *name);
void    device_free(Device *dev);

// device.c — internal
struct Device { int id; char name[64]; int online; };  // only here
```

Callers can only use `Device *` — they cannot access `dev->id` directly because they do not know the struct layout. This is C's version of OOP encapsulation. We will likely use this pattern in Phase 10 when the panels become complex.

---

## 🖥️ How the Computer Did It

When you ran `make run` after Phase 4, here is what physically happened:

```
1. MAKE reads the Makefile
   → SRCS = src/main.c src/device.c
   → OBJS = build/main.o build/device.o
   → checks timestamps: are .o files newer than .c files?
   → if no: recompile

2. COMPILE src/device.c → build/device.o
   Preprocessor:
     sees #include "device.h" → opens include/device.h → pastes it in
     result: one big text file with the Device typedef and the function bodies
   Compiler:
     sees device_create, device_print, device_free function bodies
     generates x86-64 machine code for each
   Assembler:
     produces build/device.o — a binary with three functions ready to link

3. COMPILE src/main.c → build/main.o
   Preprocessor:
     sees #include "device.h" → pastes it in again (same header, two translation units)
     sees #include <stdio.h> → pastes in stdio declarations
   Compiler:
     sees main() calls device_create, device_print, device_free
     these are NOT in main.c — leaves blank slots in the object file
     ("external reference: device_create at address ???")
   Assembler:
     produces build/main.o — binary with blank slots for the Device functions

4. LINK build/main.o + build/device.o → build/sentinel-c
   Linker reads build/main.o:
     "needs device_create — address unknown"
     "needs device_print — address unknown"
     "needs device_free — address unknown"
   Linker reads build/device.o:
     "provides device_create at offset 0x1234"
     "provides device_print at offset 0x1290"
     "provides device_free at offset 0x12f0"
   Linker fills in the blanks in main.o with the real addresses
   Writes build/sentinel-c — one complete binary

5. RUN build/sentinel-c
   OS loads the binary into virtual memory
   Calls main()
   main() calls device_create → malloc → heap allocates 72 bytes
   device_print → printf → write() syscall → terminal shows output
   device_free → free() → heap releases the 72 bytes
   main returns 0 → OS exit
```

### Memory Layout When `device_print` Ran

```
HIGH ADDRESS
┌────────────────────────────────────┐
│  Stack — main's frame              │
│  ┌──────────────────────────────┐  │
│  │ Device *dev = 0x55a3bc2a0    │  │  ← 8-byte pointer on main's stack
│  └──────────────────────────────┘  │
├────────────────────────────────────┤
│  Stack — device_print's frame      │
│  ┌──────────────────────────────┐  │
│  │ const Device *dev (copy)     │  │  ← same address, passed by value
│  └──────────────────────────────┘  │
├────────────────────────────────────┤
│         ↓                          │
│   (stack grows down)               │
│                                    │
│   (heap grows up)                  │
│         ↑                          │
├────────────────────────────────────┤
│  Heap — Device struct              │
│  ┌──────────────────────────────┐  │
│  │ id      = 1          4 bytes │  │  ← offset 0
│  │ name[0] = 's'               │  │  ← offset 4
│  │ name[1] = 'e'               │  │
│  │ ...                         │  │
│  │ name[8] = '\0'              │  │  ← null terminator
│  │ name[9..63] = 0 (unused)    │  │
│  │ online  = 1 or 0    4 bytes │  │  ← offset 68
│  └──────────────────────────────┘  │
│  Total: 72 bytes                   │
├────────────────────────────────────┤
│  Data segment (read-only)          │
│  "Device %d: %-20s [%s]\n"        │  ← format string literal in printf
│  "online"  "offline"               │  ← string literals
├────────────────────────────────────┤
│  Text segment (code)               │
│  machine code: main()              │
│  machine code: device_create()     │
│  machine code: device_print()      │
│  machine code: device_free()       │
└────────────────────────────────────┘
LOW ADDRESS
```

`dev` in `main()` and `dev` in `device_print()` are two separate pointer variables on the stack, but they both hold the same address — pointing at the same 72-byte block on the heap. Modifying `dev->online = 1` in `main()` writes to the heap directly. `device_print` then reads the same heap memory.

---

## 🏗️ What We Built

### Files Created

```
CLookout/
├── include/
│   └── device.h        ← Device struct + function prototypes (NEW this phase)
├── src/
│   ├── main.c          ← updated: creates and prints a Device
│   ├── device.c        ← NEW: device_create, device_print, device_free
│   ├── memory.c        ← Phase 2 demo (not in current build)
│   └── strings.c       ← Phase 3 demo (not in current build)
├── build/
│   ├── main.o          ← compiled output
│   ├── device.o        ← compiled output (NEW)
│   └── sentinel-c      ← final binary
└── Makefile            ← updated: SRCS includes device.c, CFLAGS has -I include
```

### `include/device.h`

```c
#ifndef DEVICE_H
#define DEVICE_H

typedef struct {
    int  id;
    char name[64];
    int  online;
} Device;

Device *device_create(int id, const char *name);
void    device_print(const Device *dev);
void    device_free(Device *dev);

#endif
```

Every decision explained:
- Include guard: prevents double-definition if this header is included by multiple files
- `typedef struct { ... } Device;` — anonymous struct + typedef in one step, so callers use `Device` not `struct Device`
- `char name[64]` — fixed-size array embedded in the struct; no separate malloc, no pointer lifetime issues
- `const Device *dev` on device_print — declares read-only intent; compiler enforces it
- No function bodies in the header — those go in `device.c`

### `src/device.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.h"

Device *device_create(int id, const char *name) {
    Device *dev = malloc(sizeof(Device));
    if (dev == NULL) return NULL;
    dev->id     = id;
    dev->online = 0;
    snprintf(dev->name, sizeof(dev->name), "%s", name);
    return dev;
}

void device_print(const Device *dev) {
    printf("Device %d: %-20s [%s]\n",
           dev->id, dev->name,
           dev->online ? "online" : "offline");
}

void device_free(Device *dev) {
    free(dev);
}
```

Every decision explained:
- `malloc(sizeof(Device))` — allocates exactly the right number of bytes on the heap
- `if (dev == NULL) return NULL` — malloc can fail; caller checks the return value
- `snprintf(dev->name, sizeof(dev->name), "%s", name)` — bounded copy; can never overflow the 64-byte buffer
- `dev->online = 0` — explicit initialization; malloc does not zero memory
- `%-20s` in printf — left-align in a 20-character field for clean column output
- `dev->online ? "online" : "offline"` — ternary; clean conditional string selection

### `src/main.c`

```c
#include <stdio.h>
#include "device.h"

int main(void) {
    Device *dev = device_create(1, "server-01");
    if (dev == NULL) return 1;

    device_print(dev);
    dev->online = 1;
    device_print(dev);

    device_free(dev);
    dev = NULL;

    return 0;
}
```

Every decision explained:
- `if (dev == NULL) return 1` — always check malloc-backed create functions
- `dev->online = 1` — direct field write; the struct is heap-allocated, we own it
- `device_free(dev); dev = NULL;` — free the memory, then null the pointer to prevent use-after-free

### Working Output

```
$ make run
Device 1: server-01              [offline]
Device 1: server-01              [online]
```

---

## 🌉 The Four-Way Bridge

### Struct Definitions

```csharp
// C# — Sentinel
public class Device {
    public int Id { get; set; }
    public string Name { get; set; }
    public bool Online { get; set; }
}
// Reference type. GC manages lifetime. No manual free.
```

```zig
// Zig — Lookout
const Device = struct {
    id: i32,
    name: [64]u8,
    online: bool,
};
// Value type. Explicit allocator for heap. Defer for cleanup.
```

```odin
// Odin — Watchtower
Device :: struct {
    id:     int,
    name:   [64]byte,
    online: bool,
}
// Value type. Explicit allocator. defer delete for heap.
```

```c
// C — Sentinel-C
typedef struct {
    int  id;
    char name[64];
    int  online;
} Device;
// Value type. Manual malloc/free. No safety net.
// Everything above was secretly doing this — just with wrappers around it.
```

### Create / Destroy Pattern

| | C# | Zig | Odin | C |
|--|--|--|--|--|
| Create (heap) | `new Device()` | `allocator.create(Device)` | `new(Device, allocator)` | `malloc(sizeof(Device))` |
| Destroy | GC handles it | `allocator.destroy(dev)` | `free(dev, allocator)` | `free(dev)` |
| Null check | NullReferenceException | compile-time `!` error type | compiler warns | `if (dev == NULL)` manually |
| Field access | `dev.Online = true` | `dev.online = true` | `dev.online = true` | `dev->online = 1` |

`->` vs `.`: In C, if `dev` is a pointer, use `->`. If `dev` is a value (not a pointer), use `.`. Zig and Odin auto-dereference so `.` works for both. C does not.

### Include / Import

| | C# | Zig | Odin | C |
|--|--|--|--|--|
| Statement | `using System;` | `const dev = @import("device.zig");` | `import "device"` | `#include "device.h"` |
| Mechanism | Compiler resolves namespace | Compiler resolves file, returns struct | Compiler resolves package | Preprocessor text paste |
| Type-safe? | Yes | Yes | Yes | No — paste happens before compilation |
| Circular safe? | Yes (compiler handles) | Yes (lazy evaluation) | Yes | No — circular includes crash the preprocessor |

### Encapsulation

| | C# | Zig | Odin | C |
|--|--|--|--|--|
| Private field | `private int _id;` | `_id: i32` (convention) | `_id: int` (convention) | No language support; use opaque pointer |
| Interface enforcement | `private` keyword | comptime-based | package visibility | `const` on pointer parameters |
| Header as API surface | N/A | N/A | N/A | Everything in `.h` is the public API |

---

## ☠️ Danger Zone

### Function Body in a Header File

```c
// WRONG — in device.h
void device_free(Device *dev) {
    free(dev);    // body in the header
}

// If main.c and device.c both include device.h:
// → linker error: multiple definition of `device_free'
```

Headers contain declarations (the shape). Source files contain definitions (the body). Always.

---

### Missing Include Guard

```c
// device.h WITHOUT include guard:
typedef struct { int id; } Device;

// some_other.h:
#include "device.h"

// main.c:
#include "device.h"
#include "some_other.h"
// → preprocessor pastes device.h TWICE
// → error: redefinition of typedef 'Device'
```

Every header file you ever write needs `#ifndef / #define / #endif`. No exceptions.

---

### Forgetting `const` and Accidentally Mutating

```c
void device_log(Device *dev) {   // no const — allowed to write
    dev->online = 0;              // silent bug: caller's Device was just mutated
    printf("logged\n");
}

// The correct signature:
void device_log(const Device *dev) {   // compiler blocks accidental writes
```

Without `const`, the compiler has no way to warn you. The mutation is silent, the bug shows up elsewhere, the debugging is painful.

---

### Using `->` on a Non-Pointer

```c
Device dev_value = *dev;   // copies the Device off the heap onto the stack
dev_value->online = 1;     // ERROR: dev_value is not a pointer
dev_value.online  = 1;     // CORRECT: value type uses dot
```

`->` is shorthand for `(*ptr).field`. It dereferences and accesses in one step. Only use `->` with pointers. Use `.` with values.

---

### `sizeof` on a Pointer vs the Struct

```c
Device *dev = malloc(sizeof(Device));   // CORRECT: 72 bytes allocated

Device *dev = malloc(sizeof(dev));      // WRONG: allocates 8 bytes (pointer size on 64-bit)
// dev points to 8 bytes but Device needs 72 — every field past the 8th byte is a buffer overflow
```

`sizeof(dev)` is the size of the *pointer*, not what it points to. Always `sizeof(*dev)` or `sizeof(Device)`.

---

### Padding Mismatch in Binary I/O

```c
Device dev = { .id = 1, .online = 0 };
fwrite(&dev, sizeof(dev), 1, file);   // writes 72 bytes including potential padding

// On a different machine with a different compiler:
fread(&dev2, sizeof(dev2), 1, file);  // may read padding into wrong fields
```

Never use `fwrite` on a struct for persistent storage or network transmission. Serialize field-by-field. Struct layout is compiler and platform specific.

---

## 🐛 Troubleshooting Log

### Error: `undefined reference to device_create`

**When:** Compiling after adding `#include "device.h"` to `main.c` but not updating the Makefile.

**Full error:**
```
/usr/bin/ld: build/main.o: undefined reference to `device_create'
/usr/bin/ld: build/main.o: undefined reference to `device_print'
collect2: error: ld returned 1 exit status
```

**Cause:** `device.c` is not in `SRCS` in the Makefile. The linker never sees `device.o`.

**Fix:**
```makefile
SRCS = src/main.c src/device.c   # add device.c
```

**Lesson:** The linker only sees files listed in `SRCS`. Every new `.c` file must be added.

---

### Error: `redefinition of typedef 'Device'`

**When:** The struct appears in a `.c` file that is included by another `.c` file, or the header is missing its include guard.

**Full error:**
```
include/device.h:3:3: error: redefinition of typedef 'Device'
```

**Cause:** Either (a) the `#ifndef DEVICE_H` guard is missing, or (b) the struct definition was placed in a `.c` file and that `.c` file was included directly.

**Fix:** Move the struct definition to `device.h`, add the include guard, never `#include` a `.c` file.

---

### Error: `error: passing argument 1 discards 'const' qualifier`

**When:** Passing a `const Device *` to a function expecting `Device *`.

**Full error:**
```
main.c:10:18: error: passing argument 1 of 'some_func' discards 'const' qualifier
```

**Cause:** The function signature does not have `const` but you are passing something declared `const`.

**Fix:** Add `const` to the function's parameter, or (if the function genuinely must modify the struct) remove `const` from the caller.

---

### Error: `fatal error: device.h: No such file or directory`

**When:** The `#include "device.h"` line is in `device.c` but the Makefile does not have `-I include`.

**Full error:**
```
src/device.c:4:10: fatal error: device.h: No such file or directory
```

**Cause:** The preprocessor searches only the current directory by default. `include/device.h` is one directory level away.

**Fix:**
```makefile
CFLAGS = ... -I include   # tell the preprocessor to also search include/
```

---

### Warning: `-Werror=format-truncation`

**When:** Using `snprintf` where the compiler can prove the output will be truncated.

**Full error:**
```
src/device.c:12: error: '%s' directive output may be truncated writing up to 255 bytes
into a region of size 64 [-Werror=format-truncation]
```

**Cause:** The buffer (`name[64]`) is provably too small for the format string at compile time.

**Fix:** Either size the buffer correctly (at least as large as the largest input), or redesign the caller to not pass strings longer than 63 characters.

---

## 💡 Interview Q&A

**Q: Why does C have separate header and source files when other languages don't?**

C was designed for separate compilation — each `.c` file compiles independently into an object file, and the linker combines them. For `main.c` to call `device_create` defined in `device.c`, the compiler needs to know the function's signature before it sees the call. That knowledge lives in the header. Without headers, you would have to copy-paste declarations by hand into every file that uses them — and keep them in sync manually, which is error-prone. Newer languages (Zig, Rust, Go) handle this at the compiler level: the compiler reads the source files it needs and derives declarations automatically. C was designed before that was practical given 1970s hardware. The header/source split is C's manual version of what newer languages automate.

---

**Q: What is a typedef and why does C use it for structs?**

A `typedef` creates an alias for a type. In C, `struct Device` and `Device` are different things without a typedef — you would have to write `struct Device *dev` everywhere. `typedef struct { ... } Device;` makes `Device` a standalone type name. This is a syntactic convenience. Zig and Odin use `const Device = struct { ... }` for the same reason — the struct is named by assigning it to a constant. C# uses `class` and `struct` keywords which automatically give the type a name. The underlying concept — naming a composite type — is the same in all four.

---

**Q: What is struct padding and when does it matter?**

The CPU reads data most efficiently when values are aligned to their natural size — a 4-byte `int` at an address divisible by 4, an 8-byte `double` at an address divisible by 8. The compiler inserts invisible padding bytes between struct fields to satisfy this. `sizeof(struct X)` returns the padded size. For in-memory data structures, padding is invisible and harmless. It matters in two cases: (1) when binary data crosses machine boundaries — network packets, file formats — where two machines may pad differently, causing field misalignment; and (2) when memory is extremely tight and you have millions of instances — reordering fields (large to small) minimizes padding. The fix for binary I/O is always field-by-field serialization, never raw struct writes.

---

**Q: What does `const` on a function parameter actually do?**

`const Device *dev` tells the compiler that the function promises not to modify the Device that `dev` points to. The compiler enforces this: any attempt to write `dev->field = value` inside that function is a compile error. At the call site, `const` is also contagious — if you have a `const Device *`, you can only pass it to functions that also take `const Device *`. You cannot pass it to a function that takes `Device *` because that function might modify it. This is a lightweight correctness contract baked into the type system. It does not change the generated machine code — it is purely a compile-time safety check. Use it on every pointer parameter that the function does not need to modify.

---

**Q: What happens during linking, exactly?**

The linker takes all the `.o` object files and combines them into a final executable. Each `.o` file is a bag of machine code with two kinds of entries: **exports** (functions or variables this file provides) and **imports** (functions or variables this file calls but that live elsewhere). The linker builds a table of all exports across all `.o` files, then goes through every import and fills it in with the address of the matching export. If an import has no matching export, the linker throws "undefined reference." If an export name appears in two `.o` files, the linker throws "multiple definition." The result of successful linking is an ELF binary (on Linux) with all addresses resolved and code segments ready to be mapped into virtual memory by the OS loader.

---

## 🔗 How It Connects

**Phase 4 → Phase 5 (The Domain Layer)**

Phase 5 adds `Incident` and `Todo` structs alongside `Device`. Each gets its own `.h` and `.c` file, following the exact module pattern established here. Phase 4 is the template. Phase 5 is applying it three times over with more complex fields (timestamps, string pointers, linked lists).

**Phase 4 → Phase 6 (ncurses)**

ncurses gives you a `WINDOW *` — a pointer to an opaque struct, exactly like `Device *`. You call functions that take `WINDOW *` without knowing what is inside it. That is the opaque pointer pattern from the Deep Dives section above. Understanding `Device *` makes `WINDOW *` immediately familiar rather than mysterious.

**Phase 4 → Phase 9 (cJSON)**

cJSON uses the same pattern: `cJSON *` is a pointer to an opaque struct, `cJSON_Parse` returns one, `cJSON_Delete` frees it. The entire cJSON API is built on the module pattern you learned here.

**Phase 4 → Phase 16 (SQLite)**

SQLite's C API is `sqlite3 *db = NULL; sqlite3_open(..., &db);` — another opaque pointer, another create/use/free lifecycle. Every C library you will ever use follows this pattern because it is the only way C can approximate encapsulation.

**Phase 4 → All Phases**

The `include/` directory, the `-I include` flag, the per-domain `.h` + `.c` pair, the create/print/free lifecycle — these are the backbone of the entire Sentinel-C architecture. Every phase from here on adds modules following this exact structure.

---

## 📝 Quick Reference

### The Module Pattern (one per domain type)

```
include/thing.h    ← struct definition + function prototypes + include guard
src/thing.c        ← function bodies + #include "thing.h"
Makefile SRCS      ← add src/thing.c
```

### Header File Template

```c
#ifndef THING_H
#define THING_H

typedef struct {
    int  id;
    char name[64];
} Thing;

Thing *thing_create(int id, const char *name);
void   thing_print(const Thing *t);
void   thing_free(Thing *t);

#endif
```

### Source File Template

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thing.h"

Thing *thing_create(int id, const char *name) {
    Thing *t = malloc(sizeof(Thing));
    if (t == NULL) return NULL;
    t->id = id;
    snprintf(t->name, sizeof(t->name), "%s", name);
    return t;
}

void thing_print(const Thing *t) {
    printf("Thing %d: %s\n", t->id, t->name);
}

void thing_free(Thing *t) {
    free(t);
}
```

### Pointer Operator Reference

```c
Device *dev = device_create(1, "x");

dev->id       // dereference pointer, access field (use with pointer)
(*dev).id     // same thing, long form
dev->name     // the char[64] array inside the struct

Device copy = *dev;   // dereference: copy the whole struct onto the stack
copy.id               // dot: access field on a value (not a pointer)
```

### `const` Quick Reference

```c
const Device *dev       // pointer to const Device — cannot modify fields
Device * const dev      // const pointer — cannot change where dev points
const Device * const dev // both: cannot modify fields, cannot move pointer
```

### `sizeof` Safety

```c
sizeof(Device)      // correct: size of the struct type
sizeof(*dev)        // correct: size of what dev points to (same thing)
sizeof(dev)         // WRONG for allocation: size of the pointer (8 bytes on 64-bit)
```

### Struct Padding Rule of Thumb

```c
// Wasteful: char, int, char = 12 bytes (4 bytes padding wasted)
struct Bad  { char a; int b; char c; };

// Efficient: int, char, char = 8 bytes (2 bytes padding, saves 4)
struct Good { int b; char a; char c; };
```

### Makefile After Phase 4

```makefile
CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -fsanitize=undefined -I include
TARGET = build/sentinel-c
SRCS   = src/main.c src/device.c
OBJS   = $(patsubst src/%.c,build/%.o,$(SRCS))
```

### Commands

```bash
make          # build (incremental)
make run      # build and run
make clean    # wipe build/
```

---

*Phase 4 complete. The Device module is live. Headers, include guards, const, malloc, free — all working.*
*The module pattern is established. Phase 5 will apply it to Incident, Todo, and Chat.*

# Phase 2 — Pointers and Memory

> Your permanent reference for Phase 2.
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

### RAM — The Giant Array of Bytes

RAM is a flat sequence of bytes, each with a unique number called an address. Address 0 is the first byte. Address 1 is the second. On a 64-bit system, addresses go up to 2^64 − 1. Every variable, every function, every string in your program lives somewhere in this array at a specific address.

### Little-Endian Byte Order

When a multi-byte value like `int 42` is stored in RAM, its bytes do not go in the order you might expect. On x86 (the CPU in your machine), the least significant byte is stored first — at the lowest address. This is called little-endian. The integer 42 (hex `0x0000002A`) in memory looks like `[2A][00][00][00]`, not `[00][00][00][2A]`. The CPU reads them back in the correct order automatically.

### Hexadecimal — Why Memory Addresses Use Base 16

Memory addresses are printed in hex (`0x7ffd15065110`) because one hex digit represents exactly 4 bits, and two hex digits represent exactly 1 byte. Hex digits run `0–9` then `a–f`, where `f = 15`. After `f`, the digit rolls over and the next column carries — `0x10f + 1 = 0x110`, exactly one address apart. Decimal 42 = hex `0x2a` = binary `0010 1010`. To convert binary to hex, split into groups of 4 bits: `0010` = 2, `1010` = a → `0x2a`.

### Registers

Registers are storage locations inside the CPU chip itself — not RAM. Only ~16 of them on x86-64. Extremely fast: sub-nanosecond vs RAM's ~100ns. The CPU cannot do math directly on RAM — it loads values into registers, operates on them, then stores back. Key registers: `rsp` (stack pointer — always holds the address of the current top of the stack), `rip` (instruction pointer — address of the next instruction).

### What a Pointer Is

A pointer is a variable whose value is a memory address. That is the entire definition. `int *p` declares a variable `p` that holds an address. If `p` contains `0x7ffd15065110`, then `p` points to whatever is stored at that address in RAM. A pointer is itself stored in RAM, at its own address — pointers are just variables.

### The `&` Operator (Address-Of)

`&a` means "give me the address of variable `a`." If `a` lives at address `0x7ffd15065110`, then `&a` evaluates to the number `0x7ffd15065110`. Type: `int *` (pointer to int).

### The `*` Operator (Dereference)

`*p` means "go to the address stored in `p` and read what is there." Writing `*p = 100` goes to that address and stores `100` there — modifying the original variable without naming it directly. The `*` in a declaration (`int *p`) is part of the type syntax, not the dereference operator. These are two different uses of the same symbol.

### The `->` Operator

When you have a pointer to a struct, `ptr->field` is shorthand for `(*ptr).field`. It dereferences the pointer and accesses the named field in one step. You use `->` with pointers to structs; you use `.` with non-pointer struct variables.

### `void *` — The Generic Pointer

`void *` is C's universal pointer type. It means "a pointer to something — I'm not saying what." Any pointer type converts to `void *` implicitly and back. The `%p` format specifier for `printf` requires a `void *` by the C standard — that is why we cast `(void *)&a` rather than passing `&a` directly.

### The Stack — Automatic Memory

The stack is a region of memory the OS sets up for your program. Local variables declared inside functions live on the stack. When a function is called, the CPU carves out a block of stack space called a stack frame to hold that function's variables. When the function returns, the frame is discarded. You do nothing to manage this — it is automatic. Stack addresses are high numbers (`0x7ffd...` on x86-64 Linux). The stack grows downward — toward lower addresses — because of a hardware convention from the original Intel 8080 in the 1970s.

### The Heap — Dynamic Memory

The heap is a separate region of memory for allocations that need to outlive the function that created them. You request heap memory with `malloc`, use it, and release it with `free`. The OS does not clean it up for you — you are the garbage collector. Heap addresses are much lower than stack addresses (`0x55...` or `0x5c...` range on Linux).

### `malloc` and `free`

`malloc(n)` asks the OS for `n` bytes of heap memory. It returns a pointer to the first byte, or `NULL` if the allocation failed. `free(ptr)` returns that memory to the allocator. Every `malloc` must have exactly one `free`. No `free` = memory leak. Two `free`s on the same pointer = undefined behavior. Always check for `NULL` before using the pointer `malloc` returns.

### Use-After-Free

Accessing memory through a pointer after calling `free` on it is undefined behavior. The allocator often writes its own bookkeeping data into freed memory immediately — so the value you read is wrong, or the program will crash. What makes it dangerous: it frequently does neither. It silently returns garbage. On our machine, reading `dev->id` after `free(dev)` returned `0` because glibc overwrote those bytes with its internal free-list pointer.

### `sizeof` — Compile-Time Size

`sizeof(Type)` returns the number of bytes a type occupies. It is computed entirely at compile time — no runtime cost. Always use `malloc(sizeof(Type))` rather than hardcoding a number.

### `typedef struct`

A struct is a named group of variables that belong together. `typedef struct { ... } Name;` creates a new type called `Name`. A `Device` struct with `id` and `online` fields is exactly 8 bytes — two adjacent ints in memory, 4 bytes each.

---

## 🟢 Analogies — Simple Layer

### RAM

A giant hotel with billions of numbered rooms. Every room holds exactly one byte. When your program needs to store a variable, the OS assigns it one or more rooms. The room numbers are the addresses you see printed.

### Little-Endian

Imagine writing the number 42,000 on a receipt, but the cashier writes the ones digit first, then the tens, then the hundreds: `0`, `0`, `2`, `4`. That is little-endian — least significant part first. The CPU has been doing this since the original Intel 8080. It reads back the digits in the right order automatically, so you only notice when you peek at the raw bytes.

### Hexadecimal

Hex is a more efficient way to count bytes. Decimal gives you ten symbols before you need two digits. Hex gives you sixteen. Since a byte holds values 0–255 and `0xFF = 255`, two hex digits represent one byte perfectly. Not magic — just a compact notation that maps cleanly to binary.

### A Pointer

A sticky note with a room number written on it. The sticky note is not the thing — it just tells you where the thing is. `int *p = &a` writes `a`'s room number on the sticky note `p`. `*p` says "go to the room written on this sticky note and look inside."

### The Stack

The hotel's automatic valet system. When you call a function, the valet parks your variables in a reserved section of rooms. When the function returns, the valet clears that section immediately — no keys needed, no checkout process. Those rooms are immediately available for the next function call.

### The Heap

A self-storage facility. You call ahead (`malloc`), reserve a unit, and they hand you the key (a pointer). The unit stays rented until you return the key (`free`). If you lose the key (lose the pointer), the unit stays rented forever — a memory leak. If you return a key you already returned (double free), the desk clerk has a problem. You are the only one managing this.

### Use-After-Free

You return a key to the storage unit. The facility immediately rents it to someone else. You use the old key to enter the unit anyway. The new tenant may not have moved in yet, so it looks fine at first. But your stuff is mixed up with theirs. Eventually something gets corrupted — and neither of you knows why, or when.

---

## 🟡 How It Works — Real Layer

### Stack Memory Layout

When `memory_demo()` is called, the CPU decrements the stack pointer (`rsp`) by the size of the stack frame to carve out space for local variables. The compiler decided at compile time how large the frame is and where each variable sits (at what offset from `rsp`). The actual address is only known at runtime.

```
HIGH ADDRESS (stack top)
┌──────────────────────┐
│  return address      │  ← where to jump when memory_demo returns
│  saved registers     │
├──────────────────────┤
│  b = 99   (4 bytes)  │  ← 0x7ffe79dc855c
│  a = 42   (4 bytes)  │  ← 0x7ffe79dc8558
│  c = 'X'  (1 byte)   │  ← 0x7ffe79dc8557
└──────────────────────┘
LOW ADDRESS (within this frame)
```

What the addresses proved:
- `b - a = 0x855c - 0x8558 = 4 bytes` — int is 4 bytes ✓
- `a - c = 0x8558 - 0x8557 = 1 byte` — char is 1 byte ✓
- `p points to == Address of a` — a pointer IS an address, nothing more ✓
- `Device addr 0x5c...` vs stack `0x7ffd...` — completely different memory regions ✓

### How Pointer Operators Work

```c
int x = 42;        // x lives at address 0x7fff1000
int *p = &x;       // p = 0x7fff1000 (the address of x)
                   // p itself lives at, say, 0x7fff1008

int y = *p;        // follow p to 0x7fff1000, read 4 bytes → 42
*p = 99;           // follow p to 0x7fff1000, write 99 there
                   // x is now 99 — changed through the pointer
```

```
    p (at 0x7fff1008)          x (at 0x7fff1000)
    ┌────────────────┐         ┌──────────────┐
    │  0x7fff1000    │────────▶│      42      │
    └────────────────┘         └──────────────┘
    p holds the address         x holds the value
```

### Heap Memory Layout

`malloc(sizeof(Device))` calls into glibc's allocator. It finds a free block in its arena and returns a pointer to the first byte of usable memory. The heap grows upward — new allocations get higher addresses. Stack and heap are completely separate regions of the process's virtual address space.

```
HIGH ADDRESS (stack territory)
│  a, b, c on stack    │  0x7ffe...
│                      │
│     (large gap)      │
│                      │
│  Device on heap      │  0x5c9f...
LOW ADDRESS
```

### What `free()` Actually Does

`free(ptr)` does not zero the memory. It does not return it to the OS immediately. It gives the block back to glibc's allocator, which writes its own internal bookkeeping data (free-list pointers) into the first bytes of the freed block. This is why reading `dev->id` after `free(dev)` returned `0` on our machine — glibc's internal pointer happened to contain zero. The memory was still in the process's address space; it just no longer belonged to us.

### Why `NULL` After `free`

After `free(dev)`, `dev` still holds the old address. This is a dangling pointer. Setting `dev = NULL` immediately converts it: if code accidentally dereferences `dev` later, it hits address `0x0` (null), which is never mapped — the OS instantly raises a segfault. A loud, obvious crash at the bad line is better than silent corruption discovered weeks later.

### The `->` Operator Decoded

```c
Device *dev = malloc(sizeof(Device));
dev->id = 1;       // exact same as: (*dev).id = 1
dev->online = 1;   // exact same as: (*dev).online = 1
```

`dev` is a pointer — an address. To access `id`, you must dereference first (`*dev`), then access the field (`.id`). `->` does both steps in one operator. The compiler generates identical machine code either way.

---

## 🔴 Deep Dives — Senior Layer

### Why the Stack Grows Downward

A hardware convention from the original Intel 8080 of the 1970s. Program code loads from low addresses upward. By growing the stack from high addresses downward, both regions can grow toward each other from opposite ends of address space, with no need to predict how much of each you will need. The collision (stack overflow) only happens with deep recursion or very large local arrays.

### What `malloc` Does Under the Hood

glibc's `malloc` is a sophisticated memory allocator. For small allocations, it uses pre-allocated arenas divided into size-class bins (8 bytes, 16 bytes, 24 bytes, etc.). Your `malloc(8)` gets a slot from the 8-byte bin. For large allocations (over 128KB), it calls `mmap()` directly to request memory from the OS. The metadata written on `free` (which overwrote `dev->id`) is a doubly-linked list pointer — the allocator threads freed chunks together so it can find them for the next `malloc`.

### Use-After-Free as a CVE

Use-after-free is in the OWASP Top 10 and has caused critical CVEs in browsers, kernels, and network daemons. The attack pattern: the victim program frees an object but keeps a pointer. The attacker triggers an allocation of the same size in a different code path — it gets the same memory. The attacker controls the contents of that allocation. The victim's dangling pointer now points at attacker-controlled data. If the victim calls a function through a function pointer in the struct, the attacker has replaced it with their own address. Code execution. This is why security-critical C is written with strict ownership disciplines.

### `void *` and the C Type System

C's type system exists entirely at compile time. At runtime, memory is just bytes — no type tags, no metadata. `void *` is the escape from the type system: "I have a pointer but I'm not telling the compiler what it points to." Any pointer converts to `void *` and back without the compiler objecting. This is how `malloc` can return memory for any type: it returns `void *` and you assign it to the typed pointer variable. The power is real — so is the danger. If you cast `void *` to the wrong type, the compiler will not stop you.

### Stack vs Heap Tradeoffs

| | Stack | Heap |
|--|--|--|
| Allocation cost | Zero — just decrement rsp | glibc lookup + possible syscall |
| Deallocation cost | Zero — just increment rsp | glibc free-list update |
| Max size | ~8MB (ulimit) | Limited by RAM + swap |
| Lifetime | Function scope only | Until free() is called |
| Fragmentation | None | Yes — heap can fragment over time |

---

## 🖥️ How the Computer Did It

When `make run` compiled and ran `memory_demo()`:

```
1. COMPILER (compile time)
   → Counted local variables: int a, int b, char c, int *p, Device *dev
   → Computed stack frame size: ~48 bytes (with alignment padding)
   → Assigned each variable an offset from rsp
   → Compiled these offsets into the machine code — not addresses, just offsets

2. RUNTIME — calling memory_demo()
   → CPU decrements rsp by frame size
   → a now lives at the new rsp+offset → actual address 0x7ffe79dc8558
   → b at rsp+4 → actual address 0x7ffe79dc855c
   → c at rsp-1 → actual address 0x7ffe79dc8557
     (compiler chose this ordering — layout within frame is up to the compiler)

3. int *p = &a
   → The number 0x7ffe79dc8558 is stored in p
   → p itself lives at another offset on the stack

4. p points to: 0x7ffe79dc8558  (identical to Address of a — proof a pointer IS an address)
   Value at p:   42              (dereference: read 4 bytes at that address)
   a is now:     100             (*p = 100 wrote to that address — a changed without naming a)

5. malloc(sizeof(Device)) = malloc(8)
   → glibc finds a free 8-byte slot in its small-allocation arena
   → returns 0x5c9f00d512c0
   → dev (on the stack) stores this address

6. dev->id = 1   →   write 1 to address 0x5c9f00d512c0
   dev->online = 1 → write 1 to address 0x5c9f00d512c4 (4 bytes later)

7. free(dev)
   → glibc takes back the 8 bytes at 0x5c9f00d512c0
   → glibc writes its free-list pointer into those bytes
     (overwrites id=1 with bookkeeping data → reads as 0)
   → dev still holds 0x5c9f00d512c0 — now a dangling pointer

8. dev = NULL
   → dev now holds 0x0
   → any accidental dereference → immediate segfault
   → the dangling pointer danger is neutralized

9. memory_demo() returns
   → CPU increments rsp back to pre-call position
   → a, b, c, p, dev cease to exist — their stack bytes are free for the next call
```

---

## 🏗️ What We Built

### Files

```
CLookout/
├── src/
│   ├── main.c       ← calls memory_demo() via forward declaration
│   └── memory.c     ← all 5 parts of the memory demo
├── build/
│   ├── main.o
│   ├── memory.o
│   └── sentinel-c
└── Makefile         ← -fsanitize=address removed (WSL2 incompatibility)
```

### `src/main.c`

```c
#include <stdio.h>

void memory_demo(void);

int main(void) {
    memory_demo();
    return 0;
}
```

`void memory_demo(void);` is a forward declaration — it tells the compiler "this function exists somewhere; the linker will find it." This is how one `.c` file calls a function defined in another `.c` file without a header.

### `src/memory.c`

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    int online;
} Device;

void memory_demo(void) {
    /* Part 1: stack variables and addresses */
    int   a = 42;
    int   b = 99;
    char  c = 'X';

    printf("Value of a:   %d\n",  a);
    printf("Value of b:   %d\n",  b);
    printf("Value of c:   %c\n",  c);
    printf("\n");
    printf("Address of a: %p\n",  (void *)&a);
    printf("Address of b: %p\n",  (void *)&b);
    printf("Address of c: %p\n",  (void *)&c);

    /* Part 2: pointer declaration, dereference, modify through pointer */
    int *p = &a;
    printf("\np points to:  %p\n",  (void *)p);
    printf("Value at p:   %d\n",   *p);
    *p = 100;
    printf("a is now:     %d\n",   a);

    /* Part 3: heap allocation */
    Device *dev = malloc(sizeof(Device));
    if (dev == NULL) {
        printf("malloc failed!\n");
        return;
    }

    dev->id     = 1;
    dev->online = 1;

    printf("\nDevice id:     %d\n",  dev->id);
    printf("Device online: %d\n",   dev->online);
    printf("Device addr:   %p\n",   (void *)dev);

    free(dev);
    dev = NULL;
    printf("Device freed.\n");
}
```

### `Makefile` (current)

```makefile
CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -fsanitize=undefined
TARGET = build/sentinel-c
SRCS   = src/main.c src/memory.c
OBJS   = $(patsubst src/%.c,build/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)
```

Key change from Phase 1: `-fsanitize=address` removed. ASan conflicts with WSL 2's virtual address space layout. UBSan (`-fsanitize=undefined`) works correctly. Memory error checking returns in Phase 18 with valgrind.

### Actual Output

```
Value of a:   42
Value of b:   99
Value of c:   X

Address of a: 0x7ffe79dc8558
Address of b: 0x7ffe79dc855c
Address of c: 0x7ffe79dc8557

p points to:  0x7ffe79dc8558
Value at p:   42
a is now:     100

Device id:     1
Device online: 1
Device addr:   0x5c9f00d512c0
Device freed.
```

---

## 🌉 The Four-Way Bridge

### Pointers

```csharp
// C# — Sentinel
// You never see raw pointers. The GC owns all addresses.
int a = 42;
ref int p = ref a;   // C# 7.0+ — ref is a managed reference, not a raw pointer
p = 100;             // a is now 100 — same idea, but safe and GC-managed
```

```zig
// Zig — Lookout/Shell
var a: i32 = 42;
const p: *i32 = &a;   // *i32 = pointer to i32, nearly identical to C
p.* = 100;            // .* dereferences — Zig uses postfix, C uses prefix
// Compiler tracks validity. No dangling pointers at comptime.
```

```odin
// Odin — Watchtower
a := 42
p := &a        // inferred type: ^int  (^ is Odin's pointer sigil)
p^ = 100       // ^ on the right dereferences
```

```c
// C — Sentinel-C
int a = 42;
int *p = &a;   // * in declaration = "pointer to"
*p = 100;      // * in expression = "dereference"
// Raw address. No safety. Wrong p = silent memory corruption.
```

### Memory Allocation

```csharp
// C# — Sentinel
var dev = new Device { Id = 1, Online = true };
// GC decides when to free it. You never call free().
```

```zig
// Zig — Lookout/Shell
var dev = try allocator.create(Device);
defer allocator.destroy(dev);   // defer: guaranteed to run at end of scope
dev.id = 1;
// Allocator is explicit. defer replaces manual free — but it's guaranteed.
```

```odin
// Odin — Watchtower
dev := new(Device)      // context.allocator
defer free(dev)
dev.id = 1
```

```c
// C — Sentinel-C
Device *dev = malloc(sizeof(Device));   // you call the OS
if (dev == NULL) { return; }            // you check for failure
dev->id = 1;
free(dev);          // you free it
dev = NULL;         // you guard against dangling pointer
// Every language above is secretly doing this.
// They just do it for you, or give you guarantees you don't forget.
```

### Struct Member Access

```csharp
dev.Id = 1;          // always dot — C# references auto-deref
```

```zig
dev.id = 1;          // always dot — Zig pointer auto-deref
```

```odin
dev.id = 1;          // always dot — Odin pointer auto-deref
```

```c
dev->id = 1;         // pointer to struct: use ->
Device d;
d.id = 1;            // non-pointer struct: use .
// C is the only one that requires different syntax for pointer vs value
```

### What Each Language Hides from You

| What C exposes | C# hides it with | Zig hides it with | Odin hides it with |
|---|---|---|---|
| Raw memory addresses | GC-managed references | Compiler-tracked pointers | Context allocator |
| `malloc` / `free` | `new` + GC | `allocator.create` + `defer` | `new` + `defer` |
| Dangling pointer risk | GC prevents it | Compile-time lifetime rules | Runtime safety checks |
| `->` vs `.` | Always `.` | Always `.` | Always `.` |
| NULL check after malloc | OOM exception thrown | `try` propagates the error | panic on OOM |
| Little-endian layout | Completely hidden | Visible via `@ptrCast` | Visible via `transmute` |

---

## ☠️ Danger Zone

### Use-After-Free

```c
Device *dev = malloc(sizeof(Device));
dev->id = 1;
free(dev);

// WRONG — dev is now a dangling pointer
printf("%d\n", dev->id);   // undefined behavior
                            // on our machine: printed 0 (glibc overwrote the bytes)
                            // in other runs: could print garbage, crash, or appear correct
```

We deliberately triggered this in Part 4. The freed memory was not zeroed — glibc wrote its own bookkeeping pointer there, which happened to look like `0`. No crash. No warning. That silent corruption is exactly what makes use-after-free a CVE.

**The fix:**
```c
free(dev);
dev = NULL;   // dangling pointer becomes null pointer
              // accidental dereference → immediate segfault at the bad line
```

---

### Not Checking `malloc` Return Value

```c
Device *dev = malloc(sizeof(Device));
dev->id = 1;   // CRASH if malloc returned NULL
               // dereferencing NULL = immediate segfault
```

On modern Linux with plenty of RAM, `malloc` almost never returns `NULL`. But containers with memory limits, embedded systems, and servers under load do run out. Always check.

```c
Device *dev = malloc(sizeof(Device));
if (dev == NULL) {
    return;   // or log, or exit — but never continue
}
dev->id = 1;  // safe
```

---

### Double Free

```c
Device *dev = malloc(sizeof(Device));
free(dev);
free(dev);   // CRASH — corrupts glibc's allocator free-list
             // glibc detects and aborts: "double free or corruption"
```

Setting `dev = NULL` after the first `free` prevents this: `free(NULL)` is explicitly defined as a no-op in C.

---

### Memory Leak

```c
void process(void) {
    Device *dev = malloc(sizeof(Device));
    if (something_failed()) {
        return;       // LEAK — forgot free(dev) on the error path
    }
    use(dev);
    free(dev);        // only freed on the success path
}
```

For a long-running process like Sentinel-C, leaks accumulate and eventually exhaust RAM. Phase 18 uses valgrind to detect every leak.

---

### Confusing `*` in Declaration vs Expression

```c
int *p = &a;   // * here is PART OF THE TYPE: "p is a pointer to int"
*p = 100;      // * here is an OPERATOR: "dereference p"
```

Two completely different uses of the same symbol. In a declaration, `*` is type syntax. In an expression, `*` is the dereference operator. Every C beginner hits this confusion at least once.

---

### Returning Address of a Local Variable

```c
int *get_value(void) {
    int x = 42;
    return &x;   // x is on the stack — dies when this function returns
}

int *p = get_value();
printf("%d\n", *p);   // undefined behavior
                      // reading dead stack memory
```

When `get_value` returns, `rsp` moves back up. The bytes are still physically there — but unowned. The next function call will overwrite them. If you need data to outlive the function, allocate it on the heap with `malloc`.

---

## 🐛 Troubleshooting Log

### Error: `undefined reference to 'main'`

**When:** First `make run` of Phase 2.

**Full error:**
```
gcc ... src/memory.c -o src/memory
/usr/bin/ld: undefined reference to 'main'
make: *** [<builtin>: src/memory] Error 1
```

**Cause:** The Makefile `SRCS` line was missing the `.c` extension on `memory`:
```makefile
SRCS = src/main.c src/memory    # wrong
```
`patsubst src/%.c,build/%.o` did not match `src/memory` (no `.c`), so it stayed as `src/memory` in `OBJS`. Make tried to build `src/memory` as an executable using its built-in `%: %.c` rule — which requires `main()` that `memory.c` does not have.

**Fix:**
```makefile
SRCS = src/main.c src/memory.c    # correct
```

**Lesson:** `patsubst` silently skips entries that do not match the pattern. Always include the `.c` extension in `SRCS`.

---

### Error: `AddressSanitizer:DEADLYSIGNAL`

**When:** Running with `-fsanitize=address` on WSL 2.

**Cause:** ASan tries to reserve shadow memory at specific high virtual addresses at startup. WSL 2's address space layout conflicts with where ASan wants to map this shadow memory. The result is a fatal signal before any user code runs.

**Fix:** Remove `-fsanitize=address` from `CFLAGS`. Keep `-fsanitize=undefined`.
```makefile
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -fsanitize=undefined
```

**Long-term:** Phase 18 uses valgrind for memory error detection. Valgrind works correctly on WSL 2.

**Note:** `ASAN_OPTIONS=detect_leaks=0 make run` sometimes works as an alternative but is not reliable on all WSL 2 configurations.

---

### Error: Recipe lines using spaces instead of tabs

**When:** Adding `mkdir -p build` and `$(CC)` to the Makefile `$(TARGET)` recipe.

**Symptom:** CLion inserted spaces instead of tabs. Make reported errors on those lines.

**Cause:** Makefile recipe lines must begin with a real tab character (`\t`). Spaces are invalid. Many editors auto-convert tabs to spaces.

**Fix:** Delete the indentation and re-type it with the Tab key. In CLion, `→` characters are tabs; `·` characters are spaces.

---

### Error: `make: /build/sentinel-c: Command not found`

**When:** First successful compile, running `make run`.

**Full error:**
```
/build/sentinel-c
make: /build/sentinel-c: Command not found
```

**Cause:** The `run` recipe had `/$(TARGET)` instead of `./$(TARGET)`:
```makefile
run: $(TARGET)
    /$(TARGET)    # wrong — absolute path from filesystem root /
```
`/build/sentinel-c` means "the file `sentinel-c` in a `build/` directory at the filesystem root." No such file exists there.

**Fix:**
```makefile
run: $(TARGET)
    ./$(TARGET)   # correct — relative to current directory
```

**Lesson:** In shell, `/path` is absolute (from filesystem root). `./path` is relative (from current directory). They look similar but mean completely different things.

---

### IDE Closed Without Saving — `memory.c` Was Empty

**When:** CLion was closed mid-session before saving.

**Symptom:** `memory.c` existed on disk but contained only 1 byte. Compilation failed — `memory_demo()` was defined nowhere.

**Cause:** CLion auto-save did not trigger, or the file was not explicitly saved before closing.

**Fix:** Re-type `memory.c` from scratch. Always `Ctrl+S` before closing.

**Lesson:** The compiler builds from the file on disk, not from the editor buffer. An unsaved file is a different file.

---

### `main.c` Printing "Hello, Sentinel-C" During Phase 2

**When:** Early Phase 2 runs after adding `memory.c`.

**Cause:** `main.c` was not updated — it still called `printf("Hello, Sentinel-C\n")` from Phase 1 instead of `memory_demo()`. Also had `#include <memory.h>` (a system header, not our file) which compiled without errors.

**Fix:**
```c
#include <stdio.h>

void memory_demo(void);

int main(void) {
    memory_demo();
    return 0;
}
```

---

## 💡 Interview Q&A

**Q: What is a pointer in C?**

A pointer is a variable that holds a memory address. Declaring `int *p` creates a variable `p` whose value is an address. The type `int *` tells the compiler what type of value lives at that address — which affects how many bytes to read when dereferencing. On x86-64, a pointer is always 8 bytes regardless of what it points to — it holds a 64-bit address. `&x` gives you the address of variable `x`. `*p` dereferences `p` — reads or writes the value at the address `p` holds.

---

**Q: What is the difference between stack and heap memory?**

Stack memory is automatic. Local variables live there. When a function is called, the CPU carves out a stack frame; when it returns, the frame is discarded. No allocation or deallocation calls needed. Stack addresses are high (`0x7ffd...` on x86-64 Linux). Stack has a fixed maximum size (~8MB on Linux).

Heap memory is manual. You request it with `malloc` and release it with `free`. It outlives the function that allocated it. It can be as large as available RAM. Heap addresses are much lower (`0x55...` or `0x5c...`). You are fully responsible for freeing it — forget to free = memory leak, use after free = undefined behavior, free twice = crash.

---

**Q: What is use-after-free and why is it dangerous?**

Use-after-free is accessing memory through a pointer after that memory has been freed. The freed memory is taken back by the allocator, which may immediately write its own bookkeeping data into it, or give it to a different allocation. The danger is that it is often silent — no crash, no warning without a tool like ASan or valgrind. In security contexts it is exploitable: an attacker who controls the next allocation after the free can position their data where the dangling pointer reads, allowing them to redirect function pointers and execute arbitrary code. This bug class has caused critical CVEs in Chrome, Firefox, the Linux kernel, and OpenSSL.

---

**Q: What does `malloc` return and what must you always do with it?**

`malloc(n)` returns a `void *` pointing to `n` bytes of heap memory, or `NULL` if the allocation failed. Always check for `NULL` before using the pointer — dereferencing null is undefined behavior (segfault in practice). After use, call `free(ptr)` exactly once. After calling `free`, set the pointer to `NULL` to prevent accidental use through the dangling pointer.

---

**Q: Why does C use `->` for struct pointer access instead of just `.`?**

Historical design choice from Kernighan and Ritchie in the early 1970s. In C, `.` accesses a field of a struct value directly, and `->` accesses a field through a pointer. They could have made `.` auto-dereference pointers (as Zig and Odin do), but chose separate operators to make the distinction explicit. `dev->id` is exactly equivalent to `(*dev).id`. The `->` operator has never changed because C maintains backward compatibility with code from 1972.

---

**Q: What is `void *` and why does `printf`'s `%p` require it?**

`void *` is C's generic pointer — a pointer to memory of unspecified type. Any pointer type converts to `void *` implicitly. The C standard specifies that `%p` must receive a `void *`; passing any other pointer type is technically undefined behavior. The cast `(void *)&a` is not "pretending" — it is the correct type that `%p` requires. `void *` is also what `malloc` returns: it does not know what type you want, so it returns a typeless pointer that you assign to the typed pointer variable.

---

**Q: What is `sizeof` and when is it evaluated?**

`sizeof` is a compile-time operator that returns the number of bytes a type or expression occupies. It is evaluated entirely at compile time — no runtime cost. `sizeof(int)` = 4, `sizeof(char)` = 1, `sizeof(Device)` = 8 (two ints). Always use `malloc(sizeof(MyType))` rather than hardcoding numbers — if the struct changes, `sizeof` updates automatically.

---

## 🔗 How It Connects

**Phase 2 → Phase 3 (Strings)**
C strings are `char *` — a pointer to the first character of an array of bytes ending with `\0`. Everything learned about pointers here applies directly. `strcpy` is dangerous because it does not know where your buffer ends — it writes bytes until it hits `\0`, wherever that might be. Buffer overflows are pointer-arithmetic errors.

**Phase 2 → Phase 4 (Structs and Headers)**
The `Device` struct defined inline in `memory.c` will move to `include/device.h` in Phase 4. The `->` operator, `sizeof`, and the malloc/free pattern from this phase are the exact tools used to build and manage every domain struct in Sentinel-C.

**Phase 2 → Phase 5 (The Domain Layer)**
Every domain type — `Device`, `Incident`, `TodoItem` — will be allocated on the heap with `malloc` and freed with `free`. The `device_create` / `device_free` pattern is the "constructor/destructor" pattern in C. Phase 2 is where the mechanics of that pattern are learned.

**Phase 2 → Phase 13 (POSIX Threads)**
Threads share the heap. Two threads calling `malloc` and `free` concurrently is why glibc's allocator uses locking internally. The pointer rules from this phase — especially "after free, set to NULL" and "one free per malloc" — become critical when multiple threads can trigger the same code paths.

**Phase 2 → Phase 18 (Memory Audit and valgrind)**
valgrind detects use-after-free, reads of uninitialized memory, and memory leaks — exactly what ASan would have detected. We removed ASan due to WSL 2 incompatibility. Phase 18 runs the complete Sentinel-C binary under valgrind and fixes every error it finds.

---

## 📝 Quick Reference

### Pointer Syntax

```c
int   a = 42;
int  *p = &a;            // p holds the address of a
int   y = *p;            // dereference: read value at address
*p = 100;                // dereference: write value at address
printf("%p", (void *)p); // print the address itself — must cast to void*
```

### Heap Allocation Pattern

```c
#include <stdlib.h>

Device *dev = malloc(sizeof(Device));   // allocate
if (dev == NULL) { return; }            // always check NULL
dev->id     = 1;                        // use -> for pointer to struct
dev->online = 1;
free(dev);                              // free exactly once
dev = NULL;                             // prevent dangling pointer
```

### Stack vs Heap at a Glance

```
int a = 42;                  // stack — lives until function returns
Device *dev = malloc(...);   // heap  — lives until free(dev)

Stack address: 0x7ffd...     // high addresses
Heap address:  0x55... or 0x5c...   // lower addresses
```

### `->` vs `.`

```c
Device  d;       // not a pointer — use .
d.id = 1;

Device *p = &d;  // pointer — use ->
p->id = 1;       // identical to: (*p).id = 1
```

### Hex Counting

```
...8, 9, a, b, c, d, e, f, 10, 11...
                           ^
                           f + 1 carries → 10

0x10f + 1 = 0x110   (one address apart = one byte apart)
```

### Binary to Hex

```
Split into groups of 4 bits:
0010  1010  =  0x2a  =  42 decimal
  2     a
```

### Memory Address Ranges on x86-64 Linux

```
0x7fff...   — stack    (grows downward ↓)
0x5c...     — heap     (grows upward ↑)
0x7f...     — shared libraries
0x400000    — program code
0x0         — NULL — never mapped — always invalid to dereference
```

### Memory Error Checklist

```
□ Every malloc has a matching free
□ Every malloc return is NULL-checked before use
□ No pointer is used after free (set to NULL after free)
□ No pointer is freed twice
□ No local variable's address is returned from a function
□ malloc(sizeof(Type)) — never hardcode byte counts
```

### Commands

```bash
make run      # compile and run
make clean    # delete build output
```

---

*Phase 2 complete. Pointers, stack, heap, malloc/free — the full memory model.*
*You are the garbage collector. Every malloc needs a free. Every free needs a NULL.*
# Phase 3 — Strings in C

> Your permanent reference for Phase 3.
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

### C Has No String Type

C has no built-in string type. There is no `string`, no `String`, no managed text type. What C calls a "string" is a convention — a pointer to the first byte of a sequence of `char` values that ends with a null terminator (`\0`, byte value 0). The string functions in `<string.h>` all depend on this convention.

### The Null Terminator

The null terminator is the byte `\0` (value `0`) at the end of every C string. It is the only mechanism C has for knowing where a string ends. Every standard string function — `strlen`, `strcpy`, `printf %s`, `strcmp` — walks forward from the start address, byte by byte, until it hits `\0`. Without it, the function has no stopping point.

### `strlen` vs `sizeof`

`strlen(s)` counts the characters up to but **not including** the `\0`. It returns the number of visible characters.

`sizeof(arr)` returns the total number of bytes in the array, **including** the `\0`.

```
char name[] = "Sentinel";

strlen(name) = 8   ← 8 characters, no \0
sizeof(name) = 9   ← 8 characters + 1 null terminator
```

The difference of 1 is the null terminator. This gap is the source of countless buffer overflow bugs — code that allocates `strlen(s)` bytes when it needed `strlen(s) + 1`.

### String Literals vs Char Arrays

There are two ways to create a string in C and they are not interchangeable:

```c
char *s1  = "hello";   // pointer to read-only data segment — cannot modify
char  s2[] = "hello";  // copy of bytes on the stack — fully modifiable
```

`s1` points to bytes baked into the binary at compile time. The OS marks that memory page read-only. Writing to it causes an immediate segmentation fault.

`s2` is a copy of those bytes allocated on the stack when the function runs. You own them. Modify freely.

Both look identical when printed. The difference is invisible until you try to write — then one works and one crashes.

### `strncpy` — Bounded Copy

`strcpy(dest, src)` copies until `\0` with no size limit. It trusts you completely. If `src` is longer than `dest`, it overflows silently.

`strncpy(dest, src, n)` copies at most `n` bytes. Safer — but has a critical gotcha: if `src` fills all `n` bytes without a `\0` being reached, `strncpy` does **not** add the null terminator. You must add it yourself.

```c
char dst[5];
strncpy(dst, src, sizeof(dst) - 1);   // copy at most 4 bytes
dst[sizeof(dst) - 1] = '\0';          // ALWAYS add \0 manually
```

### `snprintf` — The Modern Safe Pattern

`snprintf(dest, size, format, ...)` is like `printf` but writes into a buffer instead of stdout. It always null-terminates. It never writes more than `size` bytes. It returns the number of characters that *would* have been written — letting you detect truncation.

```c
char buf[11];
snprintf(buf, sizeof(buf), "%s", src);   // always safe — always \0-terminated
```

The safety comes entirely from always passing `sizeof(buf)` as the second argument. Never hardcode a number there.

### `strcmp` — String Comparison

You cannot compare strings with `==`. `==` compares pointer addresses, not string content.

`strcmp(a, b)` walks both strings byte by byte:
- Returns `0` if the strings are equal
- Returns a negative number if `a` sorts before `b` (first differing byte: `a[i] < b[i]`)
- Returns a positive number if `a` sorts after `b` (first differing byte: `a[i] > b[i]`)

The exact magnitude is the difference between the first differing bytes. In practice, only the sign matters.

---

## 🟢 Analogies — Simple Layer

### The Null Terminator

A parade of characters marching down the street. The parade has no printed schedule saying how long it is. The only way you know it is over is when a special float at the end rolls by with a sign reading **PARADE OVER**. That float is `\0`. Every string function in C looks for that float.

### `strlen` vs `sizeof`

A row of mailboxes. `strlen` counts the mailboxes that have mail in them. `sizeof` counts all the mailboxes including the empty one reserved at the end for the "PARADE OVER" sign.

### String Literal vs Char Array

A string literal is a sign painted on the wall of a building — it's part of the structure, it's there forever, and you are not allowed to repaint it. A char array is a whiteboard — you own it, you can write on it, erase it, change it freely. Both look the same from across the street. You only notice the difference when you try to pick up a marker.

### `strcpy`

Pouring water from one cup into another. It keeps pouring until the source cup is empty. It does not check whether the destination cup is big enough. If the destination is smaller — water spills over onto whatever is next to it on the table.

### `strcmp`

A judge at a spelling bee. She compares two words letter by letter. The moment she finds a difference, she checks which letter comes first in the alphabet and announces that word as "less than" the other. If she reaches the end of both words with no difference found, she calls it a tie.

---

## 🟡 How It Works — Real Layer

### How a String Sits in Memory

```c
char name[] = "Sentinel";

Address    Byte    Char
──────────────────────────
0x...110    83     'S'
0x...111   101     'e'
0x...112   110     'n'
0x...113   116     't'
0x...114   105     'i'
0x...115   110     'n'
0x...116   101     'e'
0x...117   108     'l'
0x...118     0     '\0'   ← null terminator — end of string
```

`strlen(name)` returns 8 — it counted from `0x...110` to `0x...118` and stopped before counting the `\0`.

`sizeof(name)` returns 9 — it measured the entire array including the `\0` slot.

The pointer `name` (or `&name[0]`) holds the address of the first byte: `0x...110`. That single address is the entire "string" — just a starting point and a convention about where it ends.

### How `printf("%s", s)` Works

`printf` receives a `char *` — the address of the first character. It writes characters to stdout one at a time, advancing the pointer, until it reads a byte whose value is `0`. It stops without writing the `\0` itself. If the `\0` is missing (a corrupted or unterminated string), `printf` keeps reading past your memory and prints garbage until it eventually finds a zero byte somewhere in RAM.

### How `strcmp` Works

```c
strcmp("hello", "world"):

  Step 1: 'h' (104) vs 'w' (119) — different!
  Return: 104 - 119 = -15

strcmp("hello", "hello"):

  Step 1: 'h' vs 'h' — same
  Step 2: 'e' vs 'e' — same
  Step 3: 'l' vs 'l' — same
  Step 4: 'l' vs 'l' — same
  Step 5: 'o' vs 'o' — same
  Step 6: '\0' vs '\0' — same, and both are 0
  Return: 0   ← equal
```

The C standard does not guarantee the exact return value — only the sign. Different implementations may return different nonzero numbers. Never compare `strcmp(a, b) == -1`. Always compare to `0`, `< 0`, or `> 0`.

### Two Kinds of String Storage

```
Binary (read-only data segment):
┌─────────────────────────────────┐
│  "hello\0"   (at 0x402010)      │  ← string literal
│  "Sentinel\0" (at 0x402016)     │  ← another literal
│  Page marked READ-ONLY by OS    │
└─────────────────────────────────┘
         ↑
         char *literal = "hello"
         literal points here, cannot write

Stack (your function frame):
┌─────────────────────────────────┐
│  h  e  l  l  o  \0             │  ← char arr[] = "hello"
│  (your copy, fully writable)    │
└─────────────────────────────────┘
         ↑
         arr[0] = 'H' ✓
```

### The Buffer Overflow Mechanism

```
char buf[5];
strcpy(buf, "Sentinel-C");   // "Sentinel-C" = 10 chars + \0 = 11 bytes

Memory before:
  buf:          [ _ ][ _ ][ _ ][ _ ][ _ ] | [other var][return addr]...
  index:           0    1    2    3    4  |  5    6    7    8    9   10

After strcpy writes 11 bytes into a 5-byte buffer:
  buf:          [ S ][ e ][ n ][ t ][ i ] | [ n ][ e ][ l ][-][ C ][\0]
                                            ^
                                       OVERFLOW — past your memory
                                       corrupting adjacent variables
                                       or the stack return address
```

No crash at the moment of overflow. The corruption is silent. The crash (or exploit) happens later, somewhere completely unrelated.

---

## 🔴 Deep Dives — Senior Layer

### Why `strcpy` Still Exists

`strcpy` has been in C since 1972 and was perfectly reasonable when programs were small and inputs were trusted. It was never designed for hostile input. The problem is that C's API never changed, the internet arrived, and suddenly programs were calling `strcpy` on data controlled by attackers. The function that made perfect sense for a 1970s batch processing system became the most exploited function in the history of systems software.

Many organizations ban `strcpy` entirely. OpenBSD replaced it with `strlcpy` (which always null-terminates, unlike `strncpy`). Microsoft added `strcpy_s`. The C11 standard added Annex K with `strcpy_s` and friends. None of these are universally available. `snprintf` is the portable, universally supported safe alternative.

### Stack Smashing — The Classic Exploit

When `strcpy` overflows a buffer on the stack, it can reach the **return address** — the 8-byte value that tells the CPU where to jump when the function returns. An attacker crafts an input string long enough to reach that return address and overwrite it with an address of their choice. When the function returns, it jumps to the attacker's code.

This attack was described by Aleph One in the 1996 paper "Smashing the Stack for Fun and Profit" and remains one of the most studied exploit techniques. Modern mitigations: stack canaries (a secret value placed between the buffer and return address — if overwritten, the program aborts), ASLR (randomizes where the stack is so attackers cannot predict the return address), and non-executable stack (NX/DEP — the OS refuses to execute code on the stack). None of these are complete defenses. The real fix is bounds checking.

### Why `strncpy` Does Not Null-Terminate

`strncpy` was designed for fixed-width fields in old Unix file system structures (directory entries had 14-byte fixed-width name fields). In that context, null padding (not null termination) made sense — the field was always exactly 14 bytes, no terminator needed. As a general-purpose safe string copy, it is a poor fit: it does not guarantee null termination, it pads with zeros if the source is shorter than `n` (unnecessary performance cost), and the API is confusing. Use `snprintf` instead.

### The Compiler Warning We Hit

gcc's `-Wformat-truncation` (included in `-Wextra`) warns when `snprintf` is statically known to truncate. Combined with `-Werror`, this becomes a compile error:

```c
char sbuf[5];
snprintf(sbuf, sizeof(sbuf), "%s", src);  // -Werror=format-truncation
```

This is the compiler telling us: "I can prove this will truncate, and you told me to treat that as an error." The fix: size the buffer correctly, or suppress the warning explicitly with `#pragma GCC diagnostic`. We sized the buffer correctly — `snprintf`'s safety comes from always passing `sizeof(buf)`, not from the buffer size itself.

### Null-Terminated vs Length-Prefixed Strings

C uses null-terminated strings (NTS). Most modern languages use length-prefixed strings (LPS) — a pointer plus a stored length.

| | Null-Terminated (C) | Length-Prefixed (C#, Zig, Odin) |
|---|---|---|
| Find length | O(n) — must walk to \0 | O(1) — read stored length |
| Can contain \0 | No — \0 ends the string | Yes — length tells you when to stop |
| Buffer overflow risk | Yes — functions don't know end | No — length is always known |
| C interop | Native | Must convert (add \0) |

`strlen` is O(n) — it must walk the entire string every time. In a hot loop, this matters. Cache the result: `size_t len = strlen(s)` once, reuse `len`.

---

## 🖥️ How the Computer Did It

When `strings_demo()` ran, here is what physically happened for each part:

```
Part 1 — strlen and sizeof:

  char name[] = "Sentinel";
  → Compiler computed sizeof("Sentinel") = 9 at compile time
  → At runtime: 9 bytes copied from the data segment onto the stack
  → name[0] through name[7] = 'S' through 'l'
  → name[8] = 0 (the \0)

  strlen(name):
  → Function starts at address of name[0]
  → Reads name[0] = 83 ≠ 0, count = 1
  → Reads name[1] = 101 ≠ 0, count = 2
  → ... (6 more iterations)
  → Reads name[8] = 0 — stop
  → Returns 8

  sizeof(name):
  → Computed entirely at compile time: 9
  → No runtime cost — replaced with the constant 9 in the machine code

Part 2 — segfault on literal write:

  char *literal = "hello";
  → literal holds address 0x402010 (data segment, page marked RO)

  literal[0] = 'H':
  → CPU generates write to 0x402010
  → MMU page table lookup: page is READ-ONLY
  → MMU raises page fault (exception)
  → Linux kernel: SIGSEGV sent to process
  → Process terminated: "Segmentation fault (core dumped)"

Part 3 — strncpy:

  strncpy(dst, src, 4):
  → Copies 'S','e','n','t' (4 bytes) into dst[0..3]
  → src[4] = 'i' — would be byte 5, but limit reached — stops
  → Does NOT write \0
  dst[4] = '\0':
  → We manually write \0 at index 4
  → dst = "Sent\0" — properly terminated 4-character string

Part 4 — strcmp:

  strcmp("hello", "world"):
  → Compare 'h'(104) vs 'w'(119): different
  → Return 104 - 119 = -15
  → (exact value is implementation-defined; sign is guaranteed negative)
```

---

## 🏗️ What We Built

### Files

```
CLookout/
├── src/
│   ├── main.c       ← calls strings_demo() via forward declaration
│   └── strings.c    ← Parts 1-4 of the strings demo
```

### `src/main.c`

```c
#include <stdio.h>

void strings_demo(void);

int main(void) {
    strings_demo();
    return 0;
}
```

### `src/strings.c`

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void strings_demo(void) {
    /* Part 1: a string is bytes + a null terminator */
    char name[] = "Sentinel";

    printf("String: %s\n",  name);
    printf("Length: %zu\n", strlen(name));
    printf("Size:   %zu\n", sizeof(name));
    printf("\n");

    for (int i = 0; i <= (int)strlen(name); i++) {
        printf("name[%d] = %3d  '%c'\n", i, name[i], name[i] ? name[i] : '0');
    }

    /* Part 2: char array (modifiable) vs string literal (read-only) */
    char  modifiable[] = "hello";
    char *literal      = "Hello";

    modifiable[0] = 'H';
    printf("Modified: %s\n", modifiable);
    printf("Literal:  %s\n", literal);
    /* literal[0] = 'H'; */   /* CRASH — read-only memory, segfault */

    /* Part 3: safe string copying */
    char src[] = "Sentinel-C";
    char dst[5];

    strncpy(dst, src, sizeof(dst) - 1);
    dst[sizeof(dst) - 1] = '\0';
    printf("strncpy:  %s\n", dst);

    char sbuf[11];
    snprintf(sbuf, sizeof(sbuf), "%s", src);
    printf("snprintf: %s\n", sbuf);

    /* Part 4: comparing strings — never use == */
    char *x = "hello";
    char *y = "hello";
    char *z = "world";

    printf("strcmp(x, y) = %d\n", strcmp(x, y));
    printf("strcmp(x, z) = %d\n", strcmp(x, z));
    printf("strcmp(z, x) = %d\n", strcmp(z, x));
}
```

### Actual Output

```
String: Sentinel
Length: 8
Size:   9

name[0] =  83  'S'
name[1] = 101  'e'
name[2] = 110  'n'
name[3] = 116  't'
name[4] = 105  'i'
name[5] = 110  'n'
name[6] = 101  'e'
name[7] = 108  'l'
name[8] =   0  '0'
Modified: Hello
Literal:  Hello
strncpy:  Sent
snprintf: Sentinel-C
strcmp(x, y) = 0
strcmp(x, z) = -15
strcmp(z, x) = 15
```

---

## 🌉 The Four-Way Bridge

### The String Type

```csharp
// C# — Sentinel
string name = "Sentinel";
// Immutable, managed, UTF-16 internally, length stored alongside data.
// name.Length is O(1) — no null terminator walk.
// Cannot modify in place — operations return new strings.
// Null safety: compiler warns about nullable string.
```

```zig
// Zig — Lookout/Shell
const name: []const u8 = "Sentinel";
// Slice: pointer + length. Not null-terminated.
// name.len is O(1). Can contain \0 bytes.
// Mutable: var buf: [9]u8 = "Sentinel\x00".*
// C interop uses [*:0]u8 (null-terminated pointer).
```

```odin
// Odin — Watchtower
name := "Sentinel"
// string is pointer + length. Not null-terminated.
// len(name) is O(1). Immutable (use []u8 for mutable).
// C interop: strings.clone_to_cstring(name, allocator)
```

```c
// C — Sentinel-C
char name[] = "Sentinel";
// Just bytes. Null-terminated. strlen is O(n).
// The original that all others were designed to improve upon.
```

### String Copying

```csharp
// C# — string is immutable, just assign
string copy = name;            // references same object (immutable, safe)
string modified = name + "!";  // returns a new string
```

```zig
// Zig
var buf: [9]u8 = undefined;
@memcpy(&buf, "Sentinel");     // compile-time size checking
```

```odin
// Odin
copy := strings.clone(name, allocator)
defer delete(copy, allocator)
```

```c
// C
char buf[11];
snprintf(buf, sizeof(buf), "%s", src);   // always use snprintf — never strcpy
```

### String Comparison

```csharp
if (a == b) { }            // C# — compares content (operator overloaded)
if (string.Equals(a, b)) { }
```

```zig
if (std.mem.eql(u8, a, b)) { }   // explicit byte comparison
```

```odin
if a == b { }              // Odin — == compares content for strings
```

```c
if (strcmp(a, b) == 0) { }   // C — NEVER use == for strings
```

### What Each Language Fixed About C Strings

| C's Problem | C# solution | Zig solution | Odin solution |
|---|---|---|---|
| `strlen` is O(n) | Length stored in string object | Slice carries `.len` | `string` carries length |
| Cannot contain `\0` | Irrelevant (length-based) | Irrelevant (length-based) | Irrelevant (length-based) |
| `strcpy` overflows | Immutable — no copying | Compile-time size checks | Immutable by default |
| `==` compares pointers | `==` compares content | `std.mem.eql` is explicit | `==` compares content |
| No null safety | Nullable type system | Optional pointers (`?*`) | `nil` + compiler checks |

---

## ☠️ Danger Zone

### Buffer Overflow via `strcpy`

```c
char buf[5];
strcpy(buf, "Sentinel-C");   // 11 bytes into a 5-byte buffer
                               // writes 6 bytes past the end of buf
                               // corrupts adjacent stack memory — silently
```

The program may not crash immediately. The corruption may not be noticed for many function calls. When it does crash — or when the attacker exploits it — the cause is invisible in the crash report.

**Rule: never use `strcpy` in new code.** Use `snprintf`.

---

### Missing Null Terminator After `strncpy`

```c
char dst[5];
strncpy(dst, "Sentinel-C", sizeof(dst));  // copies 5 bytes: 'S','e','n','t','i'
                                           // NO \0 written — src filled the buffer
printf("%s\n", dst);                       // reads past dst until a \0 found in RAM
                                           // prints garbage or crashes
```

`strncpy` does NOT add `\0` if the source fills the buffer. The manual termination line is not optional:

```c
strncpy(dst, src, sizeof(dst) - 1);
dst[sizeof(dst) - 1] = '\0';   // mandatory
```

---

### Using `==` to Compare Strings

```c
char *a = "hello";
char *b = "hello";

if (a == b) { }   // compares addresses, not content
                   // may be true (compiler merged literals) or false
                   // behavior is implementation-defined — never do this
```

The compiler may or may not store two identical string literals at the same address (literal deduplication). Relying on `==` produces code that works in debug builds and silently fails in optimized builds.

**Rule: always use `strcmp` for string content comparison.**

---

### Reading Past the Null Terminator

```c
char name[] = "Sentinel";   // 9 bytes: 8 chars + \0

for (int i = 0; i < 20; i++) {
    printf("%c", name[i]);   // reads 11 bytes past the end of the array
                              // undefined behavior from i = 9 onward
}
```

The loop prints "Sentinel" then whatever bytes happen to follow it on the stack. No immediate crash — just reading unowned memory. If those bytes are sensitive data (passwords, keys), you just leaked them.

---

### `sizeof` on a Pointer Is Not String Length

```c
char *s = "hello";
printf("%zu\n", sizeof(s));   // prints 8 — size of the POINTER, not the string
printf("%zu\n", strlen(s));   // prints 5 — the actual string length

char arr[] = "hello";
printf("%zu\n", sizeof(arr)); // prints 6 — size of the ARRAY including \0
```

`sizeof` on a pointer gives you the pointer size (8 bytes on 64-bit). `sizeof` on an array gives you the array's total byte count. They look identical in source code but behave completely differently.

---

### Forgetting `\0` in a Hand-Built String

```c
char buf[5];
buf[0] = 'h';
buf[1] = 'i';
// forgot to set buf[2] = '\0'

printf("%s\n", buf);   // reads past 'i' looking for \0
                        // prints "hi" + whatever garbage follows in RAM
```

When building strings byte by byte, you must explicitly terminate them.

---

## 🐛 Troubleshooting Log

### Error: `strlen + 1` Printed Wrong Length

**When:** Part 1, first run.

**Symptom:** Length printed as 9 instead of 8.

**Cause:** `printf("Length: %zu\n", strlen(name) + 1)` — accidentally added `+ 1` to the `strlen` call itself.

**Fix:** `printf("Length: %zu\n", strlen(name))` — `strlen` returns the correct length without modification.

**Lesson:** `strlen(name) + 1` is the pattern for allocation (`malloc(strlen(name) + 1)`), not for printing. Keep the `+ 1` out of the `strlen` call itself.

---

### `< vs <=` in the Byte Loop

**When:** Part 1, first run.

**Symptom:** Loop stopped at index 7 (the `l`). The null terminator at index 8 was never printed.

**Cause:** `for (int i = 0; i < strlen(name); i++)` — the `<` stops one short of `strlen(name)`. To include the `\0`, the condition must be `<=`.

**Fix:** `for (int i = 0; i <= (int)strlen(name); i++)`

---

### `-Werror=format-truncation` on `snprintf`

**When:** Part 3, first attempt with `char sbuf[5]`.

**Full error:**
```
src/strings.c:40:35: error: '%s' directive output may be truncated writing
up to 10 bytes into a region of size 5 [-Werror=format-truncation=]
```

**Cause:** gcc's `-Wformat-truncation` (enabled by `-Wextra`) statically detected that `snprintf` would truncate. Combined with `-Werror`, this became a compile error.

**Why it happened:** We were intentionally demonstrating truncation, but the compiler treated it as an error.

**Fix:** Size the buffer large enough so no truncation occurs:
```c
char sbuf[11];   // "Sentinel-C\0" = 11 bytes — no truncation
snprintf(sbuf, sizeof(sbuf), "%s", src);
```

**Lesson:** `-Werror=format-truncation` is actually helpful in production — it catches cases where you accidentally truncate output you meant to preserve. For intentional truncation in production code, use `#pragma GCC diagnostic push/ignored/pop` to suppress explicitly.

---

## 💡 Interview Q&A

**Q: How do strings work in C?**

C has no built-in string type. A C string is a `char *` pointer to the first byte of a sequence of bytes that ends with a null terminator — the byte value `0`, written `'\0'`. Every standard string function (`strlen`, `strcpy`, `printf %s`, `strcmp`) walks forward from the starting address until it finds a `\0`. Without the null terminator, functions have no stopping point and will read garbage or crash. The programmer is responsible for ensuring every string is properly null-terminated and that every buffer holding a string has at least `strlen(s) + 1` bytes.

---

**Q: What is the difference between `strlen` and `sizeof` for strings?**

`strlen(s)` counts the characters up to but not including the null terminator. It is a runtime function — it walks the bytes. `sizeof(arr)` for a char array returns the total number of bytes including the null terminator. It is a compile-time operator — no runtime cost. `sizeof` on a pointer returns the pointer size (8 bytes on 64-bit), not the string length. The gap between `strlen` and `sizeof` is always 1 — exactly the size of the null terminator. Using `strlen` for allocation without adding 1 is the classic off-by-one buffer overflow.

---

**Q: What is a buffer overflow and how does `strcpy` cause one?**

A buffer overflow is writing more bytes into a buffer than it can hold, overwriting adjacent memory. `strcpy(dest, src)` copies bytes from `src` to `dest` until it hits `\0`, with no size parameter and no bounds check. If `src` is longer than `dest`, it writes past the end of `dest`, corrupting whatever follows in memory — other variables, the stack frame's saved registers, or the return address. The program does not necessarily crash immediately. The corruption is silent. In security contexts, an attacker who controls the input to `strcpy` can overwrite the return address with an arbitrary value, redirecting execution to attacker-controlled code. This technique — stack smashing — was described in 1996 and remains relevant today.

---

**Q: What is the safe alternative to `strcpy`?**

`snprintf(dest, sizeof(dest), "%s", src)` is the preferred modern approach. It always null-terminates the output, it never writes more than `sizeof(dest)` bytes, and it returns the number of characters that would have been written — allowing you to detect truncation. The key habit: always pass `sizeof(dest)` as the second argument, never a hardcoded number. `strncpy` is an older alternative that copies at most `n` bytes but does not add `\0` if the source fills the buffer — you must add it manually with `dst[n-1] = '\0'`. For new code, prefer `snprintf`.

---

**Q: Why can't you use `==` to compare strings in C?**

`==` on pointers compares addresses, not content. `char *a = "hello"` and `char *b = "hello"` may or may not point to the same address — the compiler can deduplicate string literals or not, and the standard does not guarantee it. `a == b` might be true (if the compiler merged the literals) or false (if it stored them separately), and that behavior can change between compiler versions, optimization levels, or translation units. To compare string content, use `strcmp(a, b) == 0`. To compare case-insensitively, use `strcasecmp` (POSIX).

---

**Q: What is the difference between `char *s = "hello"` and `char s[] = "hello"`?**

`char *s = "hello"` stores the bytes `hello\0` in the binary's read-only data segment and sets `s` to point there. Writing to `s[0]` causes a segmentation fault — the OS marks that page non-writable. `char s[] = "hello"` allocates 6 bytes on the stack when the function runs and copies the bytes there. You own those bytes and can modify them freely. Both print identically with `%s`. The difference is invisible until you try to write — then one works and one crashes with no warning from the compiler.

---

## 🔗 How It Connects

**Phase 3 → Phase 4 (Structs and Headers)**

Every domain struct in Sentinel-C will have string fields — device names, incident titles, todo text. Those fields will be `char` arrays of fixed size (`char name[64]`). Everything learned here — `strncpy` with manual termination, `snprintf` for safe copying, `strcmp` for comparison — is how you work with those fields. Phase 4 introduces the structs that hold them.

**Phase 3 → Phase 5 (The Domain Layer)**

`Device`, `Incident`, and `TodoItem` structs will have `char name[64]`, `char ip[16]`, `char title[128]` style fields. Creating a device will use `snprintf(dev->name, sizeof(dev->name), "%s", input)`. Finding a device by name will use `strcmp(dev->name, query)`. These are the exact patterns from Phase 3.

**Phase 3 → Phase 9 (JSON with cJSON)**

JSON parsing produces C strings. Loading a device from JSON means copying the parsed name string into the struct's name field safely with `snprintf`. The null terminator rules from this phase are what make that safe.

**Phase 3 → Phase 14 (BSD Sockets / AI Assistant)**

Network data arrives as raw bytes. Treating received bytes as a C string requires verifying null termination before calling any string function. A missing `\0` in network input that gets passed to `strlen` or `printf("%s")` is a classic vulnerability. Phase 3's rules apply directly to network input handling.

---

## 📝 Quick Reference

### The Null Terminator Rule

```
"Sentinel"  →  8 chars  →  needs 9 bytes (strlen + 1)

strlen("Sentinel") = 8   // count of characters
sizeof(char[9])    = 9   // total bytes including \0
```

### String Storage — Two Types

```c
char *literal  = "hello";   // READ-ONLY — do not write
char  array[]  = "hello";   // read-write — stack copy
```

### Safe Copy Pattern

```c
// strncpy — manual \0 required
strncpy(dst, src, sizeof(dst) - 1);
dst[sizeof(dst) - 1] = '\0';

// snprintf — preferred, always \0-terminated
snprintf(dst, sizeof(dst), "%s", src);
```

### Comparison Pattern

```c
// WRONG
if (a == b) { }          // compares addresses

// CORRECT
if (strcmp(a, b) == 0)   // equal
if (strcmp(a, b) < 0)    // a sorts before b
if (strcmp(a, b) > 0)    // a sorts after b
```

### Format Specifiers for Strings

```c
%s     char* — prints until \0
%5s    right-align in 5-char field
%-5s   left-align in 5-char field
%.4s   print at most 4 characters (truncate)
%zu    size_t — use for strlen/sizeof results
```

### Include

```c
#include <string.h>   // strlen, strcpy, strncpy, strcmp, strncmp
#include <stdio.h>    // snprintf, printf
```

### The Allocation Rule

```c
// WRONG — no room for \0
char *buf = malloc(strlen(name));

// CORRECT
char *buf = malloc(strlen(name) + 1);
```

### Memory Error Checklist for Strings

```
□ Every string buffer is strlen + 1 bytes minimum
□ snprintf used for copying, never strcpy
□ strncpy always followed by manual dst[n-1] = '\0'
□ strcmp used for comparison, never ==
□ sizeof used for buffer sizes, strlen used for content lengths
□ Never sizeof(pointer) expecting string length — use strlen
```

---

*Phase 3 complete. Strings in C are bytes with a null terminator and nothing else.*
*Every string function is just a loop that stops at \0.*
*The safe pattern: snprintf with sizeof. The comparison: strcmp with == 0.*
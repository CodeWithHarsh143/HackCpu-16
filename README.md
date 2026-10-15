# HackCpu-16

**A 16-bit Hack computer emulator written in C++, built bottom-up from NAND gates.**

HackCpu-16 is a software implementation of the **Hack computer architecture** from [Nand2Tetris], built from the hardware level upward.

The project starts with primitive logic gates and builds the complete machine through adders, registers, RAM, ALU, CPU, memory-mapped I/O, and finally a runnable Hack program.

The emulator can execute real `.hack` machine code and provides a live **512×256 graphical display and keyboard input** through SDL3.

> **Goal:** understand how a computer works from logic gates → CPU → memory → machine code → running programs.

---

## Demo

![HackCpu-16 running](docs/demo.png)

Hack programs execute inside the emulator and interact with the outside world exclusively through the Hack memory map.

---

## Architecture

```text
                         ┌───────────────────────────────┐
                         │          HackCpu-16            │
                         │                               │
 .hack program ─────────►│ ROM32K                        │
                         │      │                        │
                         │      ▼                        │
                         │    ┌─────┐                    │
                         │    │ CPU │                    │
                         │    └──┬──┘                    │
                         │       │                       │
                         │       ▼                       │
                         │    Memory                     │
                         │       │                       │
                         │   ┌───┴───────────────┐       │
                         │   │       │           │       │
                         │ RAM16K  Screen     Keyboard   │
                         │   │       │           │       │
                         └───┼───────┼───────────┼───────┘
                             │       │           │
                             │       ▼           │
                             │   SDL3 Window ◄───┘
                             │
                             ▼
                         Program State
```

The CPU follows the Hack architecture:

```text
             ┌──────────────┐
             │   ROM32K     │
             │ Instructions │
             └──────┬───────┘
                    │
                    ▼
             ┌──────────────┐
             │     CPU      │
             │              │
             │ A Register   │
             │ D Register   │
             │ ALU          │
             │ PC           │
             └──────┬───────┘
                    │
                    ▼
             ┌──────────────┐
             │    Memory    │
             ├──────────────┤
             │ RAM16K       │
             │ Screen       │
             │ Keyboard     │
             └──────────────┘
```

---

# From NAND Gates to a Computer

The hardware hierarchy is built bottom-up rather than treating the CPU as a black box.

```text
NAND
 │
 ├── NOT / AND / OR / XOR
 │
 ├── Mux / DMux
 │
 ├── HalfAdder / FullAdder
 │
 ├── 16-bit Adders
 │
 ├── DFF
 │    └── Bit
 │         └── Register
 │
 ├── RAM8
 │    └── RAM64
 │         └── RAM512
 │              └── RAM4K
 │                   └── RAM16K
 │
 ├── ALU
 │
 ├── PC
 │
 └── CPU
      │
      └── Hack Computer
```

The implementation follows the conceptual hardware hierarchy from **Nand2Tetris Part I**, while using C++ instead of the course's HDL.

---

# Hardware vs Software Simulation

HackCpu-16 deliberately distinguishes between **hardware behaviour** and **simulation plumbing**.

### Hardware semantics

Components where the actual hardware behaviour is important are modelled explicitly:

* NAND and primitive logic gates
* Multiplexers and demultiplexers
* Half adders and full adders
* 16-bit arithmetic
* ALU control bits
* Zero and negative flags
* D flip-flop
* Registers
* Hierarchical RAM
* Program Counter
* CPU datapath
* Destination decoding
* Memory-mapped I/O

### Software-level implementation

Some parts are intentionally implemented using normal C++ because reproducing their gate-level structure would add simulation cost without improving the observable behaviour:

* Instruction decoding
* CPU control flow
* ROM loading
* Top-level memory routing
* `.hack` file parsing
* SDL rendering
* Keyboard event handling
* File I/O

This gives the project a useful balance:

> **Hardware semantics where they matter, software abstractions where they don't.**

---

# CPU

The CPU implements the Hack instruction set defined by Nand2Tetris.

### A-instruction

```text
0vvvvvvvvvvvvvvv
```

Loads a 15-bit value into the A register.

### C-instruction

```text
111 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
```

The CPU handles:

* `comp`
* `dest`
* `jump`
* A/M selection through the `a` bit
* ALU control signals
* `zr` and `ng` flags
* A and D register writes
* Program Counter updates

For example:

```asm
@SCREEN
M=0
```

becomes machine code and is executed by the same CPU datapath used by every other Hack program.

---

# Memory Map

HackCpu-16 implements the Hack memory-mapped I/O model.

```text
Address Range       Device          Size
────────────────────────────────────────────
0 - 16383           RAM16K          16K words
16384 - 24575       Screen          8K words
24576               Keyboard        1 word
24577 - 32767       Invalid         Unmapped
```

### Screen

The Hack screen is:

```text
512 × 256 pixels
```

Each 16-bit word controls 16 horizontal pixels.

```text
address = 16384 + row × 32 + col / 16
bit     = col % 16
```

Therefore:

```text
512 / 16 = 32 words per row

32 × 256 = 8192 words
```

which corresponds exactly to the Hack screen's 8K-word memory region.

---

# Screen Implementation

The logical screen remains part of the Hack memory system, while SDL3 provides the host-side visual output.

```text
CPU
 │
 │ writes to Screen memory
 ▼
Screen storage
 │
 ▼
Framebuffer
 │
 ▼
SDL3 texture
 │
 ▼
512 × 256 window
```

The renderer uses an `ARGB8888` texture and uploads the framebuffer once per frame rather than issuing one SDL draw call per pixel.

The display is scaled using nearest-neighbour rendering so each Hack pixel remains visually crisp.

---

# Keyboard

The keyboard is represented by a single memory-mapped word:

```text
Address 24576
```

SDL3 keyboard events are converted into Hack keyboard codes.

Example mappings include:

```text
Enter       → 128
Backspace   → 129
Left        → 130
Up          → 131
Right       → 132
Down        → 133
Esc         → 140
F1-F12      → 141-152
```

Normal ASCII characters are passed through directly.

When a key is pressed:

```text
Keyboard[24576] = key_code
```

When released:

```text
Keyboard[24576] = 0
```

---

# RAM Hierarchy

The RAM implementation follows the structural hierarchy of the Hack platform:

```text
Register
   │
  RAM8
   │
  RAM64
   │
 RAM512
   │
 RAM4K
   │
 RAM16K
```

Address decoding and load routing are implemented using the same conceptual **Mux/DMux selection logic** used by the Hack hardware.

For performance, selected paths can be evaluated directly during simulation rather than recursively evaluating every unused branch.

The externally observable behaviour remains equivalent to the hardware structure.

---

# ROM32K

The instruction memory contains:

```text
32,768 × 16-bit words
```

Unlike RAM, ROM is immutable during CPU execution, so it is represented as a flat C++ array and populated by loading a `.hack` program.

```text
.asm
  │
  ▼
Assembler
  │
  ▼
.hack
  │
  ▼
ROM32K
  │
  ▼
CPU
```

---

# Assembler

HackCpu-16 includes an assembler capable of translating Hack assembly language into machine code.

Example:

```asm
@2
D=A
@3
D=D+A
@0
M=D
```

becomes a sequence of 16-bit Hack instructions.

The assembler handles:

* A-instructions
* C-instructions
* `comp`
* `dest`
* `jump`
* Labels
* Symbols
* Predefined symbols

---

# Simulator

The simulator loads a `.hack` program into ROM and continuously executes CPU cycles.

```text
program.hack
      │
      ▼
   ROM32K
      │
      ▼
     CPU
      │
      ├────────► RAM
      │
      ├────────► Screen ───► SDL3
      │
      └────────► Keyboard ◄── SDL3
```

This allows actual Hack programs to interact with a graphical display and keyboard rather than only producing console output.

---

# Example Programs

Hack programs can directly manipulate memory-mapped devices.

For example, writing to the screen:

```asm
@SCREEN
M=-1
```

sets the first 16 screen pixels to black.

A larger Hack program can use the same interface to implement:

* graphics
* keyboard-controlled programs
* games
* text rendering
* simple operating-system components

---

# Testing

The project uses **Catch2** for automated testing.

Tests cover the hardware hierarchy and simulator components, including:

* Logic gates
* Mux / DMux
* Adders
* ALU
* Registers
* RAM hierarchy
* Program Counter
* CPU
* ROM
* Memory mapping
* Screen behaviour
* Keyboard mapping
* Assembler behaviour

The goal is to verify individual hardware components before relying on them in higher-level components.

---

# Build

## Requirements

* C++17 compiler
* CMake 3.28+
* SDL3
* Catch2

SDL3 and Catch2 are fetched automatically through the CMake configuration.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

## Run the Simulator

```bash
./build/hackcpu16_simulator
```

Then provide a `.hack` program to execute it inside the emulator.

---

# Bit Convention

HackCpu-16 consistently uses:

```text
bit[0] = least significant bit
bit[15] = most significant bit
```

For example:

```text
value = 1

bit[0] = 1
bit[1] = 0
...
bit[15] = 0
```

This convention is enforced throughout the gates, arithmetic units, RAM, CPU, and ROM interfaces.

---

# Project Structure

```text
HackCpu-16/
│
├── include/
│   └── hackcpu/
│       └── hardware/
│
├── src/
│   └── hardware/
│
├── apps/
│   ├── simulator/
│   └── assembler/
│
├── tests/
│
├── programs/
│
├── docs/
│   └── demo.png
│
├── CMakeLists.txt
└── README.md
```

---

# Current Status

HackCpu-16 is being developed incrementally alongside the **Nand2Tetris** hardware and assembly material.

### Completed

* [x] Primitive logic gates
* [x] Mux / DMux
* [x] Adders
* [x] ALU
* [x] DFF / registers
* [x] Hierarchical RAM
* [x] Program Counter
* [x] ROM32K
* [x] CPU
* [x] Memory-mapped screen
* [x] Keyboard input
* [x] SDL3 display
* [x] Hack assembler
* [x] Automated tests

### In Progress
* [ ] Hack Assembler 
* [ ] Further memory-map validation
* [ ] More complete Hack program compatibility
* [ ] More simulator tooling
* [ ] Additional integration tests

---

# Learning Resources

The architecture is based on the **Nand2Tetris** course, particularly the hardware and assembly portions.

**Course:**
[The Elements of Computing Systems — Nand2Tetris]

The project is intentionally being implemented while learning the underlying concepts rather than simply reproducing the reference HDL implementations.

---

# Why This Project?

Most emulators begin at the CPU instruction level.

HackCpu-16 starts much lower:

```text
NAND
 ↓
Logic Gates
 ↓
Arithmetic
 ↓
Storage
 ↓
ALU
 ↓
CPU
 ↓
Memory
 ↓
Machine Code
 ↓
Running Program
```

The objective is not just to emulate a Hack computer, but to understand the chain of abstractions that turns **Boolean logic into a programmable computer**.

---

# Tech Stack

| Component          | Technology         |
| ------------------ | ------------------ |
| Language           | C++17              |
| Build System       | CMake              |
| Graphics / Input   | SDL3               |
| Testing            | Catch2             |
| Architecture       | Hack 16-bit        |
| Reference          | Nand2Tetris Part I |
| Instruction Format | Hack machine code  |

---

C++17, CMake, SDL3 (display/input), Catch2 (tests). No external HDL tooling:
the hardware lives in ordinary, debuggable code.
Making 16 bit hack cpu from nand gate as a starting point. Refrence nand to tetris playlist in yt (currrently learning assembly)
Right Now-> working on cpu and assembler 

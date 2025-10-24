# RISC-V Kernel Implementation

This project implements a **small but fully functional kernel** for the RISC-V architecture, supporting multithreading with time-sharing capabilities. 
Developed in **C++** and **RISC-V assembly**, the kernel provides core operating system abstractions including threads, semaphores, memory allocation and context switching.
The implementation follows a library-based approach where the kernel and user applications share the same address space, making it suitable for embedded systems.

The kernel executes within a **QEMU emulator** simulating a RISC-V processor, with a modified xv6 operating system serving as the host environment. 
This educational project demonstrates fundamental OS concepts such as process scheduling, synchronization primitives, interrupt handling, 
and memory management in a controlled, single-processor environment.

> The primary motivation behind this project was to gain deep understanding of **operating system internals**, particularly thread management, context switching, and interrupt handling at the hardware level. By implementing these core mechanisms from scratch using both high-level C++ and low-level assembly, I developed practical knowledge of OS design principles and RISC-V architecture.
>
> This project was developed as the [university assignment](instructions.pdf) for the "Operating Systems 1" course at the University of Belgrade School of Electrical Engineering, majoring in Software Engineering. Please refer to the file for detailed assignment instructions.

## Table of Contents

- [System Overview](#system-overview)
  - [Architecture](#architecture)
  - [Privilege Modes](#privilege-modes)
- [System Calls](#system-calls)
  - [Memory Management](#memory-management)
  - [Thread Management](#thread-management)
  - [Semaphore Operations](#semaphore-operations)
  - [Time and Console Operations](#time-and-console-operations)
- [RISC-V Registers](#risc-v-registers)
  - [General Purpose Registers](#general-purpose-registers)
  - [System Registers (Supervisor Mode)](#system-registers-supervisor-mode)
- [API Layers](#api-layers)
  - [ABI (Application Binary Interface)](#abi-application-binary-interface)
  - [C API](#c-api)
  - [C++ API](#c-api-1)
- [Building and Running](#building-and-running)
- [Assignment Reference](#assignment-reference)

## System Overview

### Architecture

The kernel implements a **monolithic architecture** where all kernel code executes in privileged mode within a unified address space. The system consists of several abstraction layers:

<p align="center">
  <img width="365" height="4035" alt="image" src="https://github.com/user-attachments/assets/257b767f-e077-4dbb-87c8-504eb1b2cc04" />
</p>

All layers are statically linked into a single executable that runs on the RISC-V processor. 
User code executes in unprivileged mode, while kernel code runs in privileged (supervisor) mode, with transitions managed through the `ecall` instruction.

### Privilege Modes

The RISC-V architecture implements a hierarchical privilege model to provide security and isolation. This project utilizes two privilege levels:

- **User Mode (U-mode)**: The lowest privilege level where application code executes. In this mode:
  - Access to privileged instructions and system registers is forbidden
  - System calls must be used to request kernel services
  - All user threads run in this mode by default

- **Supervisor Mode (S-mode)**: A privileged mode where the kernel executes. In this mode:
  - Access to Control and Status Registers (CSRs) is granted
  - Privileged instructions (e.g., `sret`, CSR manipulation) can be executed
  - Direct hardware access is possible
  - Interrupt and exception handling occurs in this mode

**Mode Transitions:**
- **U → S**: Triggered by `ecall` instruction (system calls), exceptions, or interrupts
- **S → U**: Executed via `sret` instruction after completing kernel operations

The current privilege mode is tracked by the hardware, and the `sstatus.SPP` bit stores the previous privilege level to enable proper mode restoration after trap handling.

## System Calls

The kernel provides system calls organized into four functional categories:

### Memory Management

| System Call | Code | Signature | Description |
|------------|------|-----------|-------------|
| `mem_alloc` | `0x01` | `void* mem_alloc(size_t size)` | Allocates memory block of at least `size` bytes, aligned to `MEM_BLOCK_SIZE`. Returns pointer on success, `null` on failure. |
| `mem_free` | `0x02` | `int mem_free(void*)` | Frees previously allocated memory. Returns 0 on success, negative error code on failure. |
| `mem_get_free_space` | `0x03` | `size_t mem_get_free_space()` | Returns total free memory in bytes. |
| `mem_get_largest_free_block` | `0x04` | `size_t mem_get_largest_free_block()` | Returns size of largest contiguous free memory block in bytes. |

### Thread Management

| System Call | Code | Signature | Description |
|------------|------|-----------|-------------|
| `thread_create` | `0x11` | `int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)` | Creates new thread executing `start_routine(arg)`. Stores thread handle in `*handle`. Returns 0 on success, negative on failure. |
| `thread_exit` | `0x12` | `int thread_exit()` | Terminates current thread. Returns negative error code on failure. |
| `thread_dispatch` | `0x13` | `void thread_dispatch()` | Yields processor to another ready thread (may resume same thread). |
| `thread_get_id` | `0x14` | `uint64 thread_get_id()` | Returns the unique identifier of the current thread. |

### Semaphore Operations

| System Call | Code | Signature | Description |
|------------|------|-----------|-------------|
| `sem_open` | `0x21` | `int sem_open(sem_t* handle, unsigned init)` | Creates semaphore with initial value `init`. Stores handle in `*handle`. Returns 0 on success, negative on failure. |
| `sem_close` | `0x22` | `int sem_close(sem_t handle)` | Destroys semaphore. All waiting threads are unblocked with error. Returns 0 on success, negative on failure. |
| `sem_wait` | `0x23` | `int sem_wait(sem_t id)` | P (wait) operation on semaphore. Blocks if value is 0. Returns 0 on success, negative on failure (including if semaphore was closed). |
| `sem_signal` | `0x24` | `int sem_signal(sem_t id)` | V (signal) operation on semaphore. Returns 0 on success, negative on failure. |

### Time and Console Operations

| System Call | Code | Signature | Description |
|------------|------|-----------|-------------|
| `time_sleep` | `0x31` | `int time_sleep(time_t)` | Suspends calling thread for specified time in timer periods. Returns 0 on success, negative on failure. |
| `getc` | `0x41` | `char getc()` | Reads one character from console input buffer. Blocks if buffer empty. Returns character on success, `EOF` on error. |
| `putc` | `0x42` | `void putc(char)` | Writes character to console output. |

## RISC-V Registers

The kernel implementation utilizes various RISC-V registers for both general computation and system-level operations. Understanding these registers is crucial for kernel development and debugging.

### General Purpose Registers

These registers are accessible in both user and supervisor modes:

| Register | Name | Purpose |
|----------|------|---------|
| `x0` / `zero` | Zero | Hardwired to constant 0 (writes ignored, reads always return 0) |
| `x1` / `ra` | Return Address | Stores return address for function calls |
| `x2` / `sp` | Stack Pointer | Points to the top of the current stack (grows toward lower addresses) |
| `x3` / `gp` | Global Pointer | Points to global data section (rarely used in this project) |
| `x4` / `tp` | Thread Pointer | Points to thread-local storage (rarely used in this project) |
| `x5-x7` / `t0-t2` | Temporaries | Temporary registers (caller-saved, not preserved across calls) |
| `x8` / `s0` / `fp` | Saved / Frame Pointer | Callee-saved register, often used as frame pointer |
| `x9` / `s1` | Saved | Callee-saved register (preserved across function calls) |
| `x10-x11` / `a0-a1` | Arguments / Return | Function arguments and return values |
| `x12-x17` / `a2-a7` | Arguments | Additional function arguments |
| `x18-x27` / `s2-s11` | Saved | Callee-saved registers (preserved across function calls) |
| `x28-x31` / `t3-t6` | Temporaries | Additional temporary registers (caller-saved) |

**Note:** The compiler automatically preserves callee-saved registers (`s0-s11`) across function calls, while caller-saved registers (`t0-t6`, `a0-a7`) may be overwritten.

### System Registers (Supervisor Mode)

These Control and Status Registers (CSRs) are accessible only in supervisor mode and are critical for interrupt handling and system control:

| Register | Name | Purpose |
|----------|------|---------|
| `sstatus` | Supervisor Status | Contains various status bits including interrupt enable (SIE), previous interrupt enable (SPIE), and previous privilege mode (SPP) |
| `sie` | Supervisor Interrupt Enable | Enables/disables specific interrupt sources (software interrupts via SSIE bit, external interrupts via SEIE bit) |
| `sip` | Supervisor Interrupt Pending | Indicates which interrupts are pending (SSIP for software interrupts, SEIP for external interrupts) |
| `stvec` | Supervisor Trap Vector | Holds the address of the trap handler (interrupt/exception routine). Lower 2 bits indicate mode: 0 = direct, 1 = vectored |
| `sepc` | Supervisor Exception PC | Stores the program counter (PC) of the instruction that caused the trap or was interrupted |
| `scause` | Supervisor Cause | Indicates the cause of the trap. MSB indicates interrupt (1) vs exception (0); remaining bits specify the specific cause |
| `sscratch` | Supervisor Scratch | General-purpose scratch register for temporary storage during trap handling |

**Key `scause` Values:**

| MSB | Value | Description |
|-----|-------|-------------|
| 1 | 1 | Software interrupt (from machine mode) |
| 1 | 9 | External hardware interrupt (timer or console) |
| 0 | 2 | Illegal instruction exception |
| 0 | 5 | Load access fault |
| 0 | 7 | Store access fault |
| 0 | 8 | Environment call from user mode (`ecall` instruction) |
| 0 | 9 | Environment call from supervisor mode |

**`sstatus` Key Bits:**

- **SIE (bit 1)**: Supervisor Interrupt Enable - when 0, interrupts are disabled in supervisor mode
- **SPIE (bit 5)**: Stores previous SIE value before trap
- **SPP (bit 8)**: Stores previous privilege mode (0 = user, 1 = supervisor)

**Additional Information:**

For comprehensive details about RISC-V registers, instruction encoding, and privileged architecture, refer to:
- The project [instructions.pdf](instructions.pdf) document, which contains architecture-specific details for this implementation
- The official [RISC-V Specification](https://riscv.org/technical/specifications/) from RISC-V International

## API Layers

### ABI (Application Binary Interface)

System calls are invoked using the `ecall` instruction with parameters passed through RISC-V registers:

- `a0`: System call code (from table above)
- `a1, a2, ...`: System call parameters (left to right in function signature)
- `a0`: Return value (after syscall completes)

**Key ABI differences from C API:**
- `mem_alloc` (0x01): Size parameter expressed in **blocks**, not bytes
- `thread_create` (0x11): Takes additional `stack_space` parameter pointing to pre-allocated stack

## C API

The C API provides a procedural wrapper around the ABI layer, offering conventional function call interface. All functions handle argument preparation and invoke the corresponding system call via the `ecall` instruction.

> **Note:** All C API functions and types are declared in `syscall_c.hpp`.
> All C API functions declared in `syscall_c.hpp` and implemented using helper functions `syscall0` through `syscall4` 
(corresponding to 0-4 parameters) that encapsulate the `ecall` instruction and register manipulation. !!!!!!!!!!!

### Memory Management

| Function | Signature | Description |
|----------|-----------|-------------|
| `mem_alloc` | `void* mem_alloc(size_t size)` | Allocates memory block of at least `size` bytes, rounded up and aligned to `MEM_BLOCK_SIZE`. Returns pointer on success, `null` on failure. Converts byte size to blocks before invoking ABI system call. |
| `mem_free` | `int mem_free(void* ptr)` | Frees previously allocated memory block. Returns `0` on success, negative error code on failure. Parameter must be value returned from `mem_alloc`. |
| `mem_get_free_space` | `size_t mem_get_free_space()` | Returns total available free memory in bytes. |
| `mem_get_largest_free_block` | `size_t mem_get_largest_free_block()` | Returns size of largest contiguous free memory fragment in bytes. |

### Thread Management

**Type Definitions:**
```c
class _thread;
typedef _thread* thread_t;
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `thread_create` | `int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)` | Creates new thread executing `start_routine(arg)`. Stores thread handle in `*handle`. Returns `0` on success, negative on failure. Thread created with `DEFAULT_STACK_SIZE` and `DEFAULT_TIME_SLICE`. Allocates stack using `mem_alloc` before invoking ABI. |
| `thread_exit` | `int thread_exit()` | Terminates calling thread. Returns negative error code on failure (should not return on success). |
| `thread_dispatch` | `void thread_dispatch()` | Voluntarily yields processor to another thread. Scheduler may select same or different ready thread. |
| `thread_get_id` | `uint64 thread_get_id()` | Returns unique identifier of currently executing thread as 64-bit unsigned integer. |

### Semaphore Operations

**Type Definitions:**
```c
class _sem;
typedef _sem* sem_t;
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `sem_open` | `int sem_open(sem_t* handle, unsigned init)` | Creates semaphore with initial value `init`. Stores handle in `*handle`. Returns `0` on success, negative on failure. |
| `sem_close` | `int sem_close(sem_t handle)` | Destroys semaphore. All blocked threads unblocked with error return. Returns `0` on success, negative on failure. |
| `sem_wait` | `int sem_wait(sem_t id)` | P (wait) operation on semaphore. Blocks if value is 0. Returns `0` on success, negative on failure (including when semaphore closed while waiting). |
| `sem_signal` | `int sem_signal(sem_t id)` | V (signal) operation on semaphore. Increments value and wakes one waiting thread if any. Returns `0` on success, negative on failure. |

### Time Operations

**Type Definitions:**
```c
typedef unsigned long time_t;
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `time_sleep` | `int time_sleep(time_t duration)` | Suspends calling thread for `duration` timer periods. Returns `0` on success, negative on failure. |

### Console Operations

**Constants:**
```c
const int EOF = -1;
```

| Function | Signature | Description |
|----------|-----------|-------------|
| `getc` | `char getc()` | Reads one character from console input buffer. Blocks if buffer empty until character available. Returns character on success, `EOF` on error. |
| `putc` | `void putc(char c)` | Writes character `c` to console output. |

### System Constants

The following constants are declared in `hw.h` and defined in `hw.lib`:
```c
extern const size_t DEFAULT_STACK_SIZE;      // Default thread stack size
extern const time_t DEFAULT_TIME_SLICE;      // Default thread time slice
extern const void* HEAP_START_ADDR;          // Start of allocatable memory
extern const void* HEAP_END_ADDR;            // End of allocatable memory
extern const size_t MEM_BLOCK_SIZE;          // Memory allocation block size
```

### C++ API

Object-oriented wrapper providing modern C++ interface around the C API.
> **Note:** All C++ API classes are declared in `syscall_cpp.hpp`.
> Global operator overloads are implemented in `operators.cpp`.

### Global Operators
```cpp
void* operator new(size_t size);
void operator delete(void* ptr) noexcept;
void* operator new[](size_t size);
void operator delete[](void* ptr) noexcept;
```

Overloaded global operators that redirect dynamic memory allocation to the kernel's memory allocator:
- `operator new` / `operator new[]`: Allocate memory using kernel's `MemoryAllocator`, enabling C++ dynamic object creation
- `operator delete` / `operator delete[]`: Free memory using kernel's `MemoryAllocator`, providing automatic cleanup for C++ objects

### Thread Class
```cpp
class Thread {
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();
    int start();
    static void dispatch();
    static int sleep(time_t);
protected:
    Thread();
    virtual void run() {}
private:
    thread_t myHandle;
    void (*body)(void*); 
    void* arg;
};
```

**Public Members:**
- `Thread(void (*body)(void*), void* arg)` - Creates thread with function body and argument. Does not start execution automatically.
- `virtual ~Thread()` - Virtual destructor for proper cleanup in derived classes.
- `int start()` - Starts thread execution. Returns `0` on success, negative on failure.
- `static void dispatch()` - Yields processor to another ready thread.
- `static int sleep(time_t)` - Suspends calling thread for specified time. Returns `0` on success, negative on failure.

**Protected Members:**
- `Thread()` - Default constructor for derived classes that override `run()`.
- `virtual void run()` - Virtual method to override in derived classes. Ignored if function pointer provided to constructor.

**Usage Notes:**
- If constructor with function pointer is used, `run()` method is ignored
- Derived classes should either use default constructor and override `run()`, or use parameterized constructor
- Thread objects manage kernel thread lifecycle through RAII

### Semaphore Class
```cpp
class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();
    int wait();
    int signal();
private:
    sem_t myHandle;
};
```

**Public Members:**
- `Semaphore(unsigned init = 1)` - Creates semaphore with initial value (default: 1 for mutex).
- `virtual ~Semaphore()` - Destroys semaphore and unblocks all waiting threads.
- `int wait()` - P (wait) operation. Blocks if value is 0. Returns `0` on success, negative on failure.
- `int signal()` - V (signal) operation. Wakes one waiting thread. Returns `0` on success, negative on failure.

**Usage Notes:**
- Default initial value of 1 makes semaphore usable as binary mutex
- Destructor ensures proper cleanup and thread unblocking
- Errors from system calls propagated via return values

### PeriodicThread Class
```cpp
class PeriodicThread : public Thread {
public:
    void terminate();
protected:
    PeriodicThread(time_t period);
    virtual void periodicActivation() {}
private:
    time_t period;
};
```

Extends `Thread` to provide support for periodic execution patterns.

**Public Members:**
- `void terminate()` - Stops periodic execution and terminates thread.

**Protected Members:**
- `PeriodicThread(time_t period)` - Creates periodic thread with specified period in timer units. Constructor protected for use in derived classes only.
- `virtual void periodicActivation()` - Virtual method to override with periodic work. Called once per period.

**Usage Notes:**
- Must derive from this class and override `periodicActivation()`
- Base class handles sleep/wake cycle automatically
- Use `terminate()` for graceful shutdown

### Console Class
```cpp
class Console {
public:
    static char getc();
    static void putc(char);
};
```

Utility class providing access to console operations.

**Static Members:**
- `static char getc()` - Reads one character from console. Blocks if buffer empty. Returns character or `EOF` on error.
- `static void putc(char)` - Writes character to console output.

## Building and Running

The project uses a `Makefile` for building and execution:

### Build Commands
```bash
# Clean all generated files
make clean

# Compile and link kernel with test application
make all

# Run kernel in QEMU emulator
make qemu

# Run with debugger support (GDB)
make qemu-gdb
```

## Assignment Reference

Course: **Operating systems 1 ([13S112OS1](https://www.etf.bg.ac.rs/en/fis/karton_predmeta/13S112OS1-2013))**  
Academic Year: **2024/2025**  
University of Belgrade, School of Electrical Engineering  
Major: Software Engineering

For complete assignment details, implementation guidelines, and grading criteria, refer to [instructions.pdf](instructions.pdf).

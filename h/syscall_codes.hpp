

#ifndef OS1_RISC_V_PROJECT_SYSCALL_CODES_HPP
#define OS1_RISC_V_PROJECT_SYSCALL_CODES_HPP

// Memory management
#define SYS_MEM_ALLOC                       0x01
#define SYS_MEM_FREE                        0x02
#define SYS_MEM_GET_FREE_SPACE              0x03
#define SYS_MEM_GET_LARGEST_FREE__BLOCK     0x04

// Thread management
#define SYS_THREAD_CREATE                   0x11
#define SYS_THREAD_EXIT                     0x12
#define SYS_THREAD_DISPATCH                 0x13
#define SYS_THREAD_GET_ID                   0x14
#define SYS_THREAD_JOIN                     0x15

// Semaphore management
#define SYS_SEM_OPEN                        0x21
#define SYS_SEM_CLOSE                       0x22
#define SYS_SEM_WAIT                        0x23
#define SYS_SEM_SIGNAL                      0x24

#define SYS_TIME_SLEEP                      0x31

// Console
#define SYS_GETC                            0x41
#define SYS_PUTC                            0x42

#endif //OS1_RISC_V_PROJECT_SYSCALL_CODES_HPP
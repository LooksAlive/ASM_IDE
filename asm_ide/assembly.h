#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#define _GNU_SOURCE

#include <stdbool.h>

#include <unistd.h>       // POSIX API: Standard symbolic constants and types
#include <fcntl.h>        // POSIX API: File control options
#include <sys/stat.h>     // POSIX API: File status
#include <sys/types.h>    // POSIX API: Various types
#include <sys/mman.h>     // POSIX API: Memory management declarations
#include <sys/time.h>     // POSIX API: Time types
#include <sys/resource.h> // POSIX API: Resource limits
#include <sys/ioctl.h>    // POSIX API: I/O control operations
#include <sys/socket.h>   // POSIX API: Socket interface
#include <netinet/in.h>   // POSIX API: Internet address family
#include <arpa/inet.h>    // POSIX API: Functions for manipulating IP addresses
#include <errno.h>        // POSIX API: Error number symbols
#include <sys/uio.h>      // POSIX API: I/O vector operations
#include <signal.h>       // POSIX API: Signal handling
#include <poll.h>         // POSIX API: Polling
#include <sys/syscall.h>  // Linux syscall numbers
#include <sys/wait.h>     // POSIX API: Process termination
#include <sys/epoll.h>    // Linux API: Scalable I/O event notification
#include <sys/fsuid.h>    // Linux API: Filesystem user and group ID manipulation
#include <sys/select.h>   // POSIX API: Synchronous I/O multiplexing
#include <sys/times.h>    // POSIX API: Times and clock operations
#include <sys/utsname.h>  // POSIX API: Operating system name
#include <sys/ptrace.h>   // Linux API: Process tracing
#include <sys/user.h>     // Linux API: User-level API for manipulating process state
#include <sys/vfs.h>      // Linux API: File system information
#include <sys/quota.h>    // Linux API: Disk quota management
#include <sys/xattr.h>    // Linux API: Extended attribute manipulation
#include <sys/klog.h>     // Linux API: Kernel message logging
#include <sys/sendfile.h> // Linux API: Efficient file data transfer
#include <sys/signalfd.h> // Linux API: Signal file descriptor handling
#include <sys/swap.h>     // Linux API: Swap space control
#include <sys/eventfd.h>  // Linux API: Event file descriptor
#include <sys/sysinfo.h>  // Linux API: System information
//#include <sys/capability.h> // Linux API: Process capabilities
//#include <sys/seccomp.h>  // Linux API: Secure computing mode
#include <sys/io.h>       // Linux API: I/O port access
#include <sys/ipc.h>      // POSIX API: Inter-process communication
#include <sys/shm.h>      // POSIX API: Shared memory
#include <sys/sem.h>      // POSIX API: Semaphores
#include <sys/msg.h>      // POSIX API: Message queues
#include <sys/utsname.h>  // POSIX API: Operating system information
#include <netinet/ip.h>   // POSIX API: IP protocol family
#include <netinet/tcp.h>  // POSIX API: TCP protocol family
#include <netinet/udp.h>  // POSIX API: UDP protocol family
#include <netinet/ether.h> // POSIX API: Ethernet protocol family
#include <netinet/ip_icmp.h> // POSIX API: ICMP protocol family
#include <netinet/icmp6.h> // POSIX API: ICMPv6 protocol family
#include <net/if.h>       // POSIX API: Network interface
#include <net/route.h>    // POSIX API: Network routing
#include <netpacket/packet.h> // POSIX API: Packet interface
#include <netdb.h>        // POSIX API: Network database operations
#include <sys/un.h>       // POSIX API: UNIX domain sockets
#include <dirent.h>       // POSIX API: Directory entries
#include <limits.h>       // POSIX API: Implementation-defined constants
#include <math.h>         // POSIX API: Mathematical functions
#include <ctype.h>        // POSIX API: Character classification
#include <locale.h>       // POSIX API: Localization utilities
#include <wchar.h>        // POSIX API: Wide character handling
#include <uchar.h>
#include <wctype.h>       // POSIX API: Wide character classification
#include <stddef.h>       // POSIX API: Standard definitions
#include <stdint.h>       // POSIX API: Integer types
#include <stdio.h>        // POSIX API: Standard I/O operations
#include <stdlib.h>       // POSIX API: Standard library utilities
#include <string.h>       // POSIX API: String operations
#include <stdarg.h>       // POSIX API: Variable arguments
#include <time.h>         // POSIX API: Time utilities
#include <pthread.h>      // POSIX API: Threads
#include <setjmp.h>
#include <stdatomic.h>
#include <threads.h>
#include <iso646.h>
#include <sched.h>

// Additional headers for specific system calls
//#include <sys/sysctl.h>   // sysctl system information interface
#include <sys/prctl.h>    // prctl process control operations
//#include <sys/fs/sysfs.h> // sysfs filesystem information
#include <asm/ldt.h>      // modify_ldt modify local descriptor table
#include <sys/mount.h>    // mount filesystem
#include <sys/reboot.h>   // reboot system call
#include <linux/module.h> // create_module and other module-related calls
#include <sys/prctl.h>    // prctl process control operations
#include <unistd.h>       // setresuid, setresgid, setgroups
#include <sys/wait.h>     // waitpid, waitid, wait3, wait4
#include <sys/time.h>     // gettimeofday, settimeofday
#include <sys/utsname.h>  // uname, uname26
#include <sys/ipc.h>      // IPC set, IPC get (all)
#include <sys/msg.h>      // msgctl, msgget, msgsnd, msgrcv
#include <sys/sem.h>      // semctl, semget, semop
#include <sys/shm.h>      // shmctl, shmget, shmat, shmdt
#include <sys/fsuid.h>    // setfsuid, setfsgid, setreuid, setresuid, setreuid32
//#include <sys/capability.h> // capget, capset, capset32
#include <sys/time.h>     // setitimer, getitimer, adjtimex, adjtimex_time32
#include <sys/resource.h> // getrlimit, setrlimit, prlimit
#include <sys/poll.h>     // poll, ppoll, epoll_create, epoll_ctl, epoll_wait
#include <sys/fcntl.h>    // open, creat, fcntl, ioctl
#include <sys/sysinfo.h>  // sysinfo, ustat, old_sysinfo
#include <sys/ioctl.h>    // ioctl, ioctl_list, ioclt
#include <sys/fcntl.h>    // fcntl, fcntl64, flock, lockf, lockf64

#include <sys/socket.h>   // socket, bind, listen, accept, connect, recv, send, getsockname, getpeername, shutdown
#include <netinet/in.h>   // sockaddr_in, IPPROTO_TCP, IPPROTO_UDP, INADDR_ANY, INADDR_LOOPBACK
#include <arpa/inet.h>    // htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa, inet_pton, inet_ntop
#include <sys/epoll.h>    // epoll_create, epoll_ctl, epoll_wait
//#include <sys/seccomp.h>  // seccomp, prctl(PR_SET_SECCOMP)
#include <sys/io.h>       // ioperm, iopl, inb, inw, inl, outb, outw, outl
#include <sys/prctl.h>    // prctl, PR_SET_NAME, PR_SET_PDEATHSIG, PR_GET_NAME, PR_GET_PDEATHSIG
#include <sys/fsuid.h>    // setfsuid, setfsgid, setreuid, setresuid, setreuid32
//#include <sys/capability.h> // capget, capset, capset32
//#include <sys/ldt.h>      // modify_ldt, get_kernel_syms
//#include <sys/sysctl.h>   // sysctl, sysctlbyname, sysctl
#include <sys/sysinfo.h>  // sysinfo
#include <sys/eventfd.h>  // eventfd, eventfd2
#include <sys/signalfd.h> // signalfd, signalfd4
#include <sys/utsname.h>  // uname, uname26
#include <sys/syslog.h>   // syslog, openlog, closelog
#include <sys/timex.h>    // adjtimex, ntp_adjtime
#include <sys/unistd.h>   // sync, syncfs
#include <sys/wait.h>     // waitpid, waitid, wait3, wait4
#include <sys/time.h>     // gettimeofday, settimeofday
#include <sys/ipc.h>      // IPC set, IPC get (all)
#include <sys/msg.h>      // msgctl, msgget, msgsnd, msgrcv
#include <sys/sem.h>      // semctl, semget, semop
#include <sys/shm.h>      // shmctl, shmget, shmat, shmdt
#include <sys/mount.h>    // mount, umount, umount2
#include <sys/reboot.h>   // reboot
#include <sys/mman.h>     // mmap, munmap, mprotect, msync
#include <sys/resource.h> // getrlimit, setrlimit, prlimit
#include <sys/poll.h>     // poll, ppoll
#include <sys/fcntl.h>    // open, creat, fcntl, ioctl
#include <sys/ioctl.h>    // ioctl, ioctl_list, ioclt
//#include <sys/event.h>    // kqueue, kevent
//#include <sys/disk.h>     // setdisk
#include <sys/param.h>    // CPU_SETSIZE
#include <sys/queue.h>    // LIST_HEAD
#include <sys/user.h>     // getpwuid, getpwnam, getgrouplist

//#include <sys/sysctl.h>   // sysctl system information interface
#include <sys/prctl.h>    // prctl process control operations
//#include <sys/fs/sysfs.h> // sysfs filesystem information
#include <asm/ldt.h>      // modify_ldt modify local descriptor table
#include <sys/mount.h>    // mount filesystem
#include <sys/reboot.h>   // reboot system call
#include <linux/module.h> // create_module and other module-related calls
#include <sys/wait.h>     // waitpid, waitid, wait3, wait4
#include <sys/time.h>     // gettimeofday, settimeofday
#include <sys/utsname.h>  // uname, uname26
#include <sys/ipc.h>      // IPC set, IPC get (all)
#include <sys/msg.h>      // msgctl, msgget, msgsnd, msgrcv
#include <sys/sem.h>      // semctl, semget, semop
#include <sys/shm.h>      // shmctl, shmget, shmat, shmdt
#include <sys/fsuid.h>    // setfsuid, setfsgid, setreuid, setresuid, setreuid32
//#include <sys/capability.h> // capget, capset, capset32
//#include <sys/ldt.h>      // modify_ldt, get_kernel_syms
//#include <sys/sysctl.h>   // sysctl, sysctlbyname, sysctl
#include <sys/sysinfo.h>  // sysinfo
#include <sys/eventfd.h>  // eventfd, eventfd2
#include <sys/signalfd.h> // signalfd, signalfd4
#include <sys/utsname.h>  // uname, uname26
#include <sys/syslog.h>   // syslog, openlog, closelog
#include <sys/timex.h>    // adjtimex, ntp_adjtime
#include <sys/unistd.h>   // sync, syncfs
#include <sys/wait.h>     // waitpid, waitid, wait3, wait4
#include <sys/time.h>     // gettimeofday, settimeofday
#include <sys/ipc.h>      // IPC set, IPC get (all)
#include <sys/msg.h>      // msgctl, msgget, msgsnd, msgrcv
#include <sys/sem.h>      // semctl, semget, semop
#include <sys/shm.h>      // shmctl, shmget, shmat, shmdt
#include <sys/mount.h>    // mount, umount, umount2
#include <sys/reboot.h>   // reboot
#include <sys/mman.h>     // mmap, munmap, mprotect, msync
#include <sys/resource.h> // getrlimit, setrlimit, prlimit
#include <sys/poll.h>     // poll, ppoll
#include <sys/fcntl.h>    // open, creat, fcntl, ioctl
#include <sys/ioctl.h>    // ioctl, ioctl_list, ioclt
//#include <sys/event.h>    // kqueue, kevent
//#include <sys/disk.h>     // setdisk
#include <sys/param.h>    // CPU_SETSIZE
#include <sys/queue.h>    // LIST_HEAD
#include <sys/user.h>     // getpwuid, getpwnam, getgrouplist


typedef unsigned long long ull;

// https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86-32_bit

/*
// Data Movement Instructions
typedef enum DataMovement {
    MOV,    // Move data: MOV dest, src
    PUSH,   // Push data onto stack: PUSH src
    POP,    // Pop data from stack: POP dest
    XCHG,   // Exchange data: XCHG reg1, reg2
    LEA     // Load effective address: LEA dest, [src]
} DataMovement;



// Arithmetic Instructions
typedef enum Arithmetic {
    ADD,    // Add: ADD dest, src
    SUB,    // Subtract: SUB dest, src
    MUL,    // Unsigned multiply: MUL src
    IMUL,   // Signed multiply: IMUL src
    DIV,    // Unsigned divide: DIV src
    IDIV,   // Signed divide: IDIV src
    INC,    // Increment: INC reg
    DEC,    // Decrement: DEC reg
    ADC,    // Add with carry: ADC dest, src
    SBB,    // Subtract with borrow: SBB dest, src
    NEG     // Two's complement negation: NEG reg
} Arithmetic;

// Logical Instructions
typedef enum Logical {
    AND,    // Bitwise AND: AND dest, src
    OR,     // Bitwise OR: OR dest, src
    XOR,    // Bitwise XOR: XOR dest, src
    NOT,    // Bitwise NOT: NOT reg
    SHL,    // Shift left: SHL dest, count
    SHR,    // Shift right: SHR dest, count
    SAR,    // Shift arithmetic right: SAR dest, count
    ROL,    // Rotate left: ROL dest, count
    ROR     // Rotate right: ROR dest, count
} Logical;

// Control Flow Instructions
typedef enum ControlFlow {
    JMP,    // Unconditional jump: JMP label
    JZ,     // Jump if zero: JZ label
    JNZ,    // Jump if not zero: JNZ label
    JG,     // Jump if greater: JG label
    JGE,    // Jump if greater or equal: JGE label
    JL,     // Jump if less: JL label
    JLE,    // Jump if less or equal: JLE label
    JC,     // Jump if carry: JC label
    JNC,    // Jump if not carry: JNC label
    CALL,   // Call procedure: CALL label
    RET,    // Return from procedure: RET
    INT,    // Software interrupt: INT type
    IRET    // Interrupt return: IRET
} ControlFlow;

// String Instructions
typedef enum StringInstructions {
    MOVS,   // Move string data: MOVS dest, src
    CMPS,   // Compare string data: CMPS dest, src
    SCAS,   // Scan string data: SCAS dest
    LODS,   // Load string data: LODS dest
    STOS    // Store string data: STOS dest
} StringInstructions;

// Comparison Instructions
typedef enum Comparison {
    CMP,    // Compare two operands: CMP dest, src
    TEST    // Logical compare: TEST dest, src
} Comparison;

// Stack Management Instructions
typedef enum StackManagement {
    PUSHF,  // Push flags onto stack: PUSHF
    POPF    // Pop flags from stack: POPF
} StackManagement;

// Bit Manipulation Instructions
typedef enum BitManipulation {
    BT,     // Bit test: BT reg, bit
    BSF,    // Bit scan forward: BSF dest, src
    BSR,    // Bit scan reverse: BSR dest, src
    BTS,    // Bit test and set: BTS reg, bit
    BTR,    // Bit test and reset: BTR reg, bit
    BTC     // Bit test and complement: BTC reg, bit
} BitManipulation;

// Miscellaneous Instructions
typedef enum Miscellaneous {
    NOP,    // No operation: NOP
    HLT,    // Halt processor: HLT
    CPUID,  // CPU identification: CPUID
    RDTSC,  // Read time-stamp counter: RDTSC
    RDMSR,  // Read model-specific register: RDMSR
    WRMSR,  // Write model-specific register: WRMSR
    IN,     // Input from port: IN port
    OUT,    // Output to port: OUT port
    CLI,    // Clear interrupt flag: CLI
    STI     // Set interrupt flag: STI
} Miscellaneous;

// Floating-Point Instructions (Basic Set)
typedef enum FloatingPoint {
    FLD,    // Load floating-point value: FLD src
    FST,    // Store floating-point value: FST dest
    FADD,   // Floating-point add: FADD src
    FSUB,   // Floating-point subtract: FSUB src
    FMUL,   // Floating-point multiply: FMUL src
    FDIV,   // Floating-point divide: FDIV src
    FABS,   // Floating-point absolute value: FABS src
    FSQRT,  // Floating-point square root: FSQRT src
    FCOM,   // Floating-point compare: FCOM src
    FINIT   // Initialize floating-point unit: FINIT
} FloatingPoint;

// SSE Instructions (Basic Set)
typedef enum SSEInstructions {
    MOVAPS, // Move aligned packed single-precision values: MOVAPS dest, src
    MOVUPS, // Move unaligned packed single-precision values: MOVUPS dest, src
    ADDPS,  // Add packed single-precision values: ADDPS dest, src
    SUBPS,  // Subtract packed single-precision values: SUBPS dest, src
    MULPS,  // Multiply packed single-precision values: MULPS dest, src
    DIVPS,  // Divide packed single-precision values: DIVPS dest, src
    SQRTPS, // Square roots of packed single-precision values: SQRTPS dest
    MAXPS,  // Maximum of packed single-precision values: MAXPS dest, src
    MINPS   // Minimum of packed single-precision values: MINPS dest, src
} SSEInstructions;
*/


typedef enum __attribute__((packed)) InstructionEnum {
    NONE,
    SYSCALL,// system call: SYSCALL 15  --> constant --> any number represents standard system call,
    LABEL,  // label: LABEL label
    VAR,    // variable -> local: VAR <type ull or basic name> <name> <default value>
    MOV,    // Move data: MOV dest, src
    PUSH,   // Push data onto stack: PUSH src
    POP,    // Pop data from stack: POP dest
    XCHG,   // Exchange data: XCHG reg1, reg2
    LEA,     // Load effective address: LEA dest, [src]

    ADD,    // Add: ADD dest, src
    SUB,    // Subtract: SUB dest, src
    MUL,    // Unsigned multiply: MUL src
    MOD,    // Unsigned modulo: MOD src
    IMUL,   // Signed multiply: IMUL src
    DIV,    // Unsigned divide: DIV src
    IDIV,   // Signed divide: IDIV src
    INC,    // Increment: INC reg
    DEC,    // Decrement: DEC reg
    ADC,    // Add with carry: ADC dest, src
    SBB,    // Subtract with borrow: SBB dest, src
    NEG,     // Two's complement negation: NEG reg

    AND,    // Bitwise AND: AND dest, src
    OR,     // Bitwise OR: OR dest, src
    XOR,    // Bitwise XOR: XOR dest, src
    NOT,    // Bitwise NOT: NOT reg
    SHL,    // Shift left: SHL dest, count
    SHR,    // Shift right: SHR dest, count
    SAR,    // Shift arithmetic right: SAR dest, count
    ROL,    // Rotate left: ROL dest, count
    ROR,     // Rotate right: ROR dest, count

    JMP,    // Unconditional jump: JMP label
    JZ,     // Jump if zero: JZ label
    JNZ,    // Jump if not zero: JNZ label
    JG,     // Jump if greater: JG label
    JGE,    // Jump if greater or equal: JGE label
    JL,     // Jump if less: JL label
    JLE,    // Jump if less or equal: JLE label
    JC,     // Jump if carry: JC label
    JNC,    // Jump if not carry: JNC label
    CALL,   // Call procedure: CALL label
    RET,    // Return from procedure: RET
    INT,    // Software interrupt: INT type
    IRET,    // Interrupt return: IRET

    MOVS,   // Move string data: MOVS dest, src
    CMPS,   // Compare string data: CMPS dest, src
    SCAS,   // Scan string data: SCAS dest
    LODS,   // Load string data: LODS dest
    STOS,    // Store string data: STOS dest

    CMP,    // Compare two operands: CMP dest, src
    TEST,    // Logical compare: TEST dest, src

    PUSHF,  // Push flags onto stack: PUSHF
    POPF,    // Pop flags from stack: POPF

    BT,     // Bit test: BT reg, bit
    BSF,    // Bit scan forward: BSF dest, src
    BSR,    // Bit scan reverse: BSR dest, src
    BTS,    // Bit test and set: BTS reg, bit
    BTR,    // Bit test and reset: BTR reg, bit
    BTC,     // Bit test and complement: BTC reg, bit

    NOP,    // No operation: NOP
    HLT,    // Halt processor: HLT
    CPUID,  // CPU identification: CPUID
    RDTSC,  // Read time-stamp counter: RDTSC
    RDMSR,  // Read model-specific register: RDMSR
    WRMSR,  // Write model-specific register: WRMSR
    IN,     // Input from port: IN port
    OUT,    // Output to port: OUT port
    CLI,    // Clear interrupt flag: CLI
    STI,     // Set interrupt flag: STI

    FLD,    // Load floating-point value: FLD src
    FST,    // Store floating-point value: FST dest
    FADD,   // Floating-point add: FADD src
    FSUB,   // Floating-point subtract: FSUB src
    FMUL,   // Floating-point multiply: FMUL src
    FDIV,   // Floating-point divide: FDIV src
    FABS,   // Floating-point absolute value: FABS src
    FSQRT,  // Floating-point square root: FSQRT src
    FCOM,   // Floating-point compare: FCOM src
    FINIT,   // Initialize floating-point unit: FINIT

    MOVAPS, // Move aligned packed single-precision values: MOVAPS dest, src
    MOVUPS, // Move unaligned packed single-precision values: MOVUPS dest, src
    ADDPS,  // Add packed single-precision values: ADDPS dest, src
    SUBPS,  // Subtract packed single-precision values: SUBPS dest, src
    MULPS,  // Multiply packed single-precision values: MULPS dest, src
    DIVPS,  // Divide packed single-precision values: DIVPS dest, src
    SQRTPS, // Square roots of packed single-precision values: SQRTPS dest
    MAXPS,  // Maximum of packed single-precision values: MAXPS dest, src
    MINPS   // Minimum of packed single-precision values: MINPS dest, src

} InstructionEnum;





/*
typedef enum InstructionType {
    DATAMOVEMENT,
    ARITHMETIC,
    LOGICAL,
    CONTROLFLOW,
    STRINGINSTRUCTION,
    COMPARISON,
    STACKMAGEMENT,
    BITMANIPULATION,
    MISCELLANEOUS,
    FLOATINGPOINT,
    SSEINSTRUCTIONS
} InstructionType;
*/


typedef struct Instruction {
    /*
    InstructionType i_type;
    union {
        DataMovement dm;
        Arithmetic a;
        Logical l;
        ControlFlow cf;
        StringInstructions si;
        Comparison c;
        StackManagement sm;
        BitManipulation bm;
        Miscellaneous m;
        FloatingPoint fp;
        SSEInstructions sse;
    } spec_type;
    */

    InstructionEnum i_type;
    void* data;

    // not serializable !
    ull instruction_start_offset;

} Instruction;

Instruction deserialize_Instruction(FILE* file);  // TODO: deser. by file pointer
size_t serialize_Instruction(const Instruction* instr, unsigned char* buffer);












typedef enum __attribute__((packed)) ObjectEnum {
    OBJ_NONE,
    OBJ_OFFSET,
    OBJ_FUNCTION,
    OBJ_VARIABLE,
    OBJ_STRUCT,
    //OBJ_STRING,
    //OBJ_ARRAY,
    OBJ_ENUM,
    OBJ_MODULE,
} ObjectEnum;

typedef struct Object {
    ObjectEnum type;
    union object {
        ull offset;
        struct Function* function;
        struct Variable* variable;
        struct StructType* struct_type;
        struct EnumType* enum_type;
        struct Module* module;
        //String* string;
        //Array* array;
    } object;
    bool changed;
} Object;

typedef enum __attribute__((packed)) AtomicType {
    BASIC_TYPE_INT,
    BASIC_TYPE_VOID,
    BASIC_TYPE_FLOAT,
    BASIC_TYPE_DOUBLE,
    BASIC_TYPE_CHAR,
    BASIC_TYPELONG,
    BASIC_TYPE_SHORT,
    BASIC_TYPE_UNSIGNED_INT,
    BASIC_TYPE_UNSIGNED_LONG,
    BASIC_TYPE_UNSIGNED_CHAR,
    BASIC_TYPE_POINTER,
    BASIC_TYPE_STRUCT,
    BASIC_TYPE_LONG,
    BASIC_TYPE_NONE
} AtomicType;

char* atomic_type_to_string(AtomicType at);

typedef enum __attribute__((packed)) TypeEnum {
    TYPE_NONE,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_ENTITY,
    TYPE_FUNCTION,
    TYPE_ATOMIC
} TypeEnum;

typedef struct PointerType {
    struct Type* points_to;
    size_t pointer_level;
} PointerType;

char* pointer_type_to_string(PointerType* pt);

char* entity_type_to_string(Object* et);

typedef struct ArrayType {
    struct Type* basic_type;
    size_t array_size;
} ArrayType;

char* array_type_to_string(ArrayType* at);

typedef struct FunctionType {
    Object object;
    struct Type* return_type;
    struct Type* parameters;
    size_t parameters_size;
} FunctionType;

char* function_type_to_string(FunctionType* ft);


// basic structure for a type
typedef struct Type {
    TypeEnum type_kind;
    union {
        PointerType pointer;
        ArrayType array;
        Object entity;  // struct, enum, ...
        FunctionType function;
        AtomicType atomic;
    } type;
} Type;

char* type_to_string(Type* t);


// all have size 8 bytes == 64 bits
typedef struct local_register {
    char* name;
    ull value;
} local_register;


// module type, similar to class but only encapsulates objects
typedef struct Module {
    ull offset; // actual offset on disk
    Object parent;
    char* name;
    Object* fields; // mostly Variable*  TODO: * or **
    size_t fields_size;

} Module;


// Define a structure to represent a struct type
typedef struct StructType {
    bool is_union;
    ull offset; // actual offset on disk
    Object* parent;
    char* name;
    Object** fields; // mostly Variable*  TODO: * or **
    size_t fields_size;
    size_t struct_byte_size; // Total size of the struct
    unsigned int field_count;

    // references ull to child offsets variables
    Object** references_children;
    size_t references_children_size;

    // other objects connected to this struct/class
    Object** references_objects;
    size_t references_objects_size;

    // references ull to child offsets to structs/classes that this one inherits from
    Object** references_parents;
    size_t references_parents_size;

    // references ull to child offsets to structs/classes that this inherits this one
    Object** references_children_parents;
    size_t references_children_parents_size;

} StructType;

// structure that will represent enum
typedef struct EnumType {
    ull offset; // actual offset on disk
    Object* parent;
    char* name;
    Object* values; // Variable*  TODO: * or **
    size_t values_size;
    size_t enum_byte_size; // Total size of the enum

    // references ull to child offsets variables
    Object** references_children;
    size_t references_size;

} EnumType;



// Define a union to represent the value of a variable
typedef union VariableValue {
    int int_val;
    float float_val;
    double double_val;
    char char_val;
    long long_val;
    short short_val;
    unsigned int uint_val;
    unsigned long ulong_val;
    unsigned char uchar_val;
    void* pointer_val;
    void* struct_val; // Pointer to struct data
} VariableValue;

typedef enum __attribute__((packed)) ObjectVisibility {
    PRIVATE,
    PUBLIC,     // by default
    PROTECTED,

} ObjectVisibility;

// global variable
typedef struct Variable {
    ull offset; // actual offset on disk
    Object* parent;
    char* name;
    char is_constant;
    //VariableValue value;
    ObjectVisibility visibility;
    enum {
        BASIC_TYPE_VARIABLE,
        STRUCT_TYPE_VARIABLE
    } vtype;
    union {
        AtomicType type;
        StructType* struct_type; // If the variable is of struct type
    } data;

    VariableValue value;

    // relations to other object they work with, just this abstraction
    Object** relations;
    size_t relations_size;

    // references ull to child offsets variables
    Object** references;
    size_t references_size;

} Variable;

// local variable on stack.
typedef struct LocalVariable {
    ull offset;
    char* name;
    Type type;
    ull address;    // actual address where it is in memory
    bool is_constant;
    bool is_parameter;
    VariableValue value;
} LocalVariable;

typedef struct Parameter {
    Object* variable; // this is not Object because it will always be resolved as variables, not using ull offset here, never.
    char in_out;    // if 0 -> in, if 1 -> out
} Parameter;

// Define a structure to represent a function
typedef struct Function {
    ull offset; // actual offset on disk, start of a function def.
    Object* parent; // module or class
    ObjectVisibility visibility;
    char* name; // ends with \0
    Parameter* parameters; // Variable*  TODO: * or **
    unsigned int param_count;
    Instruction* instructions;
    ull instructions_size;

    Object**  references;     // to calls, usages
    ull     references_size;

} Function;

void append_instruction(Function* func, const Instruction instr);
void insert_instruction(Function* func, const Instruction instr, unsigned int index);
void remove_instruction(Function* func, unsigned int index);
int get_instruction_index_from_offset(Function *func, ull offset);
void init_function(Function* func);

typedef struct SizeofType {
    char isBasic;   // if true use type
    AtomicType basic_type;
    bool is_sa;
    Object structure_access;   // element access -> variable, with parent we can get to the struct
    struct Node* structure_access_node;
} SizeofType;



// push, pop  --> only optional not used very often
//ull* stack_basic;
//ull stack_size_basic;
typedef enum __attribute__((packed)) NodeType {
    L_REG,
    C_VAR,
    TYPE,

    L_VAR,
    G_VAR,
    //FUNCTION,
    CONSTANT,   // ull offset or just constant but of byte ull
    STRUCTOFFSET,
    SIZEOF,
    DEREFERENCE, // [] taking waht is in address,
    INSTRUCTION,    // in case of return from CMP for example. simplification.
    NODE_TYPE_NONE
} NodeType;

typedef struct Node {
    NodeType type;

    union {
        local_register  reg;
        Variable        var;
        //Function        fun;
        ull             offset;
        SizeofType      sizeof_;
        Object structure_access;   // element access -> variable, with parent we can get to the struct;
        Type type;
        struct Node* dereference;   // address take of this node
    } data;
} Node;


typedef struct DataMovement_s {
    Node dest;

    Node src;

    Node src_bytes_to_move;          // MOV L0, L1, 5    ---> 5 bytes, could also be an Node value (sizeof(xxx), structure offset)
} DataMovement_s;

typedef struct Arithmetic_s {
    Node dest;
    Node* src;
    unsigned char src_size;
} Arithmetic_s;

typedef struct Logical_s {
    Node dest;
    Node src;
} Logical_s;

typedef struct Call_s {
    Node dest;
    Object* function;
    Node* parameters;
    unsigned char parameters_size;
} Call_s;

typedef struct call_stack {
    Function* function;
    Variable* variables;    // including c_variables
    size_t variables_size;
    local_register* l_registers;    // local registers to this scope
    size_t l_registers_size;

} call_stack;

// CMP
typedef struct Comparison_s {
    Node operand_left;
    Node operand_right;
    InstructionEnum jump_instruction_type;  // JZ,JG,JL, ...
    Node jump_position;
} Comparison_s;

typedef struct Jump_s {
    Node jump_position;
} Jump_s;




// normal stack storing variables in table
typedef struct stack {
    local_register* globals;
    ull             globals_size;
    call_stack* frames;
    ull             frames_size;

    char* memory;
    ull memory_size;

    ull stack_pointer;
} stack;



typedef struct Range {
    ull start;
    ull end;
} Range;

typedef struct Project {
    ull file_size;

    Object* main_function;

    char** files_paths;      // all data in binary
    size_t files_paths_size;

    ull last_offset;    // offset at which we should append next data.

    Object* opened_objects;     // contains all objects that are opened in this project, so we can easily find them. referencing them all around with pointers
    size_t opened_objects_size;

    Object** top_level_objects; // objects that belongs to this project, mostly modules, structs --> they have associated other objects, building tree structure
    size_t top_level_objects_size;

    char* free_space_file;          // free ranges of bytes that can be filled for each group ^^^
    Range* free_space_file_data;
    size_t free_space_file_size;

    char* project_directory; // New member to store the project directory path

    stack project_stack;

} Project;

void remove_object(Project* proj, Object *obj);

void project_add_opened_object(Project *p, const Object obj);
void project_remove_opened_object(Project *p, const Object *obj);

void init_project(Project* proj, const char* project_name);
void clean_project(Project* proj);
void serialize_project(Project* proj, const char* filename);
void deserialize_project(Project* proj, const char* filename);



void memory_read(stack* s, size_t src, void* buffer, size_t size);
void memory_write(stack* s, size_t dest, void* src, size_t size);

Function *deserialize_function(FILE *f);
size_t serialize_function(Function *fun, unsigned char* buffer);
// function which take FILE* f suppose that position on disk is set to correct position/offset
size_t serialize_variable(const Variable *var, unsigned char* buffer);
Variable* deserialize_variable(FILE *f);
void serialize_struct_type(FILE *f, const StructType *st);
StructType* deserialize_struct_type(FILE *f);
size_t serialize_Node(const Node* node, unsigned char* buffer);
Node deserialize_Node(FILE* file);

void interpret_Instruction(Project *p, Instruction* instr);


void project_remove_opened_object(Project *p, const Object* obj);
void project_add_opened_object(Project *p, const Object obj);

LocalVariable* get_lvar_from_name(Project* p, char* name);

Function* create_function(Project* p, const char* name);

Object *deserialize_object(Project *p, ull offset);
void serialize_object(Project *p, Object *obj);

#endif // ASSEMBLY_H

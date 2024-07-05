#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdint.h>
#include <stddef.h>


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


typedef enum InstructionEnum {
    MOV,    // Move data: MOV dest, src
    PUSH,   // Push data onto stack: PUSH src
    POP,    // Pop data from stack: POP dest
    XCHG,   // Exchange data: XCHG reg1, reg2
    LEA,     // Load effective address: LEA dest, [src]

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

} Instruction;











typedef enum BasicType {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_LONG,
    TYPE_SHORT,
    TYPE_UNSIGNED_INT,
    TYPE_UNSIGNED_LONG,
    TYPE_UNSIGNED_CHAR,
    TYPE_POINTER,
    TYPE_STRUCT
} BasicType;


// all have size 8 bytes == 64 bits
typedef struct local_register {
    char* name;
    ull value;
} local_register;

// Define a structure for a field in a struct
typedef struct Field {
    char* name;
    BasicType type;
    unsigned long long offset; // Offset from the start of the struct
} Field;

// Define a structure to represent a struct type
typedef struct StructType {
    char* name;
    Field* fields;
    unsigned long long size; // Total size of the struct
    unsigned int field_count;
} StructType;

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

// Define a structure to represent a variable
typedef struct Variable {
    char* name;
    BasicType type;
    VariableValue value;
    StructType* struct_type; // If the variable is of struct type
} Variable;


// Define a structure for function parameters
typedef struct Parameter {
    char* name;
    BasicType type;
    //unsigned long long offset; // Offset in the stack frame
} Parameter;

// Define a structure for local variables in a function
typedef struct LocalVariable {
    char* name;
    BasicType type;
    unsigned long long offset; // Offset in the stack frame
} LocalVariable;

// Define a structure to represent a function
typedef struct Function {
    ull offset;
    char visible_globaly;
    char* name;
    Parameter* parameters;
    unsigned int param_count;
    LocalVariable* local_variables;
    unsigned int local_var_count;
    char* bytecode;
    unsigned long long bytecode_size;

    ull*    references;     // to calls
    ull     ref_size;

} Function;



typedef struct SizeofType {
    char isBasic;
    BasicType type;
    ull structure_access[5];
} SizeofType;



// push, pop  --> only optional not used very often
//ull* stack_basic;
//ull stack_size_basic;

typedef struct Node {
    enum NodeType {
        L_REG,
        C_REG,
        L_VAR,
        G_VAR,
        //FUNCTION,
        ULLOFFSET,
        STRUCTOFFSET,
        SIZEOF,
        DEREFERENCE, // [] taking waht is in address
        NODE_TYPE_NONE
    } type;

    union {
        local_register  reg;
        Variable        var;
        //Function        fun;
        ull             offset;
        SizeofType      sizeof_;
        ull structure_access[5];

    } data;
} Node;


typedef struct DataMovement_s {
    Node dest;

    Node src;

    Node src_bytes_to_move;          // MOV L0, 5 L1     ---> 5 bytes, could also be an Node value (sizeof(xxx), structure offset)
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

typedef struct call_stack {
    Function* function;
    Variable* variables;
    local_register* c_registers;    // call passed registers
    local_register* l_registers;    // local registers to this scope

} call_stack;

typedef struct ControlFlow_s {
    enum {
        JMPS,
        CALLC
    } type;

    union {
        Node operand;
        call_stack call;
    } data;
} ControlFlow_s;

typedef struct Comparison_s {
    ControlFlow_s condition;
    Node operand_jump;
} Comparison_s;




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




typedef struct Project {
    ull file_size;

    ull main_function;

    char** files_data_dynamic;      // variables
    size_t files_data_dynamic_size;

    char** files_bss_constants;     // constants, structures, variables, ...
    size_t files_bss_constants_size;

    char** files_functions;         // actual code
    size_t files_functions_size;


    char* free_space_file;          // free ranges of bytes that can be filled for each group ^^^

    char* project_directory; // New member to store the project directory path

    stack project_stack;

} Project;

void init_project(Project* proj, const char* project_name);
void clean_project(Project* proj);
void serialize_project(Project* proj, const char* filename);
void deserialize_project(Project* proj, const char* filename);


void memory_read(stack* s, size_t src, void* buffer, size_t size);
void memory_write(stack* s, size_t dest, void* src, size_t size);


void interpret_Instruction(Project *p, Instruction* instr);






#endif // ASSEMBLY_H

#include "assembly.h"


// Initialize project
void init_project(Project* proj, const char* project_name) {
    proj->file_size = 2 << 15;
    proj->main_function = 0;
    proj->files_data_dynamic = NULL;
    proj->files_data_dynamic_size = 1;
    proj->files_bss_constants = NULL;
    proj->files_bss_constants_size = 1;
    proj->files_functions = NULL;
    proj->files_functions_size = 1;
    proj->free_space_file = NULL;

    proj->project_stack.globals = NULL;
    proj->project_stack.frames = NULL;
    proj->project_stack.memory_size = 1024; // Example size, can be adjusted as needed
    proj->project_stack.memory = malloc(proj->project_stack.memory_size);

    // Create project directory
    proj->project_directory = malloc(strlen(project_name) + 1);
    strcpy(proj->project_directory, project_name);

    if (mkdir(proj->project_directory, 0755) != 0) {
        perror("Error creating project directory");
        free(proj->project_directory);
        return;
    }

    FILE* file;
    char filepath[256];

    // File for data_dynamic
    snprintf(filepath, sizeof(filepath), "%s/data_dynamic.bin", proj->project_directory);
    file = fopen(filepath, "wb");
    if (file != NULL) {
        proj->files_data_dynamic = malloc(sizeof(char*));
        proj->files_data_dynamic[0] = malloc(strlen(filepath) + 1);
        strcpy(proj->files_data_dynamic[0], filepath);
        fclose(file);
    } else {
        perror("Error creating data_dynamic.bin");
    }

    // File for bss_constants
    snprintf(filepath, sizeof(filepath), "%s/bss_constants.bin", proj->project_directory);
    file = fopen(filepath, "wb");
    if (file != NULL) {
        proj->files_bss_constants = malloc(sizeof(char*));
        proj->files_bss_constants[0] = malloc(strlen(filepath) + 1);
        strcpy(proj->files_bss_constants[0], filepath);
        fclose(file);
    } else {
        perror("Error creating bss_constants.bin");
    }

    // File for functions
    snprintf(filepath, sizeof(filepath), "%s/functions.bin", proj->project_directory);
    file = fopen(filepath, "wb");
    if (file != NULL) {
        proj->files_functions = malloc(sizeof(char*));
        proj->files_functions[0] = malloc(strlen(filepath) + 1);
        strcpy(proj->files_functions[0], filepath);
        fclose(file);
    } else {
        perror("Error creating functions.bin");
    }

    // File for free space
    snprintf(filepath, sizeof(filepath), "%s/free_space_file.bin", proj->project_directory);
    file = fopen(filepath, "wb");
    if (file != NULL) {
        proj->free_space_file = malloc(strlen(filepath) + 1);
        strcpy(proj->free_space_file, filepath);
        fclose(file);
    } else {
        perror("Error creating free_space_file.bin");
    }

    // File for project itself
    snprintf(filepath, sizeof(filepath), "%s/project.bin", proj->project_directory);
    file = fopen(filepath, "wb");
    if (file != NULL) {
        serialize_project(proj, filepath);
        fclose(file);
    } else {
        perror("Error creating project.bin");
    }
}

// Clean up project
void clean_project(Project* proj) {
    if (proj->files_data_dynamic) {
        for (size_t i = 0; i < proj->files_data_dynamic_size; ++i) {
            free(proj->files_data_dynamic[i]);
        }
        free(proj->files_data_dynamic);
    }

    if (proj->files_bss_constants) {
        for (size_t i = 0; i < proj->files_bss_constants_size; ++i) {
            free(proj->files_bss_constants[i]);
        }
        free(proj->files_bss_constants);
    }

    if (proj->files_functions) {
        for (size_t i = 0; i < proj->files_functions_size; ++i) {
            free(proj->files_functions[i]);
        }
        free(proj->files_functions);
    }

    free(proj->free_space_file);

    if (proj->project_stack.globals) {
        free(proj->project_stack.globals);
    }
    if (proj->project_stack.frames) {
        free(proj->project_stack.frames);
    }
    if (proj->project_stack.memory) {
        free(proj->project_stack.memory);
    }
}

void serialize_project(Project* proj, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    size_t len = strlen(proj->project_directory) + 1;
    fwrite(&len, sizeof(size_t), 1, file);
    fwrite(proj->project_directory, sizeof(char) * len, len, file);

    fwrite(&proj->file_size, sizeof(ull), 1, file);
    fwrite(&proj->main_function, sizeof(ull), 1, file);

    fwrite(&proj->files_data_dynamic_size, sizeof(size_t), 1, file);
    for (size_t i = 0; i < proj->files_data_dynamic_size; ++i) {
        size_t len = strlen(proj->files_data_dynamic[i]) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(proj->files_data_dynamic[i], sizeof(char), len, file);
    }

    fwrite(&proj->files_bss_constants_size, sizeof(size_t), 1, file);
    for (size_t i = 0; i < proj->files_bss_constants_size; ++i) {
        size_t len = strlen(proj->files_bss_constants[i]) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(proj->files_bss_constants[i], sizeof(char), len, file);
    }

    fwrite(&proj->files_functions_size, sizeof(size_t), 1, file);
    for (size_t i = 0; i < proj->files_functions_size; ++i) {
        size_t len = strlen(proj->files_functions[i]) + 1;
        fwrite(&len, sizeof(size_t), 1, file);
        fwrite(proj->files_functions[i], sizeof(char), len, file);
    }

    size_t free_space_file_len = proj->free_space_file ? strlen(proj->free_space_file) + 1 : 0;
    fwrite(&free_space_file_len, sizeof(size_t), 1, file);
    if (proj->free_space_file) {
        fwrite(proj->free_space_file, sizeof(char), free_space_file_len, file);
    }

    fwrite(&proj->project_stack.memory_size, sizeof(ull), 1, file);
    //fwrite(proj->project_stack.memory, sizeof(char), proj->project_stack.memory_size, file);

    fclose(file);
}

void deserialize_project(Project* proj, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file for reading");
        return;
    }

    size_t len;
    fread(&len, sizeof(size_t), 1, file);
    proj->project_directory = malloc(len * sizeof(char));
    fread(proj->project_directory, sizeof(char) * len, len, file);

    fread(&proj->file_size, sizeof(ull), 1, file);
    fread(&proj->main_function, sizeof(ull), 1, file);

    fread(&proj->files_data_dynamic_size, sizeof(size_t), 1, file);
    proj->files_data_dynamic = malloc(proj->files_data_dynamic_size * sizeof(char*));
    for (size_t i = 0; i < proj->files_data_dynamic_size; ++i) {
        size_t len;
        fread(&len, sizeof(size_t), 1, file);
        proj->files_data_dynamic[i] = malloc(len * sizeof(char));
        fread(proj->files_data_dynamic[i], sizeof(char), len, file);
    }

    fread(&proj->files_bss_constants_size, sizeof(size_t), 1, file);
    proj->files_bss_constants = malloc(proj->files_bss_constants_size * sizeof(char*));
    for (size_t i = 0; i < proj->files_bss_constants_size; ++i) {
        size_t len;
        fread(&len, sizeof(size_t), 1, file);
        proj->files_bss_constants[i] = malloc(len * sizeof(char));
        fread(proj->files_bss_constants[i], sizeof(char), len, file);
    }

    fread(&proj->files_functions_size, sizeof(size_t), 1, file);
    proj->files_functions = malloc(proj->files_functions_size * sizeof(char*));
    for (size_t i = 0; i < proj->files_functions_size; ++i) {
        size_t len;
        fread(&len, sizeof(size_t), 1, file);
        proj->files_functions[i] = malloc(len * sizeof(char));
        fread(proj->files_functions[i], sizeof(char), len, file);
    }

    size_t free_space_file_len;
    fread(&free_space_file_len, sizeof(size_t), 1, file);
    if (free_space_file_len > 0) {
        proj->free_space_file = malloc(free_space_file_len * sizeof(char));
        fread(proj->free_space_file, sizeof(char), free_space_file_len, file);
    } else {
        proj->free_space_file = NULL;
    }

    fread(&proj->project_stack.memory_size, sizeof(ull), 1, file);
    proj->project_stack.memory = malloc(proj->project_stack.memory_size * sizeof(char));
    //fread(proj->project_stack.memory, sizeof(char), proj->project_stack.memory_size, file);

    proj->project_stack.globals = NULL;
    proj->project_stack.frames = NULL;

    fclose(file);
}







// Function to read a block of memory of a specific size from a specific address
void memory_read(stack* s, size_t src, void* buffer, size_t size) {
    if (s == NULL || s->memory == NULL) {
        fprintf(stderr, "Stack or memory not initialized\n");
        return; // Or handle error appropriately
    }

    if (src >= 0 && src + size <= s->memory_size) {
        // Copy 'size' bytes from memory starting at 'address' into 'buffer'
        memcpy(buffer, s->memory + src, size);
    } else {
        fprintf(stderr, "Invalid memory address or size\n");
        // Or handle error appropriately
    }
}

// Function to write a block of memory of a specific size to a specific address
void memory_write(stack* s, size_t dest, void* src, size_t size) {
    if (s == NULL || s->memory == NULL) {
        fprintf(stderr, "Stack or memory not initialized\n");
        return; // Or handle error appropriately
    }

    if (dest >= 0 && dest + size <= s->memory_size) {
        // Copy 'size' bytes from 'buffer' into memory starting at 'address'
        memcpy(s->memory + dest, src, size);
    } else {
        fprintf(stderr, "Invalid memory address or size\n");
        // Or handle error appropriately
    }
}


ull getInstructionSize(const Instruction* instr) {
    // Replace with actual logic to determine the size of an instruction
    return sizeof(Instruction);
}


void append_instruction(Function* func, const Instruction instr) {
    // Increment instructions size
    func->instructions_size++;

    // Allocate or reallocate memory for instructions
    func->instructions = realloc(func->instructions, func->instructions_size * sizeof(Instruction));

    // Check if reallocation failed
    if (func->instructions == NULL) {
        fprintf(stderr, "Failed to allocate memory for instructions.\n");
        exit(EXIT_FAILURE);  // Or handle error as appropriate
    }

    // Append the new instruction to the end of the array
    func->instructions[func->instructions_size - 1] = instr;
}

void adjust_addresses(Function* func, ull code_start_offset) {
    ull* to_update[50]; char num_to_update = 0;

    short isize = getInstructionSize(&func->instructions[0]);
    func->instructions[0].instruction_start_offset = isize + code_start_offset;

    for (unsigned int i = 1; i < func->instructions_size; ++i) {
        Instruction* instr = &func->instructions[i];
        if (instr->i_type == JMP) {
            //JumpInstruction* jump_instr = (JumpInstruction*)instr->data;
            // Adjust jump_instr->target_offset based on the new instruction offsets
            // For example, if the target instruction is moved, update the target_offset accordingly
        } else if (instr->i_type == CALL) {
            //CallInstruction* call_instr = (CallInstruction*)instr->data;
            // Adjust call_instr->call_address based on the new instruction offsets
            // Similar logic to update the address
        } else if (instr->i_type == CMP) {
            //CallInstruction* call_instr = (CallInstruction*)instr->data;
            // Adjust call_instr->call_address based on the new instruction offsets
            // Similar logic to update the address
        }

        ull* to_update_[10]; char num_to_update_ = 0; int update_or_not = 1;
        for(int y = 0; y < num_to_update; y++) {
            if(*to_update[y] == func->instructions[i].instruction_start_offset) {
                to_update_[num_to_update_] = to_update[y];
                num_to_update_++;
                update_or_not = 1;
            }
        }
        if(update_or_not == 1) {
            for(int y = 0; y < num_to_update_; y++) {
                func->instructions[i].instruction_start_offset = func->instructions[i - 1].instruction_start_offset + getInstructionSize(&func->instructions[i - 1]);
                *to_update_[y] = func->instructions[i].instruction_start_offset;
            }
        }
        else {
            // regular instruction
            func->instructions[i].instruction_start_offset = func->instructions[i - 1].instruction_start_offset + getInstructionSize(&func->instructions[i - 1]);
        }
    }
}

void insert_instruction(Function* func, const Instruction instr, unsigned int index) {
    if (index > func->instructions_size) {
        fprintf(stderr, "Index out of bounds for inserting instruction.\n");
        return;
    }

    func->instructions_size++;
    func->instructions = realloc(func->instructions, func->instructions_size * sizeof(Instruction));

    if (func->instructions == NULL) {
        fprintf(stderr, "Failed to allocate memory for instructions.\n");
        exit(EXIT_FAILURE);
    }

    memmove(&func->instructions[index + 1], &func->instructions[index],
            (func->instructions_size - index - 1) * sizeof(Instruction));

    func->instructions[index] = instr;

    adjust_addresses(func, 100);    // TODO: ...
}

void remove_instruction(Function* func, unsigned int index) {
    if (index >= func->instructions_size) {
        fprintf(stderr, "Error: Invalid index to remove instruction: %u\n", index);
        return;
    }

    for (unsigned int i = index; i < func->instructions_size - 1; ++i) {
        func->instructions[i] = func->instructions[i + 1];
    }

    func->instructions_size--;
    func->instructions = realloc(func->instructions, func->instructions_size * sizeof(Instruction));

    if (func->instructions == NULL && func->instructions_size > 0) {
        fprintf(stderr, "Error: Failed to reallocate memory for instructions.\n");
        exit(EXIT_FAILURE);
    }

    adjust_addresses(func, 100);
}




void init_function(Function* func) {
    if (func == NULL) {
        return;
    }

    func->offset = 0;
    func->visibility = 0;
    func->name = NULL;
    func->parameters = NULL;
    func->param_count = 0;
    //func->local_variables = NULL;
    //func->local_var_count = 0;
    func->bytecode = NULL;
    func->bytecode_size = 0;
    func->references = NULL;
    func->ref_size = 0;
    func->instructions = NULL;
    func->instructions_size = 0;
}

// Serialization function for Instruction
size_t serialize_Instruction(const Instruction* instr, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize InstructionEnum
    memcpy(buffer + offset, &(instr->i_type), sizeof(InstructionEnum));
    offset += sizeof(InstructionEnum);

    // Serialize data based on InstructionEnum
    switch (instr->i_type) {
        case MOV: {
            // Example serialization for MOV instruction data (DataMovement_s)
            DataMovement_s* mov_data = (DataMovement_s*)(instr->data);
            // Serialize each field of DataMovement_s as needed
            // Example: serialize_Node(&(mov_data->dest), buffer + offset);
            // offset += serialized_size_of_Node(&(mov_data->dest));
            break;
        }
        // Add cases for other InstructionEnum types as needed

        default:
            fprintf(stderr, "Unsupported InstructionEnum type: %d\n", instr->i_type);
            return 0; // Return 0 to indicate failure
    }

    return offset;
}

// Deserialization function for Instruction
Instruction* deserialize_Instruction(const unsigned char* buffer) {
    Instruction* instr = (Instruction*)malloc(sizeof(Instruction));
    if (!instr) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize InstructionEnum
    memcpy(&(instr->i_type), buffer + offset, sizeof(InstructionEnum));
    offset += sizeof(InstructionEnum);

    // Allocate memory for data based on InstructionEnum
    switch (instr->i_type) {
        case MOV: {
            // Example deserialization for MOV instruction data (DataMovement_s)
            DataMovement_s* mov_data = (DataMovement_s*)malloc(sizeof(DataMovement_s));
            if (!mov_data) {
                fprintf(stderr, "Memory allocation failed.\n");
                free(instr);
                return NULL;
            }
            // Deserialize each field of DataMovement_s as needed
            // Example: deserialize_Node(buffer + offset, &(mov_data->dest));
            // offset += serialized_size_of_Node(&(mov_data->dest));
            instr->data = (void*)mov_data;
            break;
        }
        // Add cases for other InstructionEnum types as needed

        default:
            fprintf(stderr, "Unsupported InstructionEnum type during deserialization: %d\n", instr->i_type);
            free(instr);
            return NULL;
    }

    return instr;
}

// Serialization function for SizeofType
size_t serialize_SizeofType(const SizeofType* stype, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize isBasic flag
    memcpy(buffer + offset, &(stype->isBasic), sizeof(char));
    offset += sizeof(char);

    // Serialize BasicType enum
    memcpy(buffer + offset, &(stype->type), sizeof(BasicType));
    offset += sizeof(BasicType);

    // Serialize structure_access array
    memcpy(buffer + offset, stype->structure_access, sizeof(unsigned long long) * 5);
    offset += sizeof(unsigned long long) * 5;

    return offset;
}

// Deserialization function for SizeofType
SizeofType* deserialize_SizeofType(const unsigned char* buffer) {
    SizeofType* stype = (SizeofType*)malloc(sizeof(SizeofType));
    if (!stype) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize isBasic flag
    memcpy(&(stype->isBasic), buffer + offset, sizeof(char));
    offset += sizeof(char);

    // Deserialize BasicType enum
    memcpy(&(stype->type), buffer + offset, sizeof(BasicType));
    offset += sizeof(BasicType);

    // Deserialize structure_access array
    memcpy(stype->structure_access, buffer + offset, sizeof(unsigned long long) * 5);
    offset += sizeof(unsigned long long) * 5;

    return stype;
}


// Serialization function for Node
size_t serialize_Node(const Node* node, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize NodeType enum
    memcpy(buffer + offset, &(node->type), sizeof(enum NodeType));
    offset += sizeof(enum NodeType);

    // Serialize union data based on NodeType
    size_t name_len;
    switch (node->type) {
        case L_REG:
            // Example serialization for local_register
            // Serialize name (assuming it's a string terminated with '\0')
            name_len = strlen(node->data.reg.name);
            memcpy(buffer + offset, &(name_len), sizeof(size_t));
            offset += sizeof(size_t);
            memcpy(buffer + offset, node->data.reg.name, name_len);
            offset += name_len;
            // Serialize value
            memcpy(buffer + offset, &(node->data.reg.value), sizeof(unsigned long long));
            offset += sizeof(unsigned long long);
            break;
        case C_VAR:
            // Example serialization for Variable
            memcpy(buffer + offset, &(node->data.var.offset), sizeof(unsigned long long));
            offset += sizeof(unsigned long long);
            // Serialize other fields of Variable as needed
            break;
        case CONSTANT:
            // Example serialization for constant (offset)
            memcpy(buffer + offset, &(node->data.offset), sizeof(unsigned long long));
            offset += sizeof(unsigned long long);
            break;
        case SIZEOF:
            // Example serialization for SizeofType
            // Serialize isBasic flag
            memcpy(buffer + offset, &(node->data.sizeof_.isBasic), sizeof(char));
            offset += sizeof(char);
            // Serialize structure_access array
            memcpy(buffer + offset, node->data.sizeof_.structure_access, sizeof(unsigned long long) * 5);
            offset += sizeof(unsigned long long) * 5;
            break;
        // Add cases for other NodeType values as needed

        default:
            fprintf(stderr, "Unsupported NodeType type: %d\n", node->type);
            return 0; // Return 0 to indicate failure
    }

    return offset;
}

// Deserialization function for Node
Node* deserialize_Node(const unsigned char* buffer) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize NodeType enum
    memcpy(&(node->type), buffer + offset, sizeof(enum NodeType));
    offset += sizeof(enum NodeType);

    // Deserialize union data based on NodeType
    size_t name_len;
    switch (node->type) {
        case L_REG:
            // Example deserialization for local_register
            // Deserialize name length
            memcpy(&(name_len), buffer + offset, sizeof(size_t));
            offset += sizeof(size_t);
            // Allocate memory for name and deserialize
            node->data.reg.name = (char*)malloc(name_len + 1); // +1 for '\0'
            if (!node->data.reg.name) {
                fprintf(stderr, "Memory allocation failed.\n");
                free(node);
                return NULL;
            }
            memcpy(node->data.reg.name, buffer + offset, name_len);
            node->data.reg.name[name_len] = '\0'; // Ensure null termination
            offset += name_len;
            // Deserialize value
            memcpy(&(node->data.reg.value), buffer + offset, sizeof(unsigned long long));
            offset += sizeof(unsigned long long);
            break;
        case C_VAR:
            // Example deserialization for Variable
            memcpy(&(node->data.var.offset), buffer + offset, sizeof(unsigned long long));
            offset += sizeof(unsigned long long);
            // Deserialize other fields of Variable as needed
            break;
        case CONSTANT:
            // Example deserialization for constant (offset)
            memcpy(&(node->data.offset), buffer + offset, sizeof(unsigned long long));
            offset += sizeof(unsigned long long);
            break;
        case SIZEOF:
            // Example deserialization for SizeofType
            // Deserialize isBasic flag
            memcpy(&(node->data.sizeof_.isBasic), buffer + offset, sizeof(char));
            offset += sizeof(char);
            // Deserialize structure_access array
            memcpy(node->data.sizeof_.structure_access, buffer + offset, sizeof(unsigned long long) * 5);
            offset += sizeof(unsigned long long) * 5;
            break;
        // Add cases for other NodeType values as needed

        default:
            fprintf(stderr, "Unsupported NodeType type during deserialization: %d\n", node->type);
            free(node);
            return NULL;
    }

    return node;
}

// Serialization function for DataMovement_s
size_t serialize_DataMovement(const DataMovement_s* movement, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize dest Node
    // Replace with serialization function for Node if available
    // Example serialization (assuming Node is a fixed size structure)
    memcpy(buffer + offset, &(movement->dest), sizeof(Node));
    offset += sizeof(Node);

    // Serialize src Node
    // Replace with serialization function for Node if available
    memcpy(buffer + offset, &(movement->src), sizeof(Node));
    offset += sizeof(Node);

    // Serialize src_bytes_to_move Node (example additional field)
    // Replace with serialization function for Node if available
    memcpy(buffer + offset, &(movement->src_bytes_to_move), sizeof(Node));
    offset += sizeof(Node);

    return offset;
}

// Deserialization function for DataMovement_s
DataMovement_s* deserialize_DataMovement(const unsigned char* buffer) {
    DataMovement_s* movement = (DataMovement_s*)malloc(sizeof(DataMovement_s));
    if (!movement) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize dest Node
    // Replace with deserialization function for Node if available
    // Example deserialization (assuming Node is a fixed size structure)
    memcpy(&(movement->dest), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    // Deserialize src Node
    // Replace with deserialization function for Node if available
    memcpy(&(movement->src), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    // Deserialize src_bytes_to_move Node (example additional field)
    // Replace with deserialization function for Node if available
    memcpy(&(movement->src_bytes_to_move), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    return movement;
}

// Serialization function for Arithmetic_s
size_t serialize_Arithmetic(const Arithmetic_s* arithmetic, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize dest Node
    // Replace with serialization function for Node if available
    // Example serialization (assuming Node is a fixed size structure)
    memcpy(buffer + offset, &(arithmetic->dest), sizeof(Node));
    offset += sizeof(Node);

    // Serialize src_size
    memcpy(buffer + offset, &(arithmetic->src_size), sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // Serialize src Node (assuming src is a pointer)
    // Example serialization (assuming src is not NULL)
    if (arithmetic->src) {
        memcpy(buffer + offset, arithmetic->src, sizeof(Node));
        offset += sizeof(Node);
    }

    return offset;
}

// Deserialization function for Arithmetic_s
Arithmetic_s* deserialize_Arithmetic(const unsigned char* buffer) {
    Arithmetic_s* arithmetic = (Arithmetic_s*)malloc(sizeof(Arithmetic_s));
    if (!arithmetic) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize dest Node
    // Replace with deserialization function for Node if available
    // Example deserialization (assuming Node is a fixed size structure)
    memcpy(&(arithmetic->dest), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    // Deserialize src_size
    memcpy(&(arithmetic->src_size), buffer + offset, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // Deserialize src Node (assuming src is a pointer)
    // Example deserialization (assuming src is not NULL)
    if (arithmetic->src) {
        arithmetic->src = (Node*)malloc(sizeof(Node));
        memcpy(arithmetic->src, buffer + offset, sizeof(Node));
        offset += sizeof(Node);
    }

    return arithmetic;
}

// Serialization function for Logical_s
size_t serialize_Logical(const Logical_s* logical, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize dest Node
    // Replace with serialization function for Node if available
    // Example serialization (assuming Node is a fixed size structure)
    memcpy(buffer + offset, &(logical->dest), sizeof(Node));
    offset += sizeof(Node);

    // Serialize src Node
    // Replace with serialization function for Node if available
    memcpy(buffer + offset, &(logical->src), sizeof(Node));
    offset += sizeof(Node);

    return offset;
}

// Deserialization function for Logical_s
Logical_s* deserialize_Logical(const unsigned char* buffer) {
    Logical_s* logical = (Logical_s*)malloc(sizeof(Logical_s));
    if (!logical) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize dest Node
    // Replace with deserialization function for Node if available
    // Example deserialization (assuming Node is a fixed size structure)
    memcpy(&(logical->dest), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    // Deserialize src Node
    // Replace with deserialization function for Node if available
    memcpy(&(logical->src), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    return logical;
}

// Serialization function for Comparison_s
size_t serialize_Comparison(const Comparison_s* comparison, unsigned char* buffer) {
    size_t offset = 0;

    // Serialize operand_left Node
    // Replace with serialization function for Node if available
    // Example serialization (assuming Node is a fixed size structure)
    memcpy(buffer + offset, &(comparison->operand_left), sizeof(Node));
    offset += sizeof(Node);

    // Serialize operand_right Node
    // Replace with serialization function for Node if available
    memcpy(buffer + offset, &(comparison->operand_right), sizeof(Node));
    offset += sizeof(Node);

    // Serialize jump_instruction_type
    memcpy(buffer + offset, &(comparison->jump_instruction_type), sizeof(InstructionEnum));
    offset += sizeof(InstructionEnum);

    // Serialize jump_position Node
    // Replace with serialization function for Node if available
    memcpy(buffer + offset, &(comparison->jump_position), sizeof(Node));
    offset += sizeof(Node);

    return offset;
}

// Deserialization function for Comparison_s
Comparison_s* deserialize_Comparison(const unsigned char* buffer) {
    Comparison_s* comparison = (Comparison_s*)malloc(sizeof(Comparison_s));
    if (!comparison) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    size_t offset = 0;

    // Deserialize operand_left Node
    // Replace with deserialization function for Node if available
    // Example deserialization (assuming Node is a fixed size structure)
    memcpy(&(comparison->operand_left), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    // Deserialize operand_right Node
    // Replace with deserialization function for Node if available
    memcpy(&(comparison->operand_right), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    // Deserialize jump_instruction_type
    memcpy(&(comparison->jump_instruction_type), buffer + offset, sizeof(InstructionEnum));
    offset += sizeof(InstructionEnum);

    // Deserialize jump_position Node
    // Replace with deserialization function for Node if available
    memcpy(&(comparison->jump_position), buffer + offset, sizeof(Node));
    offset += sizeof(Node);

    return comparison;
}

void serialize_struct_type(FILE *f, const StructType *st) {
    fwrite(&st->offset, sizeof(ull), 1, f);

    size_t name_len = strlen(st->name);
    fwrite(&name_len, sizeof(size_t), 1, f);
    fwrite(st->name, sizeof(char), name_len, f);

    fwrite(&st->fields_size, sizeof(size_t), 1, f);
    fwrite(&st->struct_byte_size, sizeof(size_t), 1, f);
    fwrite(&st->field_count, sizeof(unsigned int), 1, f);

    for (unsigned int i = 0; i < st->field_count; i++) {
        serialize_variable(f, &st->fields[i]);
    }
}

StructType* deserialize_struct_type(FILE *f) {
    StructType *st = malloc(sizeof(StructType));

    fread(&st->offset, sizeof(ull), 1, f);

    size_t name_len;
    fread(&name_len, sizeof(size_t), 1, f);
    st->name = malloc(name_len + 1);
    fread(st->name, sizeof(char), name_len, f);
    st->name[name_len] = '\0';

    fread(&st->fields_size, sizeof(size_t), 1, f);
    fread(&st->struct_byte_size, sizeof(size_t), 1, f);
    fread(&st->field_count, sizeof(unsigned int), 1, f);

    st->fields = malloc(st->field_count * sizeof(Variable));
    for (unsigned int i = 0; i < st->field_count; i++) {
        st->fields[i] = *deserialize_variable(f);
    }

    return st;
}




void serialize_variable(FILE *f, const Variable *var) {
    fwrite(&var->offset, sizeof(ull), 1, f);

    size_t name_len = strlen(var->name);
    fwrite(&name_len, sizeof(size_t), 1, f);
    fwrite(var->name, sizeof(char), name_len, f);

    fwrite(&var->visibility, sizeof(ObjectVisibility), 1, f);
    fwrite(&var->vtype, sizeof(var->vtype), 1, f);

    if (var->vtype == BACIS_TYPE_VARUALBE) {
        fwrite(&var->data.type, sizeof(BasicType), 1, f);
    } else if (var->vtype == STRUCT_TYPE_VARIABLE) {
        serialize_struct_type(f, var->data.struct_type);
    }
}

Variable* deserialize_variable(FILE *f) {
    Variable *var = malloc(sizeof(Variable));

    fread(&var->offset, sizeof(ull), 1, f);

    size_t name_len;
    fread(&name_len, sizeof(size_t), 1, f);
    var->name = malloc(name_len + 1);
    fread(var->name, sizeof(char), name_len, f);
    var->name[name_len] = '\0';

    fread(&var->visibility, sizeof(ObjectVisibility), 1, f);
    fread(&var->vtype, sizeof(var->vtype), 1, f);

    if (var->vtype == BACIS_TYPE_VARUALBE) {
        fread(&var->data.type, sizeof(BasicType), 1, f);
    } else if (var->vtype == STRUCT_TYPE_VARIABLE) {
        var->data.struct_type = deserialize_struct_type(f);
    }

    return var;
}





void serialize_function(Project *p, Function* fun) {
    char* function_file = p->files_functions[0]; // Assuming single file for simplicity
    FILE* f = fopen(function_file, "r+b");

    if (!f) {
        perror("Failed to open file");
        return;
    }

    fseek(f, fun->offset, SEEK_SET);

    fwrite(&fun->offset, sizeof(ull), 1, f);
    fwrite(&fun->parent, sizeof(ull), 1, f);
    fwrite(&fun->visibility, sizeof(ObjectVisibility), 1, f);

    size_t name_len = strlen(fun->name);
    fwrite(&name_len, sizeof(size_t), 1, f);
    fwrite(fun->name, sizeof(char), name_len, f);

    fwrite(&fun->param_count, sizeof(unsigned int), 1, f);
    fwrite(fun->parameters, sizeof(Variable), fun->param_count, f);

    fwrite(&fun->bytecode_size, sizeof(ull), 1, f);
    fwrite(fun->bytecode, sizeof(char), fun->bytecode_size, f);

    fwrite(&fun->ref_size, sizeof(ull), 1, f);
    fwrite(fun->references, sizeof(ull), fun->ref_size, f);

    fclose(f);
}

Function* deserialize_function(Project *p, ull offset) {
    Function* fun = malloc(sizeof(Function));
    char* function_file = p->files_functions[0]; // Assuming single file for simplicity
    FILE* f = fopen(function_file, "rb");

    if (!f) {
        perror("Failed to open file");
        free(fun);
        return NULL;
    }

    fseek(f, offset, SEEK_SET);

    fread(&fun->offset, sizeof(ull), 1, f);
    fread(&fun->parent, sizeof(ull), 1, f);
    fread(&fun->visibility, sizeof(ObjectVisibility), 1, f);

    size_t name_len;
    fread(&name_len, sizeof(size_t), 1, f);
    fun->name = malloc(name_len + 1);
    fread(fun->name, sizeof(char), name_len, f);
    fun->name[name_len] = '\0';

    fread(&fun->param_count, sizeof(unsigned int), 1, f);
    fun->parameters = malloc(fun->param_count * sizeof(Variable));
    fread(fun->parameters, sizeof(Variable), fun->param_count, f);

    fread(&fun->bytecode_size, sizeof(ull), 1, f);
    fun->bytecode = malloc(fun->bytecode_size);
    fread(fun->bytecode, sizeof(char), fun->bytecode_size, f);

    fread(&fun->ref_size, sizeof(ull), 1, f);
    fun->references = malloc(fun->ref_size * sizeof(ull));
    fread(fun->references, sizeof(ull), fun->ref_size, f);

    fclose(f);
    return fun;
}
/*
// it is seuqnce of bytes always, so we need to update whole function
void update_function(Project *p, const Function* function_old, const Function* function_new) {
    size_t old_size = sizeof(ull) * 2 + sizeof(ObjectVisibility) + sizeof(size_t) + strlen(function_old->name) +
            sizeof(unsigned int) + sizeof(Variable) * function_old->param_count +
            sizeof(ull) + function_old->bytecode_size + sizeof(ull) * function_old->ref_size;

    size_t new_size = sizeof(ull) * 2 + sizeof(ObjectVisibility) + sizeof(size_t) + strlen(function_new->name) +
            sizeof(unsigned int) + sizeof(Variable) * function_new->param_count +
            sizeof(ull) + function_new->bytecode_size + sizeof(ull) * function_new->ref_size;

    if (new_size <= old_size) {
        // Rewrite existing function, pad the rest with zeros
        deserialize_function(p, (Function*)function_new);
        FILE* f = fopen(function_new->name, "r+b");
        fseek(f, function_new->offset + new_size, SEEK_SET);
        for (size_t i = new_size; i < old_size; ++i) {
            fputc(0, f);
        }
        fclose(f);
    } else {
        // Check for free space or move to a new offset (not implemented)
        // For simplicity, this example only handles in-place update
        fprintf(stderr, "Error: new function size exceeds old function size, relocation not implemented.\n");
    }
}
*/


void handle_MOV(Project *p, Instruction* instr) {
    DataMovement_s* dms = (DataMovement_s*)instr->data;

    if(dms->src.type == L_REG || dms->src.type == C_VAR) {
        if(dms->dest.type == L_REG || dms->dest.type == C_VAR) {
            if(dms->src_bytes_to_move.type == NODE_TYPE_NONE) {
                dms->src.data.reg.value = dms->dest.data.reg.value;
            }
            else {
                if(dms->src_bytes_to_move.type == CONSTANT) { // consider rather constant
                    memcpy(&dms->dest.data.reg.value, &dms->src.data.reg.value, dms->src_bytes_to_move.data.offset);
                }
            }
        }
    }
}

void handle_Arithmetic(Project *p, Instruction* instr) {

    switch (instr->i_type) {
    case ADD:
    case SUB:
    case MUL:
    case IMUL:
    case DIV:
    case IDIV:
    case INC:
    case DEC:
    case ADC:
    case SBB:
    case NEG:
        break;
    }
}

void handle_CMP(Project *p, Instruction* instr) {

    switch (instr->i_type) {
    //case JMP:
    case JMP:
    case JG:
    case JL:
    case JC:
    case JGE:
    case JLE:
    case JNC:
    case JNZ:
    case JZ:
        p->project_stack.stack_pointer = ((Node*)instr->data)->data.offset;
        break;
    }
}






void interpret_Instruction(Project *p, Instruction* instr) {
    stack* stack = &p->project_stack;

    switch (instr->i_type) {
    case MOV:
        handle_MOV(p, instr);
        break;
    case ADD:
    case SUB:
    case MUL:
    case IMUL:
    case DIV:
    case IDIV:
    case INC:
    case DEC:
    case ADC:
    case SBB:
    case NEG:
        handle_Arithmetic(p, instr);
        break;
    case JMP:
        stack->stack_pointer = ((Node*)instr->data)->data.offset;
        break;

    case CMP:
        handle_CMP(p, instr);
        break;
    }
}





































#define STACK_SIZE 1024

// Define the stack structure
typedef struct stackc {
    int top;
    void* items[STACK_SIZE];
} stackc;

// Initialize the stack
void init_stack(stackc* s) {
    s->top = -1;
}

// Push an item onto the stack
void push(stackc* s, void* item) {
    if (s->top < STACK_SIZE - 1) {
        s->items[++s->top] = item;
    } else {
        printf("Stack overflow\n");
    }
}

// Pop an item from the stack
void* pop(stackc* s) {
    if (s->top >= 0) {
        return s->items[s->top--];
    } else {
        printf("Stack underflow\n");
        return NULL;
    }
}

// Function to execute system calls


// Function to execute system calls
void execute_syscall(stackc* s, int syscall_id) {
    switch (syscall_id) {
    case 0: { // sys_read
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        char *buf = (char*)pop(s);
        size_t count = (size_t)(uintptr_t)pop(s);
        ssize_t result = read(fd, buf, count);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 1: { // sys_write
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        const char *buf = (const char*)pop(s);
        size_t count = (size_t)(uintptr_t)pop(s);
        ssize_t result = write(fd, buf, count);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 2: { // sys_open
        const char *filename = (const char*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int mode = (int)(uintptr_t)pop(s);
        int result = open(filename, flags, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 3: { // sys_close
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        int result = close(fd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 4: { // sys_stat
        const char *filename = (const char*)pop(s);
        struct stat *statbuf = (struct stat*)pop(s);
        int result = stat(filename, statbuf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 5: { // sys_fstat
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        struct stat *statbuf = (struct stat*)pop(s);
        int result = fstat(fd, statbuf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 6: { // sys_lstat
        const char *filename = (const char*)pop(s);
        struct stat *statbuf = (struct stat*)pop(s);
        int result = lstat(filename, statbuf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 7: { // sys_poll
        struct pollfd *fds = (struct pollfd*)pop(s);
        nfds_t nfds = (nfds_t)(uintptr_t)pop(s);
        int timeout = (int)(uintptr_t)pop(s);
        int result = poll(fds, nfds, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 8: { // sys_lseek
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        unsigned int whence = (unsigned int)(uintptr_t)pop(s);
        off_t result = lseek(fd, offset, whence);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 9: { // sys_mmap
        void *addr = (void*)pop(s);
        size_t length = (size_t)(uintptr_t)pop(s);
        int prot = (int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int fd = (int)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        void *result = mmap(addr, length, prot, flags, fd, offset);
        push(s, result);
        break;
    }
    case 10: { // sys_munmap
        void *addr = (void*)pop(s);
        size_t length = (size_t)(uintptr_t)pop(s);
        int result = munmap(addr, length);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 11: { // sys_brk
        void *addr = (void*)pop(s);
        void *result = brk(addr);
        push(s, result);
        break;
    }
    case 12: { // sys_rt_sigaction
        int signum = (int)(uintptr_t)pop(s);
        const struct sigaction *act = (const struct sigaction*)pop(s);
        struct sigaction *oldact = (struct sigaction*)pop(s);
        int result = sigaction(signum, act, oldact);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 13: { // sys_rt_sigprocmask
        int how = (int)(uintptr_t)pop(s);
        const sigset_t *set = (const sigset_t*)pop(s);
        sigset_t *oldset = (sigset_t*)pop(s);
        int result = sigprocmask(how, set, oldset);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 14: { // sys_ioctl
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        unsigned int cmd = (unsigned int)(uintptr_t)pop(s);
        unsigned long arg = (unsigned long)(uintptr_t)pop(s);
        int result = ioctl(fd, cmd, arg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 15: { // sys_pread64
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        void *buf = (void*)pop(s);
        size_t count = (size_t)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        ssize_t result = pread(fd, buf, count, offset);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 16: { // sys_pwrite64
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        const void *buf = (const void*)pop(s);
        size_t count = (size_t)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        ssize_t result = pwrite(fd, buf, count, offset);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 17: { // sys_readv
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        const struct iovec *iov = (const struct iovec*)pop(s);
        int iovcnt = (int)(uintptr_t)pop(s);
        ssize_t result = readv(fd, iov, iovcnt);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 18: { // sys_writev
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        const struct iovec *iov = (const struct iovec*)pop(s);
        int iovcnt = (int)(uintptr_t)pop(s);
        ssize_t result = writev(fd, iov, iovcnt);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 19: { // sys_access
        const char *pathname = (const char*)pop(s);
        int mode = (int)(uintptr_t)pop(s);
        int result = access(pathname, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 20: { // sys_pipe
        int *pipefd = (int*)pop(s);
        int result = pipe(pipefd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 21: { // sys_select
        int nfds = (int)(uintptr_t)pop(s);
        fd_set *readfds = (fd_set*)pop(s);
        fd_set *writefds = (fd_set*)pop(s);
        fd_set *exceptfds = (fd_set*)pop(s);
        struct timeval *timeout = (struct timeval*)pop(s);
        int result = select(nfds, readfds, writefds, exceptfds, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 22: { // sys_sched_yield
        int result = sched_yield();
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 23: { // sys_mremap
        void *old_address = (void*)pop(s);
        size_t old_size = (size_t)(uintptr_t)pop(s);
        size_t new_size = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        void *new_address = mremap(old_address, old_size, new_size, flags);
        push(s, new_address);
        break;
    }*/
    case 24: { // sys_msync
        void *addr = (void*)pop(s);
        size_t length = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = msync(addr, length, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 25: { // sys_mincore
        void *addr = (void*)pop(s);
        size_t length = (size_t)(uintptr_t)pop(s);
        unsigned char *vec = (unsigned char*)pop(s);
        int result = mincore(addr, length, vec);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 26: { // sys_madvise
        void *addr = (void*)pop(s);
        size_t length = (size_t)(uintptr_t)pop(s);
        int advice = (int)(uintptr_t)pop(s);
        int result = madvise(addr, length, advice);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 27: { // sys_shmget
        key_t key = (key_t)(uintptr_t)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int shmflg = (int)(uintptr_t)pop(s);
        int result = shmget(key, size, shmflg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 28: { // sys_shmat
        int shmid = (int)(uintptr_t)pop(s);
        const void *shmaddr = (const void*)pop(s);
        int shmflg = (int)(uintptr_t)pop(s);
        void *result = shmat(shmid, shmaddr, shmflg);
        push(s, result);
        break;
    }
    case 29: { // sys_shmctl
        int shmid = (int)(uintptr_t)pop(s);
        int cmd = (int)(uintptr_t)pop(s);
        struct shmid_ds *buf = (struct shmid_ds*)pop(s);
        int result = shmctl(shmid, cmd, buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 30: { // sys_dup
        unsigned int oldfd = (unsigned int)(uintptr_t)pop(s);
        int result = dup(oldfd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 31: { // sys_dup2
        unsigned int oldfd = (unsigned int)(uintptr_t)pop(s);
        unsigned int newfd = (unsigned int)(uintptr_t)pop(s);
        int result = dup2(oldfd, newfd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 32: { // sys_pause
        int result = pause();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 33: { // sys_nanosleep
        const struct timespec *req = (const struct timespec*)pop(s);
        struct timespec *rem = (struct timespec*)pop(s);
        int result = nanosleep(req, rem);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 34: { // sys_getitimer
        int which = (int)(uintptr_t)pop(s);
        struct itimerval *curr_value = (struct itimerval*)pop(s);
        int result = getitimer(which, curr_value);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 35: { // sys_alarm
        unsigned int seconds = (unsigned int)(uintptr_t)pop(s);
        unsigned int result = alarm(seconds);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 36: { // sys_setitimer
        int which = (int)(uintptr_t)pop(s);
        const struct itimerval *new_value = (const struct itimerval*)pop(s);
        struct itimerval *old_value = (struct itimerval*)pop(s);
        int result = setitimer(which, new_value, old_value);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 37: { // sys_getpid
        pid_t result = getpid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 38: { // sys_sendfile
        int out_fd = (int)(uintptr_t)pop(s);
        int in_fd = (int)(uintptr_t)pop(s);
        off_t *offset = (off_t*)pop(s);
        size_t count = (size_t)(uintptr_t)pop(s);
        ssize_t result = sendfile(out_fd, in_fd, offset, count);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 39: { // sys_socket
        int domain = (int)(uintptr_t)pop(s);
        int type = (int)(uintptr_t)pop(s);
        int protocol = (int)(uintptr_t)pop(s);
        int result = socket(domain, type, protocol);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 40: { // sys_connect
        int sockfd = (int)(uintptr_t)pop(s);
        const struct sockaddr *addr = (const struct sockaddr*)pop(s);
        socklen_t addrlen = (socklen_t)(uintptr_t)pop(s);
        int result = connect(sockfd, addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 41: { // sys_accept
        int sockfd = (int)(uintptr_t)pop(s);
        struct sockaddr *addr = (struct sockaddr*)pop(s);
        socklen_t *addrlen = (socklen_t*)pop(s);
        int result = accept(sockfd, addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 42: { // sys_sendto
        int sockfd = (int)(uintptr_t)pop(s);
        const void *buf = (const void*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        const struct sockaddr *dest_addr = (const struct sockaddr*)pop(s);
        socklen_t addrlen = (socklen_t)(uintptr_t)pop(s);
        ssize_t result = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 43: { // sys_recvfrom
        int sockfd = (int)(uintptr_t)pop(s);
        void *buf = (void*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        struct sockaddr *src_addr = (struct sockaddr*)pop(s);
        socklen_t *addrlen = (socklen_t*)pop(s);
        ssize_t result = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 44: { // sys_sendmsg
        int sockfd = (int)(uintptr_t)pop(s);
        const struct msghdr *msg = (const struct msghdr*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        ssize_t result = sendmsg(sockfd, msg, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 45: { // sys_recvmsg
        int sockfd = (int)(uintptr_t)pop(s);
        struct msghdr *msg = (struct msghdr*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        ssize_t result = recvmsg(sockfd, msg, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 46: { // sys_shutdown
        int sockfd = (int)(uintptr_t)pop(s);
        int how = (int)(uintptr_t)pop(s);
        int result = shutdown(sockfd, how);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 47: { // sys_bind
        int sockfd = (int)(uintptr_t)pop(s);
        const struct sockaddr *addr = (const struct sockaddr*)pop(s);
        socklen_t addrlen = (socklen_t)(uintptr_t)pop(s);
        int result = bind(sockfd, addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 48: { // sys_listen
        int sockfd = (int)(uintptr_t)pop(s);
        int backlog = (int)(uintptr_t)pop(s);
        int result = listen(sockfd, backlog);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 49: { // sys_getsockname
        int sockfd = (int)(uintptr_t)pop(s);
        struct sockaddr *addr = (struct sockaddr*)pop(s);
        socklen_t *addrlen = (socklen_t*)pop(s);
        int result = getsockname(sockfd, addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 50: { // sys_getpeername
        int sockfd = (int)(uintptr_t)pop(s);
        struct sockaddr *addr = (struct sockaddr*)pop(s);
        socklen_t *addrlen = (socklen_t*)pop(s);
        int result = getpeername(sockfd, addr, addrlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 51: { // sys_socketpair
        int domain = (int)(uintptr_t)pop(s);
        int type = (int)(uintptr_t)pop(s);
        int protocol = (int)(uintptr_t)pop(s);
        int sv[2];
        int result = socketpair(domain, type, protocol, sv);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 52: { // sys_setsockopt
        int sockfd = (int)(uintptr_t)pop(s);
        int level = (int)(uintptr_t)pop(s);
        int optname = (int)(uintptr_t)pop(s);
        const void *optval = (const void*)pop(s);
        socklen_t optlen = (socklen_t)(uintptr_t)pop(s);
        int result = setsockopt(sockfd, level, optname, optval, optlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 53: { // sys_getsockopt
        int sockfd = (int)(uintptr_t)pop(s);
        int level = (int)(uintptr_t)pop(s);
        int optname = (int)(uintptr_t)pop(s);
        void *optval = (void*)pop(s);
        socklen_t *optlen = (socklen_t*)pop(s);
        int result = getsockopt(sockfd, level, optname, optval, optlen);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 54: { // sys_clone
        unsigned long clone_flags = (unsigned long)(uintptr_t)pop(s);
        void *child_stack = (void*)pop(s);
        int *ptid = (int*)pop(s);
        int *ctid = (int*)pop(s);
        unsigned long newtls = (unsigned long)(uintptr_t)pop(s);
        int result = clone(clone_flags, child_stack, ptid, ctid, newtls);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 55: { // sys_fork
        int result = fork();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 56: { // sys_vfork
        int result = vfork();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 57: { // sys_execve
        const char *filename = (const char*)pop(s);
        char *const argv[] = {(char*)pop(s), NULL};
        char *const envp[] = {(char*)pop(s), NULL};
        int result = execve(filename, argv, envp);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 58: { // sys_exit
        int status = (int)(uintptr_t)pop(s);
        _exit(status);
        break;
    }
    case 59: { // sys_wait4
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int *status = (int*)pop(s);
        int options = (int)(uintptr_t)pop(s);
        struct rusage *rusage = (struct rusage*)pop(s);
        pid_t result = wait4(pid, status, options, rusage);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 60: { // sys_kill
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int sig = (int)(uintptr_t)pop(s);
        int result = kill(pid, sig);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 61: { // sys_uname
        struct utsname *buf = (struct utsname*)pop(s);
        int result = uname(buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 62: { // sys_semget
        key_t key = (key_t)(uintptr_t)pop(s);
        int nsems = (int)(uintptr_t)pop(s);
        int semflg = (int)(uintptr_t)pop(s);
        int result = semget(key, nsems, semflg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 63: { // sys_semop
        int semid = (int)(uintptr_t)pop(s);
        struct sembuf *sops = (struct sembuf*)pop(s);
        size_t nsops = (size_t)(uintptr_t)pop(s);
        int result = semop(semid, sops, nsops);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 64: { // sys_semctl
        int semid = (int)(uintptr_t)pop(s);
        int semnum = (int)(uintptr_t)pop(s);
        int cmd = (int)(uintptr_t)pop(s);
        union semun *arg = (union semun*)pop(s);
        int result = semctl(semid, semnum, cmd); // , *arg
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 65: { // sys_shmdt
        const void *shmaddr = (const void*)pop(s);
        int result = shmdt(shmaddr);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 66: { // sys_msgget
        key_t key = (key_t)(uintptr_t)pop(s);
        int msgflg = (int)(uintptr_t)pop(s);
        int result = msgget(key, msgflg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 67: { // sys_msgsnd
        int msqid = (int)(uintptr_t)pop(s);
        const void *msgp = (const void*)pop(s);
        size_t msgsz = (size_t)(uintptr_t)pop(s);
        int msgflg = (int)(uintptr_t)pop(s);
        int result = msgsnd(msqid, msgp, msgsz, msgflg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 68: { // sys_msgrcv
        int msqid = (int)(uintptr_t)pop(s);
        void *msgp = (void*)pop(s);
        size_t msgsz = (size_t)(uintptr_t)pop(s);
        long msgtyp = (long)(uintptr_t)pop(s);
        int msgflg = (int)(uintptr_t)pop(s);
        ssize_t result = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 69: { // sys_msgctl
        int msqid = (int)(uintptr_t)pop(s);
        int cmd = (int)(uintptr_t)pop(s);
        struct msqid_ds *buf = (struct msqid_ds*)pop(s);
        int result = msgctl(msqid, cmd, buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 70: { // sys_fcntl
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        int cmd = (int)(uintptr_t)pop(s);
        unsigned long arg = (unsigned long)(uintptr_t)pop(s);
        int result = fcntl(fd, cmd, arg);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 71: { // sys_flock
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        int operation = (int)(uintptr_t)pop(s);
        //int result = flock(fd, operation);
        //push(s, (void*)(uintptr_t)result);
        break;
    }
    case 72: { // sys_fsync
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        int result = fsync(fd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 73: { // sys_fdatasync
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        int result = fdatasync(fd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 74: { // sys_truncate
        const char *path = (const char*)pop(s);
        off_t length = (off_t)(uintptr_t)pop(s);
        int result = truncate(path, length);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 75: { // sys_ftruncate
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        off_t length = (off_t)(uintptr_t)pop(s);
        int result = ftruncate(fd, length);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 76: { // sys_getdents
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        struct linux_dirent *dirent = (struct linux_dirent*)pop(s);
        unsigned int count = (unsigned int)(uintptr_t)pop(s);
        int result = getdents64(fd, dirent, count);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 77: { // sys_getcwd
        char *buf = (char*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        char *result = getcwd(buf, size);
        push(s, result);
        break;
    }
    case 78: { // sys_chdir
        const char *path = (const char*)pop(s);
        int result = chdir(path);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 79: { // sys_fchdir
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        int result = fchdir(fd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 80: { // sys_rename
        const char *oldpath = (const char*)pop(s);
        const char *newpath = (const char*)pop(s);
        int result = rename(oldpath, newpath);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 81: { // sys_mkdir
        const char *pathname = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int result = mkdir(pathname, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 82: { // sys_rmdir
        const char *pathname = (const char*)pop(s);
        int result = rmdir(pathname);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 83: { // sys_creat
        const char *pathname = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int result = creat(pathname, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 84: { // sys_link
        const char *oldpath = (const char*)pop(s);
        const char *newpath = (const char*)pop(s);
        int result = link(oldpath, newpath);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 85: { // sys_unlink
        const char *pathname = (const char*)pop(s);
        int result = unlink(pathname);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 86: { // sys_symlink
        const char *target = (const char*)pop(s);
        const char *linkpath = (const char*)pop(s);
        int result = symlink(target, linkpath);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 87: { // sys_readlink
        const char *pathname = (const char*)pop(s);
        char *buf = (char*)pop(s);
        size_t bufsiz = (size_t)(uintptr_t)pop(s);
        ssize_t result = readlink(pathname, buf, bufsiz);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 88: { // sys_chmod
        const char *path = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int result = chmod(path, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 89: { // sys_fchmod
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int result = fchmod(fd, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 90: { // sys_chown
        const char *path = (const char*)pop(s);
        uid_t owner = (uid_t)(uintptr_t)pop(s);
        gid_t group = (gid_t)(uintptr_t)pop(s);
        int result = chown(path, owner, group);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 91: { // sys_fchown
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        uid_t owner = (uid_t)(uintptr_t)pop(s);
        gid_t group = (gid_t)(uintptr_t)pop(s);
        int result = fchown(fd, owner, group);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 92: { // sys_lchown
        const char *path = (const char*)pop(s);
        uid_t owner = (uid_t)(uintptr_t)pop(s);
        gid_t group = (gid_t)(uintptr_t)pop(s);
        int result = lchown(path, owner, group);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 93: { // sys_umask
        mode_t mask = (mode_t)(uintptr_t)pop(s);
        mode_t result = umask(mask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 94: { // sys_gettimeofday
        struct timeval *tv = (struct timeval*)pop(s);
        struct timezone *tz = (struct timezone*)pop(s);
        int result = gettimeofday(tv, tz);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 95: { // sys_getrlimit
        int resource = (int)(uintptr_t)pop(s);
        struct rlimit *rlim = (struct rlimit*)pop(s);
        int result = getrlimit(resource, rlim);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 96: { // sys_getrusage
        int who = (int)(uintptr_t)pop(s);
        struct rusage *usage = (struct rusage*)pop(s);
        int result = getrusage(who, usage);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 97: { // sys_sysinfo
        struct sysinfo *info = (struct sysinfo*)pop(s);
        int result = sysinfo(info);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 98: { // sys_times
        struct tms *buf = (struct tms*)pop(s);
        clock_t result = times(buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 99: { // sys_ptrace
        enum __ptrace_request request = (enum __ptrace_request)(uintptr_t)pop(s);
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        void *addr = (void*)pop(s);
        void *data = (void*)pop(s);
        int result = ptrace(request, pid, addr, data);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 100: { // sys_getuid
        uid_t result = getuid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 101: { // sys_syslog
        int type = (int)(uintptr_t)pop(s);
        char *bufp = (char*)pop(s);
        int len = (int)(uintptr_t)pop(s);
        syslog(type, bufp, len);
        //push(s, (void*)(uintptr_t)result);
        break;
    }
    case 102: { // sys_getgid
        gid_t result = getgid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 103: { // sys_setuid
        uid_t uid = (uid_t)(uintptr_t)pop(s);
        int result = setuid(uid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 104: { // sys_setgid
        gid_t gid = (gid_t)(uintptr_t)pop(s);
        int result = setgid(gid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 105: { // sys_geteuid
        uid_t result = geteuid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 106: { // sys_getegid
        gid_t result = getegid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 107: { // sys_setpgid
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        pid_t pgid = (pid_t)(uintptr_t)pop(s);
        int result = setpgid(pid, pgid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 108: { // sys_getppid
        pid_t result = getppid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 109: { // sys_getpgrp
        pid_t result = getpgrp();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 110: { // sys_setsid
        pid_t result = setsid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 111: { // sys_setreuid
        uid_t ruid = (uid_t)(uintptr_t)pop(s);
        uid_t euid = (uid_t)(uintptr_t)pop(s);
        int result = setreuid(ruid, euid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 112: { // sys_setregid
        gid_t rgid = (gid_t)(uintptr_t)pop(s);
        gid_t egid = (gid_t)(uintptr_t)pop(s);
        int result = setregid(rgid, egid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 113: { // sys_getgroups
        int size = (int)(uintptr_t)pop(s);
        gid_t list[] = {(gid_t)(uintptr_t)pop(s)};
        int result = getgroups(size, list);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 114: { // sys_setgroups
        size_t size = (size_t)(uintptr_t)pop(s);
        const gid_t *list = (const gid_t*)pop(s);
        int result = setgroups(size, list);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 115: { // sys_setresuid
        uid_t ruid = (uid_t)(uintptr_t)pop(s);
        uid_t euid = (uid_t)(uintptr_t)pop(s);
        uid_t suid = (uid_t)(uintptr_t)pop(s);
        int result = setresuid(ruid, euid, suid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 116: { // sys_getresuid
        uid_t *ruid = (uid_t*)pop(s);
        uid_t *euid = (uid_t*)pop(s);
        uid_t *suid = (uid_t*)pop(s);
        int result = getresuid(ruid, euid, suid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 117: { // sys_setresgid
        gid_t rgid = (gid_t)(uintptr_t)pop(s);
        gid_t egid = (gid_t)(uintptr_t)pop(s);
        gid_t sgid = (gid_t)(uintptr_t)pop(s);
        int result = setresgid(rgid, egid, sgid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 118: { // sys_getresgid
        gid_t *rgid = (gid_t*)pop(s);
        gid_t *egid = (gid_t*)pop(s);
        gid_t *sgid = (gid_t*)pop(s);
        int result = getresgid(rgid, egid, sgid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 119: { // sys_getpgid
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        pid_t result = getpgid(pid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 120: { // sys_setfsuid
        uid_t uid = (uid_t)(uintptr_t)pop(s);
        int result = setfsuid(uid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 121: { // sys_setfsgid
        gid_t gid = (gid_t)(uintptr_t)pop(s);
        int result = setfsgid(gid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 122: { // sys_getsid
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        pid_t result = getsid(pid);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 123: { // sys_capget
        cap_user_header_t header = (cap_user_header_t)(uintptr_t)pop(s);
        cap_user_data_t data = (cap_user_data_t)(uintptr_t)pop(s);
        int result = capget(header, data);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 124: { // sys_capset
        cap_user_header_t header = (cap_user_header_t)(uintptr_t)pop(s);
        const cap_user_data_t data = (const cap_user_data_t)(uintptr_t)pop(s);
        int result = capset(header, data);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 125: { // sys_rt_sigpending
        sigset_t *set = (sigset_t*)pop(s);
        int result = rt_sigpending(set);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 126: { // sys_rt_sigtimedwait
        const sigset_t *uthese = (const sigset_t*)pop(s);
        siginfo_t *uinfo = (siginfo_t*)pop(s);
        const struct timespec *uts = (const struct timespec*)pop(s);
        size_t sigsetsize = (size_t)(uintptr_t)pop(s);
        int result = rt_sigtimedwait(uthese, uinfo, uts, sigsetsize);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 127: { // sys_rt_sigqueueinfo
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int sig = (int)(uintptr_t)pop(s);
        const siginfo_t *uinfo = (const siginfo_t*)pop(s);
        int result = rt_sigqueueinfo(pid, sig, uinfo);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 128: { // sys_rt_sigsuspend
        const sigset_t *unewset = (const sigset_t*)pop(s);
        size_t sigsetsize = (size_t)(uintptr_t)pop(s);
        int result = rt_sigsuspend(unewset, sigsetsize);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 129: { // sys_sigaltstack
        const struct sigaltstack *uss = (const struct sigaltstack*)pop(s);
        struct sigaltstack *uoss = (struct sigaltstack*)pop(s);
        int result = sigaltstack(uss, uoss);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 130: { // sys_utime
        const char *filename = (const char*)pop(s);
        const struct utimbuf *times = (const struct utimbuf*)pop(s);
        int result = utime(filename, times);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 131: { // sys_mknod
        const char *pathname = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        dev_t dev = (dev_t)(uintptr_t)pop(s);
        int result = mknod(pathname, mode, dev);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 132: { // sys_uselib
        const char *library = (const char*)pop(s);
        int result = uselib(library);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 133: { // sys_personality
        unsigned long personality = (unsigned long)(uintptr_t)pop(s);
        int result = personality(personality);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 134: { // sys_ustat
        dev_t dev = (dev_t)(uintptr_t)pop(s);
        struct ustat *ubuf = (struct ustat*)pop(s);
        int result = ustat(dev, ubuf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 135: { // sys_statfs
        const char *path = (const char*)pop(s);
        struct statfs *buf = (struct statfs*)pop(s);
        int result = statfs(path, buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 136: { // sys_fstatfs
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        struct statfs *buf = (struct statfs*)pop(s);
        int result = fstatfs(fd, buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 137: { // sys_sysfs
        int option = (int)(uintptr_t)pop(s);
        unsigned long arg1 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg2 = (unsigned long)(uintptr_t)pop(s);
        int result = sysfs(option, arg1, arg2);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 138: { // sys_getpriority
        int which = (int)(uintptr_t)pop(s);
        int who = (int)(uintptr_t)pop(s);
        int result = getpriority(which, who);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 139: { // sys_setpriority
        int which = (int)(uintptr_t)pop(s);
        int who = (int)(uintptr_t)pop(s);
        int niceval = (int)(uintptr_t)pop(s);
        int result = setpriority(which, who, niceval);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 140: { // sys_sched_setparam
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        const struct sched_param *param = (const struct sched_param*)pop(s);
        int result = sched_setparam(pid, param);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 141: { // sys_sched_getparam
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        struct sched_param *param = (struct sched_param*)pop(s);
        int result = sched_getparam(pid, param);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 142: { // sys_sched_setscheduler
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int policy = (int)(uintptr_t)pop(s);
        const struct sched_param *param = (const struct sched_param*)pop(s);
        int result = sched_setscheduler(pid, policy, param);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 143: { // sys_sched_getscheduler
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int result = sched_getscheduler(pid);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 144: { // sys_sched_get_priority_max
        int policy = (int)(uintptr_t)pop(s);
        int result = sched_get_priority_max(policy);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 145: { // sys_sched_get_priority_min
        int policy = (int)(uintptr_t)pop(s);
        int result = sched_get_priority_min(policy);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 146: { // sys_sched_rr_get_interval
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        struct timespec *tp = (struct timespec*)pop(s);
        int result = sched_rr_get_interval(pid, tp);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 147: { // sys_mlock
        const void *addr = (const void*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int result = mlock(addr, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 148: { // sys_munlock
        const void *addr = (const void*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int result = munlock(addr, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 149: { // sys_mlockall
        int flags = (int)(uintptr_t)pop(s);
        int result = mlockall(flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 150: { // sys_munlockall
        int result = munlockall();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 151: { // sys_vhangup
        int result = vhangup();
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 152: { // sys_modify_ldt
        int func = (int)(uintptr_t)pop(s);
        void *ptr = (void*)pop(s);
        unsigned long bytecount = (unsigned long)(uintptr_t)pop(s);
        int result = modify_ldt(func, ptr, bytecount);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 153: { // sys_pivot_root
        const char *new_root = (const char*)pop(s);
        const char *put_old = (const char*)pop(s);
        int result = pivot_root(new_root, put_old);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 154: { // sys__sysctl
        struct __sysctl_args *args = (struct __sysctl_args*)pop(s);
        int result = __sysctl(args);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 155: { // sys_prctl
        int option = (int)(uintptr_t)pop(s);
        unsigned long arg2 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg3 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg4 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg5 = (unsigned long)(uintptr_t)pop(s);
        int result = prctl(option, arg2, arg3, arg4, arg5);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 156: { // sys_arch_prctl
        int option = (int)(uintptr_t)pop(s);
        unsigned long arg2 = (unsigned long)(uintptr_t)pop(s);
        int result = arch_prctl(option, arg2);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 157: { // sys_adjtimex
        struct timex *buf = (struct timex*)pop(s);
        int result = adjtimex(buf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 158: { // sys_setrlimit
        int resource = (int)(uintptr_t)pop(s);
        const struct rlimit *rlim = (const struct rlimit*)pop(s);
        int result = setrlimit(resource, rlim);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 159: { // sys_chroot
        const char *filename = (const char*)pop(s);
        int result = chroot(filename);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 160: { // sys_sync
        sync();
        break;
    }
    case 161: { // sys_acct
        const char *filename = (const char*)pop(s);
        int result = acct(filename);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 162: { // sys_settimeofday
        const struct timeval *tv = (const struct timeval*)pop(s);
        const struct timezone *tz = (const struct timezone*)pop(s);
        int result = settimeofday(tv, tz);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 163: { // sys_mount
        const char *src = (const char*)pop(s);
        const char *target = (const char*)pop(s);
        const char *fs = (const char*)pop(s);
        unsigned long flags = (unsigned long)(uintptr_t)pop(s);
        void *data = (void*)pop(s);
        int result = mount(src, target, fs, flags, data);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 164: { // sys_umount2
        const char *target = (const char*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = umount2(target, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 165: { // sys_swapon
        const char *path = (const char*)pop(s);
        int swap_flags = (int)(uintptr_t)pop(s);
        int result = swapon(path, swap_flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 166: { // sys_swapoff
        const char *path = (const char*)pop(s);
        int result = swapoff(path);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 167: { // sys_reboot
        int magic = (int)(uintptr_t)pop(s);
        int result = reboot(magic);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 168: { // sys_sethostname
        const char *name = (const char*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int result = sethostname(name, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 169: { // sys_setdomainname
        const char *name = (const char*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int result = setdomainname(name, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 170: { // sys_iopl
        unsigned int level = (unsigned int)(uintptr_t)pop(s);
        int result = iopl(level);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 171: { // sys_ioperm
        unsigned long from = (unsigned long)(uintptr_t)pop(s);
        unsigned long num = (unsigned long)(uintptr_t)pop(s);
        int on = (int)(uintptr_t)pop(s);
        int result = ioperm(from, num, on);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 172: { // sys_create_module
        const char *name = (const char*)pop(s);
        unsigned int size = (unsigned int)(uintptr_t)pop(s);
        int result = create_module(name, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 173: { // sys_init_module
        void *umod = (void*)pop(s);
        unsigned long len = (unsigned long)(uintptr_t)pop(s);
        const char *uargs = (const char*)pop(s);
        int result = init_module(umod, len, uargs);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 174: { // sys_delete_module
        const char *name = (const char*)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        int result = delete_module(name, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 175: { // sys_get_kernel_syms
        struct kernel_sym *table = (struct kernel_sym*)pop(s);
        int result = get_kernel_syms(table);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 176: { // sys_query_module
        const char *name = (const char*)pop(s);
        int which = (int)(uintptr_t)pop(s);
        void *buf = (void*)pop(s);
        size_t bufsize = (size_t)(uintptr_t)pop(s);
        size_t *ret = (size_t*)pop(s);
        int result = query_module(name, which, buf, bufsize, ret);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 177: { // sys_nfsservctl
        int cmd = (int)(uintptr_t)pop(s);
        struct nfsctl_arg *argp = (struct nfsctl_arg*)pop(s);
        union nfsctl_res *resp = (union nfsctl_res*)pop(s);
        int result = nfsservctl(cmd, argp, resp);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 178: { // sys_getpmsg
        // Not implemented: obsolete call
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 179: { // sys_putpmsg
        // Not implemented: obsolete call
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 180: { // sys_afs_syscall
        // Not implemented: unused in Linux
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 181: { // sys_tuxcall
        // Not implemented: unused in Linux
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 182: { // sys_security
        // Not implemented: Security-related system call
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }/*
    case 183: { // sys_gettid
        pid_t result = gettid();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 184: { // sys_readahead
        int fd = (int)(uintptr_t)pop(s);
        loff_t offset = (loff_t)(uintptr_t)pop(s);
        size_t count = (size_t)(uintptr_t)pop(s);
        int result = readahead(fd, offset, count);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 185: { // sys_setxattr
        const char *path = (const char*)pop(s);
        const char *name = (const char*)pop(s);
        const void *value = (const void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = setxattr(path, name, value, size, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 186: { // sys_lsetxattr
        const char *path = (const char*)pop(s);
        const char *name = (const char*)pop(s);
        const void *value = (const void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = lsetxattr(path, name, value, size, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 187: { // sys_fsetxattr
        int fd = (int)(uintptr_t)pop(s);
        const char *name = (const char*)pop(s);
        const void *value = (const void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = fsetxattr(fd, name, value, size, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 188: { // sys_getxattr
        const char *path = (const char*)pop(s);
        const char *name = (const char*)pop(s);
        void *value = (void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int result = getxattr(path, name, value, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 189: { // sys_lgetxattr
        const char *path = (const char*)pop(s);
        const char *name = (const char*)pop(s);
        void *value = (void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int result = lgetxattr(path, name, value, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 190: { // sys_fgetxattr
        int fd = (int)(uintptr_t)pop(s);
        const char *name = (const char*)pop(s);
        void *value = (void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int result = fgetxattr(fd, name, value, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 191: { // sys_listxattr
        const char *path = (const char*)pop(s);
        char *list = (char*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int result = listxattr(path, list, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 192: { // sys_llistxattr
        const char *path = (const char*)pop(s);
        char *list = (char*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int result = llistxattr(path, list, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 193: { // sys_flistxattr
        int fd = (int)(uintptr_t)pop(s);
        char *list = (char*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int result = flistxattr(fd, list, size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 194: { // sys_removexattr
        const char *path = (const char*)pop(s);
        const char *name = (const char*)pop(s);
        int result = removexattr(path, name);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 195: { // sys_lremovexattr
        const char *path = (const char*)pop(s);
        const char *name = (const char*)pop(s);
        int result = lremovexattr(path, name);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 196: { // sys_fremovexattr
        int fd = (int)(uintptr_t)pop(s);
        const char *name = (const char*)pop(s);
        int result = fremovexattr(fd, name);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 197: { // sys_tkill
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int sig = (int)(uintptr_t)pop(s);
        int result = tkill(pid, sig);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 198: { // sys_time
        time_t *tloc = (time_t*)pop(s);
        time_t result = time(tloc);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 199: { // sys_futex
        int *uaddr = (int*)pop(s);
        int op = (int)(uintptr_t)pop(s);
        int val = (int)(uintptr_t)pop(s);
        const struct timespec *timeout = (const struct timespec*)pop(s);
        int *uaddr2 = (int*)pop(s);
        int val3 = (int)(uintptr_t)pop(s);
        int result = futex(uaddr, op, val, timeout, uaddr2, val3);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 200: { // sys_sched_setaffinity
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        size_t cpusetsize = (size_t)(uintptr_t)pop(s);
        const cpu_set_t *mask = (const cpu_set_t*)pop(s);
        int result = sched_setaffinity(pid, cpusetsize, mask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 201: { // sys_sched_getaffinity
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        size_t cpusetsize = (size_t)(uintptr_t)pop(s);
        cpu_set_t *mask = (cpu_set_t*)pop(s);
        int result = sched_getaffinity(pid, cpusetsize, mask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 202: { // sys_set_thread_area
        struct user_desc *u_info = (struct user_desc*)pop(s);
        int result = set_thread_area(u_info);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 203: { // sys_io_setup
        unsigned nr_reqs = (unsigned)(uintptr_t)pop(s);
        aio_context_t *ctx = (aio_context_t*)pop(s);
        int result = io_setup(nr_reqs, ctx);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 204: { // sys_io_destroy
        aio_context_t ctx = (aio_context_t)(uintptr_t)pop(s);
        int result = io_destroy(ctx);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 205: { // sys_io_getevents
        aio_context_t ctx_id = (aio_context_t)(uintptr_t)pop(s);
        long min_nr = (long)(uintptr_t)pop(s);
        long nr = (long)(uintptr_t)pop(s);
        struct io_event *events = (struct io_event*)pop(s);
        struct timespec *timeout = (struct timespec*)pop(s);
        int result = io_getevents(ctx_id, min_nr, nr, events, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 206: { // sys_io_submit
        aio_context_t ctx_id = (aio_context_t)(uintptr_t)pop(s);
        long nr = (long)(uintptr_t)pop(s);
        struct iocb *ios = (struct iocb*)pop(s);
        int result = io_submit(ctx_id, nr, ios);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 207: { // sys_io_cancel
        aio_context_t ctx_id = (aio_context_t)(uintptr_t)pop(s);
        struct iocb *iocb = (struct iocb*)pop(s);
        struct io_event *result = (struct io_event*)pop(s);
        int result = io_cancel(ctx_id, iocb, result);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 208: { // sys_get_thread_area
        struct user_desc *u_info = (struct user_desc*)pop(s);
        int result = get_thread_area(u_info);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 209: { // sys_lookup_dcookie
        u64 cookie64 = (u64)(uintptr_t)pop(s);
        char *buf = (char*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int result = lookup_dcookie(cookie64, buf, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 210: { // sys_epoll_create
        int size = (int)(uintptr_t)pop(s);
        int result = epoll_create(size);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 211: { // sys_epoll_ctl_old
        // Not implemented: obsolete call
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 212: { // sys_epoll_wait_old
        // Not implemented: obsolete call
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 213: { // sys_remap_file_pages
        void *start = (void*)pop(s);
        size_t size = (size_t)(uintptr_t)pop(s);
        int prot = (int)(uintptr_t)pop(s);
        ssize_t pgoff = (ssize_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = remap_file_pages(start, size, prot, pgoff, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 214: { // sys_getdents64
        unsigned int fd = (unsigned int)(uintptr_t)pop(s);
        struct linux_dirent64 *dirent = (struct linux_dirent64*)pop(s);
        unsigned int count = (unsigned int)(uintptr_t)pop(s);
        int result = getdents64(fd, dirent, count);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 215: { // sys_set_tid_address
        int *tidptr = (int*)pop(s);
        int result = set_tid_address(tidptr);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 216: { // sys_restart_syscall
        int result = restart_syscall();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 217: { // sys_semtimedop
        int semid = (int)(uintptr_t)pop(s);
        struct sembuf *sops = (struct sembuf*)pop(s);
        unsigned nsops = (unsigned)(uintptr_t)pop(s);
        const struct timespec *timeout = (const struct timespec*)pop(s);
        int result = semtimedop(semid, sops, nsops, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 218: { // sys_fadvise64
        int fd = (int)(uintptr_t)pop(s);
        loff_t offset = (loff_t)(uintptr_t)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int advice = (int)(uintptr_t)pop(s);
        int result = fadvise64(fd, offset, len, advice);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 219: { // sys_timer_create
        clockid_t which_clock = (clockid_t)(uintptr_t)pop(s);
        struct sigevent *timer_event_spec = (struct sigevent*)pop(s);
        timer_t * created_timer_id = (timer_t*)pop(s);
        int result = timer_create(which_clock, timer_event_spec, created_timer_id);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 220: { // sys_timer_settime
        timer_t timer_id = (timer_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        const struct itimerspec *new_value = (const struct itimerspec*)pop(s);
        struct itimerspec * old_value = (struct itimerspec*)pop(s);
        int result = timer_settime(timer_id, flags, new_value, old_value);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 221: { // sys_timer_gettime
        timer_t timer_id = (timer_t)(uintptr_t)pop(s);
        struct itimerspec * curr_value = (struct itimerspec*)pop(s);
        int result = timer_gettime(timer_id, curr_value);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 222: { // sys_timer_getoverrun
        timer_t timer_id = (timer_t)(uintptr_t)pop(s);
        int result = timer_getoverrun(timer_id);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 223: { // sys_timer_delete
        timer_t timer_id = (timer_t)(uintptr_t)pop(s);
        int result = timer_delete(timer_id);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 224: { // sys_clock_settime
        clockid_t which_clock = (clockid_t)(uintptr_t)pop(s);
        const struct timespec *tp = (const struct timespec*)pop(s);
        int result = clock_settime(which_clock, tp);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 225: { // sys_clock_gettime
        clockid_t which_clock = (clockid_t)(uintptr_t)pop(s);
        struct timespec *tp = (struct timespec*)pop(s);
        int result = clock_gettime(which_clock, tp);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 226: { // sys_clock_getres
        clockid_t which_clock = (clockid_t)(uintptr_t)pop(s);
        struct timespec *tp = (struct timespec*)pop(s);
        int result = clock_getres(which_clock, tp);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 227: { // sys_clock_nanosleep
        clockid_t which_clock = (clockid_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        const struct timespec *rqtp = (const struct timespec*)pop(s);
        struct timespec *rmtp = (struct timespec*)pop(s);
        int result = clock_nanosleep(which_clock, flags, rqtp, rmtp);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 228: { // sys_exit_group
        int status = (int)(uintptr_t)pop(s);
        exit_group(status);
        break;
    }*/
    case 229: { // sys_epoll_wait
        int epfd = (int)(uintptr_t)pop(s);
        struct epoll_event *events = (struct epoll_event*)pop(s);
        int maxevents = (int)(uintptr_t)pop(s);
        int timeout = (int)(uintptr_t)pop(s);
        int result = epoll_wait(epfd, events, maxevents, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 230: { // sys_epoll_ctl
        int epfd = (int)(uintptr_t)pop(s);
        int op = (int)(uintptr_t)pop(s);
        int fd = (int)(uintptr_t)pop(s);
        struct epoll_event *event = (struct epoll_event*)pop(s);
        int result = epoll_ctl(epfd, op, fd, event);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 231: { // sys_tgkill
        pid_t tgid = (pid_t)(uintptr_t)pop(s);
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int sig = (int)(uintptr_t)pop(s);
        int result = tgkill(tgid, pid, sig);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 232: { // sys_utimes
        const char *filename = (const char*)pop(s);
        struct timeval *utimes = (struct timeval*)pop(s);
        int result = utimes(filename, utimes);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 233: { // sys_vserver
        // Not implemented: linux-vserver support
        push(s, (void*)(uintptr_t)-ENOSYS);
        break;
    }
    case 234: { // sys_mbind
        void *addr = (void*)pop(s);
        unsigned long len = (unsigned long)(uintptr_t)pop(s);
        int mode = (int)(uintptr_t)pop(s);
        const unsigned long *nodemask = (const unsigned long*)pop(s);
        unsigned long maxnode = (unsigned long)(uintptr_t)pop(s);
        unsigned flags = (unsigned)(uintptr_t)pop(s);
        int result = mbind(addr, len, mode, nodemask, maxnode, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 235: { // sys_set_mempolicy
        int mode = (int)(uintptr_t)pop(s);
        const unsigned long *nodemask = (const unsigned long*)pop(s);
        unsigned long maxnode = (unsigned long)(uintptr_t)pop(s);
        int result = set_mempolicy(mode, nodemask, maxnode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 236: { // sys_get_mempolicy
        int *policy = (int*)pop(s);
        unsigned long *nodemask = (unsigned long*)pop(s);
        unsigned long maxnode = (unsigned long)(uintptr_t)pop(s);
        void *addr = (void*)pop(s);
        unsigned long flags = (unsigned long)(uintptr_t)pop(s);
        int result = get_mempolicy(policy, nodemask, maxnode, addr, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 237: { // sys_mq_open
        const char *name = (const char*)pop(s);
        int oflag = (int)(uintptr_t)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        struct mq_attr *attr = (struct mq_attr*)pop(s);
        mqd_t result = mq_open(name, oflag, mode, attr);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 238: { // sys_mq_unlink
        const char *name = (const char*)pop(s);
        int result = mq_unlink(name);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 239: { // sys_mq_timedsend
        mqd_t mqdes = (mqd_t)(uintptr_t)pop(s);
        const char *msg_ptr = (const char*)pop(s);
        size_t msg_len = (size_t)(uintptr_t)pop(s);
        unsigned int msg_prio = (unsigned int)(uintptr_t)pop(s);
        const struct timespec *abs_timeout = (const struct timespec*)pop(s);
        int result = mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, abs_timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 240: { // sys_mq_timedreceive
        mqd_t mqdes = (mqd_t)(uintptr_t)pop(s);
        char *msg_ptr = (char*)pop(s);
        size_t msg_len = (size_t)(uintptr_t)pop(s);
        unsigned int *msg_prio = (unsigned int*)pop(s);
        const struct timespec *abs_timeout = (const struct timespec*)pop(s);
        ssize_t result = mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, abs_timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 241: { // sys_mq_notify
        mqd_t mqdes = (mqd_t)(uintptr_t)pop(s);
        const struct sigevent *notification = (const struct sigevent*)pop(s);
        int result = mq_notify(mqdes, notification);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 242: { // sys_mq_getsetattr
        mqd_t mqdes = (mqd_t)(uintptr_t)pop(s);
        const struct mq_attr *mqstat = (const struct mq_attr*)pop(s);
        struct mq_attr *omqstat = (struct mq_attr*)pop(s);
        int result = mq_getsetattr(mqdes, mqstat, omqstat);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 243: { // sys_kexec_load
        unsigned long entry = (unsigned long)(uintptr_t)pop(s);
        unsigned long nr_segments = (unsigned long)(uintptr_t)pop(s);
        const struct kexec_segment *segments = (const struct kexec_segment*)pop(s);
        unsigned long flags = (unsigned long)(uintptr_t)pop(s);
        int result = kexec_load(entry, nr_segments, segments, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 244: { // sys_waitid
        idtype_t idtype = (idtype_t)(uintptr_t)pop(s);
        id_t id = (id_t)(uintptr_t)pop(s);
        siginfo_t *infop = (siginfo_t*)pop(s);
        int options = (int)(uintptr_t)pop(s);
        struct rusage *ru = (struct rusage*)pop(s);
        int result = waitid(idtype, id, infop, options, ru);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 245: { // sys_add_key
        const char *type = (const char*)pop(s);
        const char *description = (const char*)pop(s);
        const void *payload = (const void*)pop(s);
        size_t plen = (size_t)(uintptr_t)pop(s);
        key_serial_t keyring = (key_serial_t)(uintptr_t)pop(s);
        int result = add_key(type, description, payload, plen, keyring);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 246: { // sys_request_key
        const char *type = (const char*)pop(s);
        const char *description = (const char*)pop(s);
        const char *callout_info = (const char*)pop(s);
        key_serial_t keyring = (key_serial_t)(uintptr_t)pop(s);
        int result = request_key(type, description, callout_info, keyring);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 247: { // sys_keyctl
        int cmd = (int)(uintptr_t)pop(s);
        unsigned long arg2 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg3 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg4 = (unsigned long)(uintptr_t)pop(s);
        unsigned long arg5 = (unsigned long)(uintptr_t)pop(s);
        int result = keyctl(cmd, arg2, arg3, arg4, arg5);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 248: { // sys_ioprio_set
        int which = (int)(uintptr_t)pop(s);
        int who = (int)(uintptr_t)pop(s);
        int ioprio = (int)(uintptr_t)pop(s);
        int result = ioprio_set(which, who, ioprio);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 249: { // sys_ioprio_get
        int which = (int)(uintptr_t)pop(s);
        int who = (int)(uintptr_t)pop(s);
        int result = ioprio_get(which, who);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 250: { // sys_inotify_init
        int result = inotify_init();
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 251: { // sys_inotify_add_watch
        int fd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        uint32_t mask = (uint32_t)(uintptr_t)pop(s);
        int result = inotify_add_watch(fd, pathname, mask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 252: { // sys_inotify_rm_watch
        int fd = (int)(uintptr_t)pop(s);
        int wd = (int)(uintptr_t)pop(s);
        int result = inotify_rm_watch(fd, wd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 253: { // sys_migrate_pages
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        unsigned long maxnode = (unsigned long)(uintptr_t)pop(s);
        const unsigned long *old_nodes = (const unsigned long*)pop(s);
        const unsigned long *new_nodes = (const unsigned long*)pop(s);
        int result = migrate_pages(pid, maxnode, old_nodes, new_nodes);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 254: { // sys_openat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int result = openat(dirfd, pathname, flags, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 255: { // sys_mkdirat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int result = mkdirat(dirfd, pathname, mode);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 256: { // sys_mknodat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        dev_t dev = (dev_t)(uintptr_t)pop(s);
        int result = mknodat(dirfd, pathname, mode, dev);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 257: { // sys_fchownat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        uid_t user = (uid_t)(uintptr_t)pop(s);
        gid_t group = (gid_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = fchownat(dirfd, pathname, user, group, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 258: { // sys_futimesat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        const struct timeval *times = (const struct timeval*)pop(s);
        int result = futimesat(dirfd, pathname, times);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 259: { // sys_newfstatat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        struct stat *statbuf = (struct stat*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = newfstatat(dirfd, pathname, statbuf, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 260: { // sys_unlinkat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = unlinkat(dirfd, pathname, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 261: { // sys_renameat
        int olddirfd = (int)(uintptr_t)pop(s);
        const char *oldpath = (const char*)pop(s);
        int newdirfd = (int)(uintptr_t)pop(s);
        const char *newpath = (const char*)pop(s);
        int result = renameat(olddirfd, oldpath, newdirfd, newpath);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 262: { // sys_linkat
        int olddirfd = (int)(uintptr_t)pop(s);
        const char *oldpath = (const char*)pop(s);
        int newdirfd = (int)(uintptr_t)pop(s);
        const char *newpath = (const char*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = linkat(olddirfd, oldpath, newdirfd, newpath, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 263: { // sys_symlinkat
        const char *target = (const char*)pop(s);
        int newdirfd = (int)(uintptr_t)pop(s);
        const char *linkpath = (const char*)pop(s);
        int result = symlinkat(target, newdirfd, linkpath);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 264: { // sys_readlinkat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        char *buf = (char*)pop(s);
        size_t bufsiz = (size_t)(uintptr_t)pop(s);
        ssize_t result = readlinkat(dirfd, pathname, buf, bufsiz);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 265: { // sys_fchmodat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        mode_t mode = (mode_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = fchmodat(dirfd, pathname, mode, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 266: { // sys_faccessat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        int mode = (int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = faccessat(dirfd, pathname, mode, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 267: { // sys_pselect6
        int nfds = (int)(uintptr_t)pop(s);
        fd_set *readfds = (fd_set*)pop(s);
        fd_set *writefds = (fd_set*)pop(s);
        fd_set *exceptfds = (fd_set*)pop(s);
        const struct timespec *timeout = (const struct timespec*)pop(s);
        sigset_t *sigmask = (sigset_t*)pop(s);
        int result = pselect6(nfds, readfds, writefds, exceptfds, timeout, sigmask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 268: { // sys_ppoll
        struct pollfd *fds = (struct pollfd*)pop(s);
        nfds_t nfds = (nfds_t)(uintptr_t)pop(s);
        const struct timespec *tmo_p = (const struct timespec*)pop(s);
        const sigset_t *sigmask = (const sigset_t*)pop(s);
        int result = ppoll(fds, nfds, tmo_p, sigmask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 269: { // sys_unshare
        int flags = (int)(uintptr_t)pop(s);
        int result = unshare(flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 270: { // sys_set_robust_list
        struct robust_list_head *head = (struct robust_list_head*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int result = set_robust_list(head, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 271: { // sys_get_robust_list
        int pid = (int)(uintptr_t)pop(s);
        struct robust_list_head **head_ptr = (struct robust_list_head**)pop(s);
        size_t *len_ptr = (size_t*)pop(s);
        int result = get_robust_list(pid, head_ptr, len_ptr);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 272: { // sys_splice
        int fd_in = (int)(uintptr_t)pop(s);
        loff_t *off_in = (loff_t*)pop(s);
        int fd_out = (int)(uintptr_t)pop(s);
        loff_t *off_out = (loff_t*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        ssize_t result = splice(fd_in, off_in, fd_out, off_out, len, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 273: { // sys_tee
        int fd_in = (int)(uintptr_t)pop(s);
        int fd_out = (int)(uintptr_t)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        ssize_t result = tee(fd_in, fd_out, len, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 274: { // sys_sync_file_range
        int fd = (int)(uintptr_t)pop(s);
        off64_t offset = (off64_t)(uintptr_t)pop(s);
        off64_t nbytes = (off64_t)(uintptr_t)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        int result = sync_file_range(fd, offset, nbytes, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 275: { // sys_vmsplice
        int fd = (int)(uintptr_t)pop(s);
        const struct iovec *iov = (const struct iovec*)pop(s);
        unsigned long nr_segs = (unsigned long)(uintptr_t)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        ssize_t result = vmsplice(fd, iov, nr_segs, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 276: { // sys_move_pages
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        unsigned long count = (unsigned long)(uintptr_t)pop(s);
        void **pages = (void**)pop(s);
        const int *nodes = (const int*)pop(s);
        int *status = (int*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = move_pages(pid, count, pages, nodes, status, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 277: { // sys_utimensat
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        const struct timespec *times = (const struct timespec*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = utimensat(dirfd, pathname, times, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 278: { // sys_epoll_pwait
        int epfd = (int)(uintptr_t)pop(s);
        struct epoll_event *events = (struct epoll_event*)pop(s);
        int maxevents = (int)(uintptr_t)pop(s);
        int timeout = (int)(uintptr_t)pop(s);
        const sigset_t *sigmask = (const sigset_t*)pop(s);
        int result = epoll_pwait(epfd, events, maxevents, timeout, sigmask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 279: { // sys_signalfd
        int fd = (int)(uintptr_t)pop(s);
        const sigset_t *mask = (const sigset_t*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = signalfd(fd, mask, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 280: { // sys_timerfd_create
        int clockid = (int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = timerfd_create(clockid, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 281: { // sys_eventfd
        unsigned int initval = (unsigned int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = eventfd(initval, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 282: { // sys_fallocate
        int fd = (int)(uintptr_t)pop(s);
        int mode = (int)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        off_t len = (off_t)(uintptr_t)pop(s);
        int result = fallocate(fd, mode, offset, len);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 283: { // sys_timerfd_settime
        int fd = (int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        const struct itimerspec *new_value = (const struct itimerspec*)pop(s);
        struct itimerspec *old_value = (struct itimerspec*)pop(s);
        int result = timerfd_settime(fd, flags, new_value, old_value);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 284: { // sys_timerfd_gettime
        int fd = (int)(uintptr_t)pop(s);
        struct itimerspec *curr_value = (struct itimerspec*)pop(s);
        int result = timerfd_gettime(fd, curr_value);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 285: { // sys_accept4
        int sockfd = (int)(uintptr_t)pop(s);
        struct sockaddr *addr = (struct sockaddr*)pop(s);
        socklen_t *addrlen = (socklen_t*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = accept4(sockfd, addr, addrlen, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 286: { // sys_signalfd4
        int fd = (int)(uintptr_t)pop(s);
        const sigset_t *mask = (const sigset_t*)pop(s);
        size_t sizemask = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = signalfd4(fd, mask, sizemask, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 287: { // sys_eventfd2
        unsigned int initval = (unsigned int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = eventfd2(initval, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 288: { // sys_epoll_create1
        int flags = (int)(uintptr_t)pop(s);
        int result = epoll_create1(flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 289: { // sys_dup3
        int oldfd = (int)(uintptr_t)pop(s);
        int newfd = (int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = dup3(oldfd, newfd, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 290: { // sys_pipe2
        int *pipefd = (int*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = pipe2(pipefd, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 291: { // sys_inotify_init1
        int flags = (int)(uintptr_t)pop(s);
        int result = inotify_init1(flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    case 292: { // sys_preadv
        int fd = (int)(uintptr_t)pop(s);
        const struct iovec *iov = (const struct iovec*)pop(s);
        int iovcnt = (int)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        ssize_t result = preadv(fd, iov, iovcnt, offset);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 293: { // sys_pwritev
        int fd = (int)(uintptr_t)pop(s);
        const struct iovec *iov = (const struct iovec*)pop(s);
        int iovcnt = (int)(uintptr_t)pop(s);
        off_t offset = (off_t)(uintptr_t)pop(s);
        ssize_t result = pwritev(fd, iov, iovcnt, offset);
        push(s, (void*)(uintptr_t)result);
        break;
    }/*
    case 294: { // sys_rt_tgsigqueueinfo
        pid_t tgid = (pid_t)(uintptr_t)pop(s);
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int sig = (int)(uintptr_t)pop(s);
        siginfo_t *uinfo = (siginfo_t*)pop(s);
        int result = rt_tgsigqueueinfo(tgid, pid, sig, uinfo);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 295: { // sys_perf_event_open
        struct perf_event_attr *attr_uptr = (struct perf_event_attr*)pop(s);
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        int cpu = (int)(uintptr_t)pop(s);
        int group_fd = (int)(uintptr_t)pop(s);
        unsigned long flags = (unsigned long)(uintptr_t)pop(s);
        int result = perf_event_open(attr_uptr, pid, cpu, group_fd, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 296: { // sys_recvmmsg
        int sockfd = (int)(uintptr_t)pop(s);
        struct mmsghdr *msgvec = (struct mmsghdr*)pop(s);
        unsigned int vlen = (unsigned int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        struct timespec *timeout = (struct timespec*)pop(s);
        ssize_t result = recvmmsg(sockfd, msgvec, vlen, flags, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 297: { // sys_fanotify_init
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        int result = fanotify_init(flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 298: { // sys_fanotify_mark
        int fanotify_fd = (int)(uintptr_t)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        uint64_t mask = (uint64_t)(uintptr_t)pop(s);
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        int result = fanotify_mark(fanotify_fd, flags, mask, dirfd, pathname);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 299: { // sys_prlimit64
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        unsigned int resource = (unsigned int)(uintptr_t)pop(s);
        const struct rlimit *new_limit = (const struct rlimit*)pop(s);
        struct rlimit *old_limit = (struct rlimit*)pop(s);
        int result = prlimit64(pid, resource, new_limit, old_limit);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 300: { // sys_name_to_handle_at
        int dfd = (int)(uintptr_t)pop(s);
        const char *name = (const char*)pop(s);
        struct file_handle *handle = (struct file_handle*)pop(s);
        int *mnt_id = (int*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = name_to_handle_at(dfd, name, handle, mnt_id, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 301: { // sys_open_by_handle_at
        int mount_fd = (int)(uintptr_t)pop(s);
        struct file_handle *handle = (struct file_handle*)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = open_by_handle_at(mount_fd, handle, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 302: { // sys_syncfs
        int fd = (int)(uintptr_t)pop(s);
        int result = syncfs(fd);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 303: { // sys_sendmmsg
        int sockfd = (int)(uintptr_t)pop(s);
        struct mmsghdr *msgvec = (struct mmsghdr*)pop(s);
        unsigned int vlen = (unsigned int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        ssize_t result = sendmmsg(sockfd, msgvec, vlen, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 304: { // sys_setns
        int fd = (int)(uintptr_t)pop(s);
        int nstype = (int)(uintptr_t)pop(s);
        int result = setns(fd, nstype);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 305: { // sys_clock_adjtime
        clockid_t which_clock = (clockid_t)(uintptr_t)pop(s);
        struct timex *tx = (struct timex*)pop(s);
        int result = clock_adjtime(which_clock, tx);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 306: { // sys_sync_file_range2
        int fd = (int)(uintptr_t)pop(s);
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        off64_t offset = (off64_t)(uintptr_t)pop(s);
        off64_t nbytes = (off64_t)(uintptr_t)pop(s);
        int result = sync_file_range2(fd, flags, offset, nbytes);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 307: { // sys_set_tid_address
        int *tidptr = (int*)pop(s);
        int result = set_tid_address(tidptr);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 308: { // sys_futex
        int *uaddr = (int*)pop(s);
        int op = (int)(uintptr_t)pop(s);
        int val = (int)(uintptr_t)pop(s);
        const struct timespec *timeout = (const struct timespec*)pop(s);
        int *uaddr2 = (int*)pop(s);
        int val3 = (int)(uintptr_t)pop(s);
        int result = futex(uaddr, op, val, timeout, uaddr2, val3);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 309: { // sys_sched_setaffinity
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        size_t cpusetsize = (size_t)(uintptr_t)pop(s);
        const cpu_set_t *mask = (const cpu_set_t*)pop(s);
        int result = sched_setaffinity(pid, cpusetsize, mask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 310: { // sys_sched_getaffinity
        pid_t pid = (pid_t)(uintptr_t)pop(s);
        size_t cpusetsize = (size_t)(uintptr_t)pop(s);
        cpu_set_t *mask = (cpu_set_t*)pop(s);
        int result = sched_getaffinity(pid, cpusetsize, mask);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 311: { // sys_setns
        int fd = (int)(uintptr_t)pop(s);
        int nstype = (int)(uintptr_t)pop(s);
        int result = setns(fd, nstype);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 312: { // sys_io_pgetevents
        aio_context_t ctx_id = (aio_context_t)(uintptr_t)pop(s);
        long min_nr = (long)(uintptr_t)pop(s);
        long nr = (long)(uintptr_t)pop(s);
        struct io_event *events = (struct io_event*)pop(s);
        const struct timespec *timeout = (const struct timespec*)pop(s);
        int result = io_pgetevents(ctx_id, min_nr, nr, events, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 313: { // sys_mlock2
        const void *addr = (const void*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int result = mlock2(addr, len, flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 314: { // sys_pkey_mprotect
        void *addr = (void*)pop(s);
        size_t len = (size_t)(uintptr_t)pop(s);
        int prot = (int)(uintptr_t)pop(s);
        int pkey = (int)(uintptr_t)pop(s);
        int result = pkey_mprotect(addr, len, prot, pkey);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 315: { // sys_pkey_alloc
        unsigned int flags = (unsigned int)(uintptr_t)pop(s);
        int result = pkey_alloc(flags);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 316: { // sys_pkey_free
        int pkey = (int)(uintptr_t)pop(s);
        int result = pkey_free(pkey);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 317: { // sys_statx
        int dirfd = (int)(uintptr_t)pop(s);
        const char *pathname = (const char*)pop(s);
        unsigned flags = (unsigned)(uintptr_t)pop(s);
        unsigned mask = (unsigned)(uintptr_t)pop(s);
        struct statx *statxbuf = (struct statx*)pop(s);
        int result = statx(dirfd, pathname, flags, mask, statxbuf);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 318: { // sys_io_pgetevents
        aio_context_t ctx_id = (aio_context_t)(uintptr_t)pop(s);
        long min_nr = (long)(uintptr_t)pop(s);
        long nr = (long)(uintptr_t)pop(s);
        struct io_event *events = (struct io_event*)pop(s);
        const struct timespec *timeout = (const struct timespec*)pop(s);
        int result = io_pgetevents(ctx_id, min_nr, nr, events, timeout);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 319: { // sys_rseq
        void *rseq = (void*)pop(s);
        unsigned int rseq_len = (unsigned int)(uintptr_t)pop(s);
        int flags = (int)(uintptr_t)pop(s);
        int sig = (int)(uintptr_t)pop(s);
        int result = rseq(rseq, rseq_len, flags, sig);
        push(s, (void*)(uintptr_t)result);
        break;
    }
    case 320: { // sys_io_uring_setup
        unsigned entries = (unsigned)(uintptr_t)pop(s);
        struct io_uring_params *params = (struct io_uring_params*)pop(s);
        int result = io_uring_setup(entries, params);
        push(s, (void*)(uintptr_t)result);
        break;
    }*/
    default:
        printf("Invalid syscall ID: %d\n", syscall_id);
        break;
    }
}







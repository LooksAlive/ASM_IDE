#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "assembly.h"


// Initialize project
void init_project(Project* proj, const char* project_name) {
    proj->file_size = 2 << 15;
    proj->main_function = 1;
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







void init_function(Function* func) {
    if (func == NULL) {
        return;
    }

    func->offset = 0;
    func->visible_globaly = 0;
    func->name = NULL;
    func->parameters = NULL;
    func->param_count = 0;
    func->local_variables = NULL;
    func->local_var_count = 0;
    func->bytecode = NULL;
    func->bytecode_size = 0;
    func->references = NULL;
    func->ref_size = 0;
}

void serialize_instruction(Instruction* instr, char** bytecode, ull* bytecode_size) {

}

// we do not need bytecode size, since all information are there
Instruction deserialize_instruction(char* bytecode) {
    Instruction instr;

    switch (bytecode[0]) {
        case MOV:
            break;
    }

    return instr;
}





// it is seuqnce of bytes always, so we need to update whole function
void update_function(const Function* function_old, const Function* function_new) {
    // check if we have enough space:
    // * function is shorter -> rewrite existing, the rest fill with 0, update free space file


    // * function is longer --> check for free space after this function, if not fill with 0, write to new offset




}


void handle_MOV(Project *p, Instruction* instr) {
    DataMovement_s* dms = (DataMovement_s*)instr->data;

    if(dms->src.type == L_REG || dms->src.type == C_REG) {
        if(dms->dest.type == L_REG || dms->dest.type == C_REG) {
            if(dms->src_bytes_to_move.type == NODE_TYPE_NONE) {
                dms->src.data.reg.value = dms->dest.data.reg.value;
            }
            else {
                if(dms->src_bytes_to_move.type == ULLOFFSET) { // consider rather constant
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
    ControlFlow_s* cmp_data = ((ControlFlow_s*)instr->data);


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















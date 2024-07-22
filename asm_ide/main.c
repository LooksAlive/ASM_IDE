#include <stdio.h>

#include "assembly.h"

#include <signal.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static Project p;

static Function *current_function = NULL;
Instruction *current_instruction = NULL;

#define INPUT_WINDOW_HEIGHT 3

WINDOW *main_win, *input_win, *details_win, *instructions_win, *registers_win, *stack_win, *memory_win, *help_win, *stack_vars_win, *call_stack_win;
int parent_x, parent_y;
int details_win_height = 10;  // Adjust height as needed
int instructions_win_start_y = 11;
int main_win_current_column, main_win_current_row;


typedef enum Mode {
    MODE_NONE = 0,
    MODE_FUNCTION_INFO = 1 << 0,
    MODE_INSTRUCTION_INFO = 1 << 1,
    MODE_INSTRUCTION_EDIT = 1 << 2,
    MODE_INSTRUCTION_ADD = 1 << 3,
    MODE_INSTRUCTION_DELETE = 1 << 4,
    MODE_PROJECT_INFO = 1 << 5,
    MODE_PROJECT_EDIT = 1 << 6,
    MODE_PROJECT_ADD = 1 << 7,
    MODE_PROJECT_DELETE = 1 << 8,
    MODE_PROJECT_SAVE = 1 << 9,
    MODE_PROJECT_LOAD = 1 << 10,
    MODE_PROJECT_NEW = 1 << 11,
    MODE_PROJECT_CLEAN = 1 << 12,
    MODE_PROJECT_EXIT = 1 << 13,
    MODE_PROJECT_HELP = 1 << 14,
    MODE_PROJECT_ABOUT = 1 << 15,
    MODE_INSTRUCTION_HELP = 1 << 16,
    MODE_INSTRUCTION_ABOUT = 1 << 17,
    MODE_INSTRUCTION_SAVE = 1 << 18,
    MODE_FUNCTION_EDIT = 1 << 19,
} Mode;

// boolean values
int mode = MODE_NONE;

void set_mode(Mode mode_, bool clear) {
    if (clear) {
        mode = 0;
    }
    mode |= mode_;
}

char* strdup(const char* str) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, str, len);
    }
    return copy;
}

void cleanup()
{
    delwin(main_win);
    delwin(input_win);
    endwin();

    exit(0);
}

void handle_resize(int sig)
{
    endwin();
    refresh();
    clear();

    // Get the new size of the terminal window
    getmaxyx(stdscr, parent_y, parent_x);

    // Ensure the new size is valid
    if (parent_y <= INPUT_WINDOW_HEIGHT)
    {
        // Minimum height required is INPUT_WINDOW_HEIGHT + 1 for main window
        fprintf(stderr, "Terminal height too small\n");
        cleanup();
        exit(1);
    }

    // Resize and move the main and input windows
    if (main_win)
    {
        wresize(main_win, parent_y - INPUT_WINDOW_HEIGHT, parent_x);
        mvwin(main_win, 0, 0);
    }
    if (input_win)
    {
        wresize(input_win, INPUT_WINDOW_HEIGHT, parent_x);
        mvwin(input_win, parent_y - INPUT_WINDOW_HEIGHT, 0);
    }

    // Create boxes around the windows
    box(main_win, 0, 0);
    box(input_win, 0, 0);

    // Display the initial prompt in the input window
    mvwprintw(input_win, 1, 1, "<input># ");
    wrefresh(input_win);
    wclear(main_win);

    // Redisplay the project info or other content in the main window
    //display_project_info("Project Info: Placeholder"); // Replace with actual project info
}

void handle_sigint(int sig)
{
    cleanup();
    exit(0);
}

void show_struct_info(StructType* st) {
    // Clear the main window
    wclear(main_win);
    wrefresh(main_win);

    int y = 1;
    // start displaying the struct info
    // Display the struct info in the main window
    mvwprintw(main_win, y, 1, "Struct: ");
    mvwprintw(main_win, y, strlen("Struct: "), st->name);
    y++;
    // show parent object
    mvwprintw(main_win, y, 1, "Parent: ");
    mvwprintw(main_win, y, strlen("Parent: "), st->parent.object.offset);
    y++;
    // Display the struct members in the main window
    mvwprintw(main_win, y, 1, "Members: ");
    
    for (int i = 0; i < st->fields_size; i++)
    {
        mvwprintw(main_win, y, 1, "  %u: %s", st->fields[i].object.variable->name, st->fields[i].object.variable->vtype);
        y++;
    }
    mvwprintw(main_win, y, 1, "References: ");

    y++;
    /*
    for (int i = 0; i < st->references_children; i++)
    {
        mvwprintw(main_win, y, 1, "  %s: %s", st->references[i].object.variable->name, st->references[i].object.variable->vtype);
        y++;
    }
    */

    wrefresh(main_win);
}

void show_module_info(Module *m) {
    // Clear the main window
    wclear(main_win);
    wrefresh(main_win); 

    int y = 1;
    // start displaying the module info
    // Display the module info in the main window
    mvwprintw(main_win, y, 1, "Module: ");
    mvwprintw(main_win, y, strlen("Module: "), m->name);
    y++;
    // show parent object
    mvwprintw(main_win, y, 1, "Parent: ");
    mvwprintw(main_win, y, strlen("Parent: "), m->parent.object.offset);
    y++;

    // Display the module members in the main window
    mvwprintw(main_win, y, 1, "Members: ");
    y++;
    for (int i = 0; i < m->fields_size; i++) {
        mvwprintw(main_win, y, 1, "  %u: %s", m->fields[i].object.offset, m->fields[i].object.variable->vtype);
        y++;
    }
    
    wclear(main_win);
    wrefresh(main_win);
}







char *get_instruction_name(Instruction *instr)
{
    switch (instr->i_type)
    {
    case MOV:
        return "MOV";
    case ADD:
        return "ADD";
    case SUB:
        return "SUB";
    case JMP:
        return "JMP";
    case JZ:
        return "JZ";
    case JNZ:
        return "JNZ";
    case CALL:
        return "CALL";
    case RET:
        return "RET";
    case PUSH:
        return "PUSH";
    case POP:
        return "POP";
    case INT:
        return "INT";
    case NOP:
        return "NOP";
    case HLT:
        return "HLT";
    default:
        return "UNKNOWN";
    }
}

InstructionEnum get_instruction_type_from_name(const char *name) {
    if (strcmp(name, "MOV") == 0) return MOV;
    if (strcmp(name, "ADD") == 0) return ADD;
    if (strcmp(name, "SUB") == 0) return SUB;
    if (strcmp(name, "JMP") == 0) return JMP;
    if (strcmp(name, "JZ") == 0) return JZ;
    if (strcmp(name, "JNZ") == 0) return JNZ;
    if (strcmp(name, "CALL") == 0) return CALL;
    if (strcmp(name, "RET") == 0) return RET;
    if (strcmp(name, "PUSH") == 0) return PUSH;
    if (strcmp(name, "POP") == 0) return POP;
    if (strcmp(name, "INT") == 0) return INT;
    if (strcmp(name, "NOP") == 0) return NOP;
    if (strcmp(name, "HLT") == 0) return HLT;
    if (strcmp(name, "VAR") == 0) return VAR;

    return NONE;
}

void get_instruction_address(Instruction *instr, char *buffer, size_t buffer_size) {
    // Format the address using snprintf for safer bounds checking
    int bytes_written = snprintf(buffer, buffer_size, "%llu", instr->instruction_start_offset);

    // Ensure successful formatting (optional, but recommended for robustness)
    if (bytes_written < 0 || bytes_written >= buffer_size) {
        // Handle error (e.g., clear buffer or print an error message)
        snprintf(buffer, buffer_size, "ERROR");
    }
}



int parse_instruction_address(const char *address) {
    char *endptr;
    long int address_int = strtol(address, &endptr, 16);
    if (*endptr!= '\0') {
        return -1;
    }
    return address_int;
}

// construct operands from void* data, mostly specific type e.g. DataMovement_s and then Node*
void get_instruction_operands(Instruction *instr, char* operands, size_t operand_size) {
    if (instr->data == NULL) {
        snprintf(operands, operand_size, " ");
        return;
    }

    size_t len = 0;

    switch (instr->i_type) {
    case MOV:
    {
        DataMovement_s *dm = (DataMovement_s *)instr->data;

        // Process destination
        switch (dm->dest.type) {
        case L_REG:
            len += snprintf(operands + len, operand_size - len, "%s,", dm->dest.data.reg.name);
            break;
        case C_VAR:
            len += snprintf(operands + len, operand_size - len, "%s,", dm->dest.data.var.name);
            break;
        case CONSTANT:
            len += snprintf(operands + len, operand_size - len, "%llu,", dm->dest.data.offset);
            break;
        default:
            len += snprintf(operands + len, operand_size - len, "UNKNOWN,");
            break;
        }

        // Process source
        switch (dm->src.type) {
        case L_REG:
            len += snprintf(operands + len, operand_size - len, "%s", dm->src.data.reg.name);
            break;
        case C_VAR:
            len += snprintf(operands + len, operand_size - len, "%s", dm->src.data.var.name);
            break;
        case CONSTANT:
            len += snprintf(operands + len, operand_size - len, "%llu", dm->src.data.offset);
            break;
        default:
            len += snprintf(operands + len, operand_size - len, "UNKNOWN");
            break;
        }

        // Process source bytes to move
        switch (dm->src_bytes_to_move.type) {
        case CONSTANT:
            len += snprintf(operands + len, operand_size - len, ",%llu", dm->src_bytes_to_move.data.offset);
            break;
        default:
            len += snprintf(operands + len, operand_size - len, ",UNKNOWN");
            break;
        }
        break;
    }
    case ADD:
    case SUB:
    case DIV:
    case MUL:
    {
        Arithmetic_s *arith = (Arithmetic_s *)instr->data;

        // Process destination
        switch (arith->dest.type) {
        case L_REG:
            len += snprintf(operands + len, operand_size - len, "%s", arith->dest.data.reg.name);
            break;
        case C_VAR:
            len += snprintf(operands + len, operand_size - len, "%s", arith->dest.data.var.name);
            break;
        case CONSTANT:
            len += snprintf(operands + len, operand_size - len, "%llu", arith->dest.data.offset);
            break;
        default:
            len += snprintf(operands + len, operand_size - len, "UNKNOWN");
            break;
        }

        // Process sources
        for (int i = 0; i < arith->src_size; i++) {
            switch (arith->src[i].type) {
            case L_REG:
                len += snprintf(operands + len, operand_size - len, ",%s", arith->src[i].data.reg.name);
                break;
            case C_VAR:
                len += snprintf(operands + len, operand_size - len, ",%s", arith->src[i].data.var.name);
                break;
            case CONSTANT:
                len += snprintf(operands + len, operand_size - len, ",%llu", arith->src[i].data.offset);
                break;
            default:
                len += snprintf(operands + len, operand_size - len, ",UNKNOWN");
                break;
            }
        }
        break;
    }
    }

    // Ensure null termination
    operands[operand_size - 1] = '\0';
}


void parse_instruction_operands(const char *operands_str, Instruction *instr) {
    // Example for MOV instruction
    if (instr->i_type == MOV) {
        DataMovement_s *dm = (DataMovement_s *)malloc(sizeof(DataMovement_s));
        char dest[10], src[10];
        sscanf(operands_str, "%9[^,], %9s", dest, src);

        // Parse dest
        if (dest[0] == 'R') {
            dm->dest.type = L_REG;
            strcpy(dm->dest.data.reg.name, dest);
        } else if (isalpha(dest[0])) {
            dm->dest.type = C_VAR;
            strcpy(dm->dest.data.var.name, dest);
        } else {
            dm->dest.type = CONSTANT;
            dm->dest.data.offset = strtoull(dest, NULL, 10);
        }

        // Parse src
        if (src[0] == 'R') {
            dm->src.type = L_REG;
            strcpy(dm->src.data.reg.name, src);
        } else if (isalpha(src[0])) {
            dm->src.type = C_VAR;
            strcpy(dm->src.data.var.name, src);
        } else {
            dm->src.type = CONSTANT;
            dm->src.data.offset = strtoull(src, NULL, 10);
        }

        instr->data = (void *)dm;
    }

    // Add more cases for other instruction types (ADD, SUB, etc.)
}

Instruction *construct_instruction_from_strings(const char *address_str, const char *name_str, const char *operands_str) {
    Instruction *instr = (Instruction *)malloc(sizeof(Instruction));
    instr->instruction_start_offset = parse_instruction_address(address_str);
    instr->i_type = get_instruction_type_from_name(name_str);

    parse_instruction_operands(operands_str, instr);

    return instr;
}

void display_function_details(WINDOW *win, Function *func) {
    // Clear the window
    wclear(win);
    box(win, 0, 0);
    wattron(win, A_BOLD);
    mvwprintw(win, 1, 1, "Function Details");
    wattroff(win, A_BOLD);
    mvwprintw(win, 2, 1, "Name: %s", func->name);
    mvwprintw(win, 3, 1, "Offset: 0x%llX", func->offset);
    if(func->parent.type == OBJ_FUNCTION) {
        mvwprintw(win, 4, 1, "Parent: %s (0x%llX)", func->parent.object.function->offset, func->parent.object.offset);
    } else {
        mvwprintw(win, 4, 1, "Parent: 0x%llX", func->parent.object.offset);
    }
    mvwprintw(win, 5, 1, "Visibility: %s", 
        func->visibility == PUBLIC ? "Public" :
        func->visibility == PRIVATE ? "Private" :
        func->visibility == PROTECTED ? "Protected" : "Unknown");
    mvwprintw(win, 6, 1, "Parameter Count: %u", func->param_count);
    
    for (unsigned int i = 0; i < func->param_count; ++i) {
        mvwprintw(win, 7 + i, 1, "Parameter %d: %s", i + 1, func->parameters[i].variable->name);
    }
    mvwprintw(win, 8, 1, "Instruction Count: %llu", func->instructions_size);
    
    wrefresh(win);
}

void display_instructions(WINDOW *win, Function *func) {
    // Clear the window
    wclear(win);
    box(win, 0, 0);
    
    // Display the header
    wattron(win, A_BOLD);
    mvwprintw(win, 1, 1, "%-10s %-15s %-10s", "Address", "Instruction", "Operands");
    wattroff(win, A_BOLD);

    for (ull i = 0; i < func->instructions_size; i++) {
        Instruction *instr = &func->instructions[i];
        char addr[30];
        get_instruction_address(instr, addr, sizeof(addr));
        char operands[1024];
        get_instruction_operands(instr, operands, sizeof(operands));
        mvwprintw(win, i + 2, 1, "%-10s %-15s %-10s", addr, get_instruction_name(instr), operands);
    }

    wrefresh(win);
}

void show_function_info(Function *func) {
    wclear(main_win);
    //wrefresh(input_win);

    // Get terminal size
    getmaxyx(stdscr, parent_y, parent_x);

    // Calculate positions and sizes
    instructions_win_start_y = details_win_height + 1;

    // Create windows
    //details_win = newwin(details_win_height, parent_x, 0, 0);
    //instructions_win = newwin(parent_y - details_win_height - 1, parent_x, instructions_win_start_y, 0);

    // Display function details
    display_function_details(details_win, func);
    
    // Display instructions
    display_instructions(instructions_win, func);

    // Cleanup
    //delwin(details_win);
    //delwin(instructions_win);
    //endwin();
}

void analyze_user_input(char *input);
void get_user_input(WINDOW *input_win)
{
    wclear(input_win);
    mvwprintw(input_win, 1, 1, "<input># ");
    box(input_win, 0, 0);
    wrefresh(input_win);

    char input[80];
    int pos = 0;
    int len = 0;
    int ch;

    while ((ch = wgetch(input_win)) != '\n' && ch != KEY_ENTER)
    {
        if (ch == 27) // ESC
        {
            goto only_clear;
        }
        else if (ch == KEY_BACKSPACE || ch == 127)
        {
            if (pos > 0)
            {
                pos--;
                mvwdelch(input_win, 1, 10 + pos);
                wclrtoeol(input_win);
            }
            continue;
        }
        else if (ch == KEY_HOME)
        {
            pos = 0;
            wmove(input_win, 1, 10 + pos);
            continue;
        }
        else if (ch == KEY_LEFT)
        {
            if (pos > 0)
            {
                pos--;
                wmove(input_win, 1, 10 + pos);
            }
            continue;
        }
        else if (ch == KEY_RIGHT)
        {
            if (pos < len)
            {
                pos++;
                wmove(input_win, 1, 10 + pos);
            }
            continue;
        }
        else if (isprint(ch))
        {
            if (pos < sizeof(input) - 1)
            {
                input[pos++] = ch;
                len++;
                waddch(input_win, ch);
            }
            continue;
        }
        else if (ch == KEY_END)
        {
            pos = len;
            wmove(input_win, 1, 10 + pos);
            continue;
        }
        wrefresh(input_win);
    }

    input[pos] = '\0';

    // Call the function to analyze the input
    analyze_user_input(input);

only_clear:
    // Clear the input line and reset the prompt
    wmove(input_win, 1, 1);
    wclrtoeol(input_win); // Clear to end of line
    mvwprintw(input_win, 1, 1, "<input># "); // Reset prompt
    wmove(input_win, 1, 10); // Move cursor back to input position
    wrefresh(input_win); // Refresh window to show changes
}

void display_project_info()
{
    wclear(main_win);
    wrefresh(input_win);
    
    box(main_win, 0, 0);
    mvwprintw(main_win, 2, 2, "Project info:");
    char file_size_str[64];
    sprintf(file_size_str, "%llu", p.file_size);
    mvwprintw(main_win, 4, 2, "File size: %s", file_size_str);
    if(p.main_function.type == OBJ_FUNCTION)
    {
        mvwprintw(main_win, 5, 2, "Main function: %s (%llu)", p.main_function.object.function->name, p.main_function.object.function->offset);
    } 
    else {
        mvwprintw(main_win, 5, 2, "Main function: %llu", p.main_function.object.offset);
    }
    
    mvwprintw(main_win, 6, 2, "Files paths:");
    int line = 7;
    for (int i = 0; i < p.files_paths_size; i++)
    {
        mvwprintw(main_win, line++, 4, "%s", p.files_paths[i]);
    }
    mvwprintw(main_win, line++, 2, "Opened functions:");
    for (int i = 0; i < p.opened_objects_size; i++)
    {
        mvwprintw(main_win, line++, 4, "%s", p.opened_objects[i]->object.function->name);
    }
    mvwprintw(main_win, line++, 2, "Free space file: %s", p.free_space_file);
    mvwprintw(main_win, line++, 2, "Project directory: %s", p.project_directory);
    char offset_str[32];
    sprintf(offset_str, "%llu", p.last_offset);
    mvwprintw(main_win, line++, 2, "Last offset: %s", offset_str);
    wrefresh(main_win);
}

void analyze_user_input(char *input)
{
    // set keywords from user
    const char *kw_project = "project";
    const char *kw_load = "load";
    const char *kw_show = "show";
    const char *kw_save = "save";
    const char *kw_quit = "quit";
    const char *kw_help = "help";
    const char *kw_list = "list";
    const char *kw_edit = "edit";
    const char *kw_display = "display";
    const char *kw_run = "run";
    const char *kw_step = "step";
    const char *kw_next = "next";
    const char *kw_step_over = "step_over";
    const char *kw_step_out = "step_out";
    const char *kw_breakpoint = "breakpoint";
    const char *kw_delete = "delete";
    const char *kw_add = "add";
    const char *kw_main = "main";
    const char *kw_function = "function";
    const char *kw_variable = "variable";
    const char *kw_structure = "structure";
    const char *kw_set = "set";
    const char *kw_exit = "exit";

    // TODO:
    // 1. Parse the input string
    char *token = strtok(input, " \n\0");
    while (token != NULL)
    {
        if (strcmp(token, kw_project) == 0)
        {
            // move to next token
            token = strtok(NULL, " \n\0");
            // Handle "project" keyword
        }
        else if (strcmp(token, kw_load) == 0)
        {
            // Handle "load" keyword
        }
        else if (strcmp(token, kw_save) == 0)
        {
            // Handle "save" keyword
        }
        else if (strcmp(token, kw_quit) == 0)
        {
            // Handle "quit" keyword
        }
        else if (strcmp(token, kw_help) == 0)
        {
            // Handle "help" keyword
        }
        else if (strcmp(token, kw_list) == 0)
        {
            // Handle "list" keyword
        }
        else if (strcmp(token, kw_display) == 0)
        {
            // Handle "display" keyword
        }
        else if (strcmp(token, kw_run) == 0)
        {
            // Handle "run" keyword
        }
        else if (strcmp(token, kw_step) == 0)
        {
            // Handle "step" keyword
        }
        else if (strcmp(token, kw_next) == 0)
        {
            // Handle "next" keyword
        }
        else if (strcmp(token, kw_step_over) == 0)
        {
            // Handle "step_over" keyword
        }
        else if (strcmp(token, kw_step_out) == 0)
        {
            // Handle "step_out" keyword
        }
        else if (strcmp(token, kw_breakpoint) == 0)
        {
            // Handle "breakpoint" keyword
        }
        else if (strcmp(token, kw_delete) == 0)
        {
            // Handle "delete" keyword
        }
        else if (strcmp(token, kw_add) == 0)
        {
            token = strtok(NULL, " \n\0");
            // add main function
            if (strcmp(token, kw_main) == 0)
            {
                // Handle "add main" keyword
                if (p.main_function.object.offset != 0)
                {
                    wclear(main_win);
                    mvwprintw(main_win, 1, 1, "Main function already exists.");
                    wrefresh(main_win);
                }
                else
                {
                    // add main function
                    p.main_function.object.function = create_function(&p, "main");
                    p.main_function.type = OBJ_FUNCTION;
                    current_function = p.main_function.object.function;

                    wclear(main_win);
                    mvwprintw(main_win, 1, 1, "Main function added.");
                    wrefresh(main_win);
                }
            }
            // Handle "add" keyword
        }
        else if (strcmp(token, kw_function) == 0)
        {
            // Handle "function" keyword
        }
        else if (strcmp(token, kw_variable) == 0)
        {
            // Handle "variable" keyword
        }
        else if (strcmp(token, kw_structure) == 0)
        {
            // Handle "structure" keyword
        }
        else if (strcmp(token, "sp") == 0) {
            goto show_project;
        }
        else if (strcmp(token, "sf") == 0) {
            goto show_function;
        }
        else if (strcmp(token, "ei") == 0) {
            if(current_function != NULL) {
                show_function_info(current_function);
                set_mode(MODE_INSTRUCTION_EDIT, true);
            }
        }
        else if (strcmp(token, kw_show) == 0)
        {
            // Handle "show" keyword
            token = strtok(NULL, " \n\0");
            // project
            if (strcmp(token, "project") == 0)
            {
                // Handle "show project" keyword
                token = strtok(NULL, " \n\0");
show_project:
                if (&p == 0)
                {
                    // error message to main_win
                    mvwprintw(main_win, 1, 1, "No project loaded.");
                }
                else
                {
                    display_project_info();
                    current_function = NULL;
                }
            }
            // function 
            else if (strcmp(token, kw_function) == 0) {
                // Handle "show function" keyword
show_function:
                token = strtok(NULL, " \n\0");
                if(token == NULL) {
                    if(current_function != NULL) {
                        show_function_info(current_function);
                        return;
                    }
                }
                if (strcmp(token, kw_main) == 0) {
                    // Handle "show function main" keyword
                    //display_function_info(p.main_function);
                    if(p.main_function.object.function == NULL) {
                        wclear(main_win);
                        mvwprintw(main_win, 1, 1, "No main function found.");
                        wrefresh(main_win);
                    }
                    else {
                        show_function_info(p.main_function.object.function);
                        current_function = p.main_function.object.function;
                    }
                }
                else {
                    // get function ull offset, read it and display
                    ull offset;
                    // from token convert ull string to number
                    offset = strtoull(token, NULL, 16);
                    Function* fun = deserialize_function(&p, offset);
                    show_function_info(fun);
                }
            }
        }
        else if (strcmp(token, "ef") == 0) {
            goto edit_function;
        }
        else if (strcmp(token, kw_edit) == 0)
        {
            // Handle "edit" keyword
            token = strtok(NULL, " \n\0");
            if (strcmp(token, kw_function) == 0) {
                token = strtok(NULL, " \n\0");
edit_function:
                if(current_function == NULL) {
                    wclear(main_win);
                    mvwprintw(main_win, 1, 1, "No function loaded.");
                    wrefresh(main_win);
                }
                else {
                    set_mode(MODE_FUNCTION_EDIT, true);
                    return;
                }
            }
            // current function if loaded
        }
        else if (strcmp(token, kw_set) == 0)
        {
            // Handle "set" keyword
        }
        else if (strcmp(token, kw_delete) == 0) {
            // Handle "delete" keyword
            token = strtok(NULL, " \n\0");
            if (strcmp(token, kw_function) == 0) {
                // Handle "delete function" keyword
                token = strtok(NULL, " \n\0");
                if (strcmp(token, kw_main) == 0) {
                    // Handle "delete function main" keyword
                    if(p.main_function.object.function == NULL) {
                        wclear(main_win);
                    }
                }
                else {
                    mvwprintw(main_win, 1, 1, "NOT IMPLEMENTED.");
                    return;
                    // get function ull offset, read it and display
                    ull offset;
                    // from token convert ull string to number
                    offset = strtoull(token, NULL, 16);
                    Function* fun = deserialize_function(&p, offset);
                    if(fun == NULL) {
                        wclear(main_win);
                        mvwprintw(main_win, 1, 1, "Function not found.");
                        wrefresh(main_win);
                    }
                    else {
                        // delete function
                        //delete_function(fun);
                    }
                }
            }
        }
        else if (strcmp(token, kw_exit) == 0) {
            clean_project(&p);
            cleanup();
        }
        else
        {
            mvwprintw(main_win, 1, 1, "Invalid command.");
        }
        token = strtok(NULL, " \n\0");
    }
}


void analyze_command(char *command) {
    char *token = strtok(command, " \n\0");
    if (strcmp(token, "set") == 0)
    {
        token = strtok(NULL, " \n\0");
        if (strcmp(token, "name") == 0) {
            token = strtok(NULL, " \n");
            free(current_function->name);
            current_function->name = strdup(token);
            show_function_info(current_function);
        }  else if (strcmp(token, "visibility") == 0) {
            token = strtok(NULL, " \n\0");
            if (strcmp(token, "PRIVATE") == 0) {
                current_function->visibility = PRIVATE;
            } else if (strcmp(token, "PUBLIC") == 0) {
                current_function->visibility = PUBLIC;
            } else if (strcmp(token, "PROTECTED") == 0) {
                current_function->visibility = PROTECTED;
            } else {
                current_function->visibility = PROTECTED; // default
            }
        }  else if (strcmp(token, "reference") == 0) {
            current_function->references_size = strtoull(token, NULL, 10);
        }
    }
    else if (strcmp(token, "exit") == 0) {
        set_mode(MODE_NONE, true);
        return;
    }
    else if (strcmp(token, "add") == 0) {
        token = strtok(NULL, " \n\0");
        if (strcmp(token, "parameter") == 0) {
            token = strtok(NULL, " \n");
            //Parameter *param = malloc(sizeof(Parameter));
            show_function_info(current_function);
        }
    }
    else if (strcmp(token, "ei") == 0) {
        set_mode(MODE_INSTRUCTION_EDIT, true);
    }
    // edit instructions
    else if (strcmp(token, "edit") == 0) {
        token = strtok(NULL, " \n\0");
        if (strcmp(token, "instructions") == 0) {
            token = strtok(NULL, " \n");
            set_mode(MODE_INSTRUCTION_EDIT, true);
        }
    }
    else {
        //mvwprintw(main_win, 1, 1, "Invalid command.");
    }
}


void edit_function_parameters() {
    char command[256];
    int pos = 0;
    int ch;

    // Initial display
    wclear(input_win);
    box(input_win, 0, 0);
    //mvwprintw(input_win, 1, 1, "Edit Function: %s", current_function->name);
    mvwprintw(input_win, 1, 1, "<editf># ");
    wmove(input_win, 1, 10);
    wrefresh(input_win);

    while (1) {
        ch = wgetch(input_win);

        if (ch == 27) { // ESC key to exit
            set_mode(MODE_NONE, true);
            break;
        } else if (ch == '\n' || ch == KEY_ENTER) { // Enter key to process command
            command[pos] = '\0';
            analyze_command(command);
            pos = 0; // Reset position for new input
            memset(command, 0, sizeof(command)); // Clear command buffer

            // Redraw input prompt after processing
            wclear(input_win);
            box(input_win, 0, 0);
            //mvwprintw(input_win, 1, 1, "Edit Function: %s", current_function->name);
            mvwprintw(input_win, 1, 1, "<editf># ");
            wmove(input_win, 1, 10);
            wrefresh(input_win);
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace key
            if (pos > 0) {
                pos--;
                mvwdelch(input_win, 1, 10 + pos);
                wclrtoeol(input_win);
                wmove(input_win, 1, 10 + pos);
            }
        } else if (isprint(ch)) { // Printable character
            if (pos < sizeof(command) - 1) {
                command[pos++] = ch;
                waddch(input_win, ch);
            }
        }
        wrefresh(input_win);
    }
}

// function that works on input_win returning user input after enter or \n
char *get_input_from_user(char* display_string) {
    char* command = malloc(sizeof(char) * 256);
    int pos = 0;
    int ch;
    // Initial display
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, display_string);
    wmove(input_win, 1, strlen(display_string));
    wrefresh(input_win);

    while (1) {
        ch = wgetch(input_win);
        if (ch == 27) { // ESC key to exit
            break;
        }
        else if (ch == '\n' || ch == KEY_ENTER) { // Enter key to process command
            command[pos] = '\0';
            //pos = 0; // Reset position for new input
            return command;
        }
        else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace key
        if (pos > 0) {
                pos--;
                mvwdelch(input_win, 1, 1 + pos);
                wclrtoeol(input_win);
                wmove(input_win, 1, 1 + pos);
            }
        }
        else if (isprint(ch)) { // Printable character
            if (pos < sizeof(command) - 1) {
                command[pos++] = ch;
                waddch(input_win, ch);
            }
        }
        wrefresh(input_win);
    }
    return NULL;
}


Type *get_type_from_user() {
    char command[256];
    int pos = 0;
    int ch;
    // Initial display
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "<edit_type># ");
    wmove(input_win, 1, strlen("<edit_type># "));
    wrefresh(input_win);
    while (1) {
        ch = wgetch(input_win);
        if (ch == 27) { // ESC key to exit
            break;
        } else if (ch == '\n' || ch == KEY_ENTER) { // Enter key to process command
            command[pos] = '\0';
            //pos = 0; // Reset position for new input
            //return get_type_from_string(command);
        } else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace key
            if (pos > 0) {
                pos--;
                mvwdelch(input_win, 1, 1 + pos);
                wclrtoeol(input_win);
                wmove(input_win, 1, 1 + pos);
            }
        } else if (isprint(ch)) { // Printable character
            if (pos < sizeof(command) - 1) {
                command[pos++] = ch;
                waddch(input_win, ch);
            }
            wrefresh(input_win);
        }
    }
}


Node construct_node_from_user_design_input() {
    char command[256];
    int pos = 0;
    int ch;

    // Initial display
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "<edit_node_opearand># ");
    wmove(input_win, 1, strlen("<edit_node_opearand># "));
    wrefresh(input_win);

    Node d_node;
    d_node.type = NODE_TYPE_NONE;

    ch = wgetch(input_win);
    if (ch == 27) { // ESC key to exit
        return d_node;
    }
    // here we catch specific keys where each key will represent enum type we set to node: NodeType
    switch(ch) {
        case 'c':   // offset
            d_node.type = CONSTANT;
            {
                // record input from user
                // get_input_from_user(command);
                char* input = get_input_from_user("<edit_node_opearand_constant># ");
                if (input == NULL) {
                    mvwprintw(input_win, 1, 1, "Invalid command.");
                    break;
                }
                // convert to ull
                d_node.data.offset = strtoull(input, NULL, 10);
                free(input);
            }
            break;
        case 't': // TYPE
            d_node.type = TYPE;
        {
            // record input from user
            // get_input_from_user(command);
            char* input = get_input_from_user("<edit_node_opearand_type># ");
            if (input == NULL) {
                mvwprintw(input_win, 1, 1, "Invalid command.");
                break;
            }
            // convert to ull
            d_node.data.offset = strtoull(input, NULL, 10);
            free(input);
        }
        case 'l': // L_VAR
            d_node.type = L_VAR;
            {
                // record input from user
                // get_input_from_user(command);
                char* input = get_input_from_user("<edit_node_opearand_l_var># ");
                if (input == NULL) {
                    mvwprintw(input_win, 1, 1, "Invalid command.");
                    break;
                }
                // convert to ull
                d_node.data.offset = strtoull(input, NULL, 10);
                free(input);
            }
            break;
        case 'g': // G_VAR
            d_node.type = L_VAR;
            {
                // record input from user
                // get_input_from_user(command);
                char* input = get_input_from_user("<edit_node_opearand_g_var># ");
                if (input == NULL) {
                    mvwprintw(input_win, 1, 1, "Invalid command.");
                    break;
                }
                // convert to ull
                d_node.data.offset = strtoull(input, NULL, 10);
                free(input);
            }
            break;
        case 'd': // DEREFERENCE
            d_node.type = DEREFERENCE;
            // .data will become another Node*
            {
                d_node.type = DEREFERENCE;
                *d_node.data.dereference = construct_node_from_user_design_input();
            }
            break;
        case 's': // SIZEOF
            d_node.type = SIZEOF;
            {
                // record input from user
                // get_input_from_user(command);
                char* input = get_input_from_user("<edit_node_opearand_sizeof># ");
                if (input == NULL) {
                    mvwprintw(input_win, 1, 1, "Invalid command.");
                    break;
                }
                // convert to ull
                if(isdigit(input[0])) {
                    d_node.data.offset = strtoull(input, NULL, 10);
                } else {
                    d_node.data.var; //get_lvar_from_name(&p, input);
                }
                free(input);
            }
            break;
        case 'o': // STRUCTOFFSET
            d_node.type = STRUCTOFFSET;
            {
                // record input from user
                // get_input_from_user(command);
                char* input = get_input_from_user("<edit_node_opearand_structoffset># ");
                if (input == NULL) {
                    mvwprintw(input_win, 1, 1, "Invalid command.");
                    break;
                }
                // convert to ull
                d_node.data.offset = strtoull(input, NULL, 10);
                free(input);
            }
            break;
        case 'r': // L_REG
            d_node.type = L_REG;
            {
                // record input from user
                // get_input_from_user(command);
                char* input = get_input_from_user("<edit_node_opearand_l_reg># ");
                if (input == NULL) {
                    mvwprintw(input_win, 1, 1, "Invalid command.");
                    break;
                }
                // convert to ull
                d_node.data.offset = strtoull(input, NULL, 10);
                free(input);
            }
            break;
        default:
            break;            
    }

    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "<edit_node_opearand># ");
    wmove(input_win, 1, strlen("<edit_node_opearand># "));
    wrefresh(input_win);

    return d_node;
}

Type resolve_user_typed_type(const char* str) {
    Type t;
    return t;
}

void analyze_instruction_command(char *command) {
    char *token = strtok(command, " \n");
    if (token == NULL) {
        return;
    }

    if (strcmp(token, "insert") == 0) {
        token = strtok(NULL, " \n");
        if (token == NULL) return;
        ull offset = strtoull(token, NULL, 10);
        // insert new instruction at this offset, now lets designing the instruction

        //void insert_instruction(Function* func, const Instruction instr, unsigned int index)
        Instruction instr;
        // now we as user to type the instruction parts, opcode, operand, etc.
        wclear(input_win);
        //box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "<edit_inst_insert_opcode># ");
        wmove(input_win, 1, strlen("<edit_inst_insert_opcode># "));
        wrefresh(input_win);


        char* opcode = malloc(256);
        int pos = 0;
        while(1) {
            int ch = wgetch(input_win);
            if (ch == 27) { // ESC key to exit
                return;
            }
            else if (ch == '\n' || ch == KEY_ENTER) { // Enter key to process command
                opcode[pos] = '\0';
                break;
            }
            else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace key
                if (pos > 0) {
                    pos--;
                    mvwdelch(input_win, 1, strlen("<edit_inst_insert_opcode># ") + pos);
                    wclrtoeol(input_win);
                    wmove(input_win, 1, strlen("<edit_inst_insert_opcode># ") + pos);
                }
            }
            else if (isprint(ch)) { // Printable character 
                if (pos < sizeof(opcode) - 1) {
                    opcode[pos++] = ch;
                    waddch(input_win, ch);
                }
            }
            wrefresh(input_win);
        }
        /*
        wclear(input_win);
        //box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "<edit_inst_insert_operand># ");
        mvwprintw(input_win, 1, strlen("<edit_inst_insert_operand># "), "%s", token);
        wmove(input_win, 1, strlen("<edit_inst_insert_operand># ") + strlen(token));
        wrefresh(input_win);
        */
        instr.i_type = get_instruction_type_from_name(opcode);
        free(opcode);

        switch (instr.i_type)
        {
            case MOV:
            {
                // here we provide a clear interface for the user to type the operand in predefined format
                // so that it will basically create it, e.g. Node structure for each operand
                DataMovement_s* dm = malloc(sizeof(DataMovement_s));
                dm->dest = construct_node_from_user_design_input();
                dm->src = construct_node_from_user_design_input();
                dm->src_bytes_to_move = construct_node_from_user_design_input();
                instr.data = dm;
            }
            break;
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            {
                Arithmetic_s* ar = malloc(sizeof(Arithmetic_s));
                ar->dest = construct_node_from_user_design_input();
                ar->src_size = 1;
                ar->src = malloc(sizeof(Node) * ar->src_size);
                ar->src[0] = construct_node_from_user_design_input();
                instr.data = ar;
            }
            break;
            case JMP:
            {
                Node* n = malloc(sizeof(Node));
                *n = construct_node_from_user_design_input();   // mostly offset Node
                instr.data = n;
            }
            break;
            case RET:
            {
                instr.data = NULL;
            }
            break;
            case CALL:
            {
                Call_s* call = malloc(sizeof(Call_s));
                call->dest = construct_node_from_user_design_input();
                // resolve function object
                call->function->type = OBJ_OFFSET;
                assert(call->function->type == OBJ_OFFSET);
                call->function->object.offset = call->dest.data.offset;

                // parameters Nodes, for now just one
                call->parameters_size = 1;
                call->parameters = malloc(sizeof(Node) * call->parameters_size);
                call->parameters[0] = construct_node_from_user_design_input();
                instr.data = call;
            }
            break;
            case AND:
            case OR:
            case XOR:
            {
                Logical_s* log = malloc(sizeof(Logical_s));
                log->dest = construct_node_from_user_design_input();
                log->src = construct_node_from_user_design_input();
                instr.data = log;
            }
            break;
            case CMP:
            {
                Comparison_s* cmp = malloc(sizeof(Comparison_s));
                cmp->operand_left = construct_node_from_user_design_input();
                cmp->operand_right = construct_node_from_user_design_input();
                //cmp->jump_instruction_type = construct_node_from_user_design_input();
                cmp->jump_instruction_type = JZ;
                cmp->jump_position = construct_node_from_user_design_input();
                
                instr.data = cmp;
            }
            break;
            case VAR:
            {
                // LocalVariable
                LocalVariable* lv = malloc(sizeof(LocalVariable));
                lv->type = resolve_user_typed_type(get_input_from_user("xxx"));
                lv->name = get_input_from_user("name: ");
                

                instr.data = lv;
            }

            default:
            {
                // error
                printf("Error: Unknown instruction type.\n");
            }
        }

        insert_instruction(current_function, instr, get_instruction_index_from_offset(current_function, offset));
    }
    else if (strcmp(token, "delete") == 0 || strcmp(token, "d") == 0) {
        token = strtok(NULL, " \n");
        if (token == NULL) return;
        ull offset = strtoull(token, NULL, 10);
        // delete instruction at this offset
        int index = get_instruction_index_from_offset(current_function, offset);
        if (index == -1) {
            mvwprintw(main_win, 1, 1, "Instruction not found.");
            return;
        }
        remove_instruction(current_function, index);
    }
    else if (strcmp(token, "edit") == 0 || strcmp(token, "e") == 0) {
        token = strtok(NULL, " \n");
        if (token == NULL) return;
        ull offset = strtoull(token, NULL, 10);
        // edit instruction at this offset, now lets designing the instruction
    }
    
     else if (strcmp(token, "exit") == 0 || strcmp(token, "q") == 0) {
        set_mode(MODE_FUNCTION_EDIT, true); // back to function
        return;
    } else {
        // Handle invalid commands
        mvwprintw(main_win, 1, 1, "Invalid command.");
        wrefresh(main_win);
    }
}


void edit_function_instructions() {
    char command[256];
    int pos = 0;
    int ch;

    // Initial display
    wclear(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 1, "<edit_inst># ");
    wmove(input_win, 1, 14);
    wrefresh(input_win);

    while (1) {
        ch = wgetch(input_win);

        if (ch == 27) { // ESC key to exit
            set_mode(MODE_FUNCTION_EDIT, true);
            break;
        } else if (ch == '\n' || ch == KEY_ENTER) { // Enter key to process command
            command[pos] = '\0';
            analyze_instruction_command(command);
            pos = 0; // Reset position for new input
            memset(command, 0, sizeof(command)); // Clear command buffer

            // Redraw input prompt after processing
            wclear(input_win);
            box(input_win, 0, 0);
            mvwprintw(input_win, 1, 1, "<edit_inst># ");
            wmove(input_win, 1, strlen("<edit_inst># "));
            wrefresh(input_win);
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) { // Backspace key
            if (pos > 0) {
                pos--;
                mvwdelch(input_win, 1, 14 + pos);
                wclrtoeol(input_win);
                wmove(input_win, 1, 14 + pos);
            }
        } else if (isprint(ch)) { // Printable character
            if (pos < sizeof(command) - 1) {
                command[pos++] = ch;
                waddch(input_win, ch);
            }
        }
        // exit
        else if(ch == 'q') {
            break;
        }
        wrefresh(input_win);
    }
    show_function_info(current_function);
}



void init_terminal_windows()
{
    initscr();                       // Initialize the window
    cbreak();                        // Disable line buffering
    noecho();                        // Don't echo input back to the terminal
    keypad(stdscr, TRUE);            // Enable special keys like arrow keys
    signal(SIGWINCH, handle_resize); // Handle window resize
    signal(SIGINT, handle_sigint);   // Handle Ctrl+C

    // Get the size of the terminal window
    getmaxyx(stdscr, parent_y, parent_x);

    // Create the main and input windows
    main_win = newwin(parent_y - INPUT_WINDOW_HEIGHT, parent_x, 0, 0);
    input_win = newwin(INPUT_WINDOW_HEIGHT, parent_x, parent_y - INPUT_WINDOW_HEIGHT, 0);

    details_win = newwin(details_win_height, parent_x, 0, 0);
    instructions_win = newwin(parent_y - details_win_height - 1, parent_x, instructions_win_start_y, 0);
    int registers_win_start_y, call_stack_win_start_y, memory_win_start_y, help_win_start_y, stack_win_start_y = 0;
    registers_win = newwin(parent_y - details_win_height - 1, parent_x, registers_win_start_y, 0);
    call_stack_win = newwin(parent_y - details_win_height - 1, parent_x, call_stack_win_start_y, 0);
    memory_win = newwin(parent_y - details_win_height - 1, parent_x, memory_win_start_y, 0);
    help_win = newwin(parent_y - details_win_height - 1, parent_x, help_win_start_y, 0);
    stack_win = newwin(parent_y - details_win_height - 1, parent_x, stack_win_start_y, 0);




    // Create boxes around the windows
    box(main_win, 0, 0);
    box(input_win, 0, 0);

    // Display the initial prompt in the input window
    mvwprintw(input_win, 1, 1, "<input># ");
    wrefresh(input_win);

    // Display the assembly instructions in the main window
    // display_assembly(main_win);
    display_project_info();
    // Place cursor in the input window
    wmove(input_win, 1, 10);
    wrefresh(input_win);
    
    // Main loop for capturing user input
    while (1)
    {
        // for the given mode
        switch (mode) {
            case MODE_NONE:
                get_user_input(input_win);
                break;
            case MODE_PROJECT_EDIT:
                display_project_info();
                break;
            case MODE_FUNCTION_EDIT:
                //show_function_info(current_function);
                edit_function_parameters();
                break;
            case MODE_INSTRUCTION_EDIT:
                edit_function_instructions();
                break;
        }

    }

    cleanup();
}

int main()
{

    //init_project(&p, "test");
    deserialize_project(&p, "test/project.bin");

    Function f;
    init_function(&f);

    current_function = &f;

    init_terminal_windows();

    //serialize_project(&p, "test/project.bin");

    clean_project(&p);

    return 0;
}

/*
     initscr();            // Initialize the window
     cbreak();
     noecho();

     int height = LINES, width = COLS / 2, start_y = 0, start_x = 0;
     WINDOW *left_win = newwin(height, width, start_y, start_x);
     WINDOW *right_win = newwin(height, width, start_y, start_x + width);

     box(left_win, 0, 0);
     box(right_win, 0, 0);

     mvwprintw(left_win, 1, 1, "Left Window");
     mvwprintw(right_win, 1, 1, "Right Window");

     refresh();
     wrefresh(left_win);
     wrefresh(right_win);

     WINDOW *input_win = newwin(1, width * 2, height - 1, 0);
     box(input_win, 0, 0);
     wrefresh(input_win);

     getch();              // Wait for user input

     delwin(left_win);
     delwin(right_win);
     delwin(input_win);

     endwin();             // End curses mode
     return 0;
     */
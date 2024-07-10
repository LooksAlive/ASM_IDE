#include <stdio.h>

#include "assembly.h"

#include <signal.h>
#include <ncurses.h>

static Project p;

static Function *current_function = NULL;
Instruction *current_instruction = NULL;

#define INPUT_WINDOW_HEIGHT 3

WINDOW *main_win, *input_win, *details_win, *instructions_win;
int parent_x, parent_y;
int details_win_height = 10;  // Adjust height as needed
int instructions_win_start_y;
int main_win_current_column, main_win_current_row;

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

    return NONE;
}

void get_instruction_address(Instruction *instr, char *buffer) {

    // Format the address using snprintf for safer bounds checking
    int bytes_written = snprintf(buffer, 30, "%08lX", instr->instruction_start_offset);

    // Ensure successful formatting (optional, but recommended for robustness)
    if (bytes_written < 0) {
        // Handle error (e.g., return NULL)
        return NULL;
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
void get_instruction_operands(Instruction *instr, char* operands)
{
    if(instr->data == NULL) {
        sprintf(operands, " ");
        return;
    }
    size_t len = 0;
    // switch for i_type
    switch (instr->i_type)
    {
    case MOV:
    {    
        DataMovement_s *dm = (DataMovement_s *)instr->data;
        // switch for Node* NodeType in dm attributes
        switch (dm->dest.type)
        {
        case L_REG:
            sprintf(operands, "%s,", dm->dest.data.reg.name);
            break;
        case C_VAR:
            sprintf(operands, "%s,", dm->dest.data.var.name);
            break;
        case CONSTANT:
            sprintf(operands, "%llu,", dm->dest.data.offset);
            break;
        default:
            sprintf(operands, "UNKNOWN");
            break;
        }
        // switch for Node* NodeType in dm attributes
        switch (dm->src.type)
        {
        case L_REG:
            sprintf(operands + len, "%s", dm->src.data.reg.name);
            break;
        case C_VAR:
            sprintf(operands + len, "%s", dm->src.data.var.name);
            break;
        case CONSTANT:
            sprintf(operands + len, "%llu", dm->src.data.offset);
            break;
        default:
            sprintf(operands + len, "UNKNOWN");
            break;
        }
        // switch for Node* NodeType in dm attributes --> src_bytes_to_move
        switch (dm->src_bytes_to_move.type)
        {
        case CONSTANT:
            sprintf(operands + len, ",%llu", dm->src_bytes_to_move.data.offset);
            break;
        default:
            sprintf(operands + len, ",UNKNOWN");
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
        switch (arith->dest.type)
        {
        case L_REG:
            sprintf(operands, "%s", arith->dest.data.reg.name);
            break;
        case C_VAR:
            sprintf(operands, "%s", arith->dest.data.var.name);
            break;
        case CONSTANT:
            sprintf(operands, "%llu", arith->dest.data.offset);
            break;
        default:
            sprintf(operands, "UNKNOWN");
            break;
        }
        for (int i = 0; i < arith->src_size; i++)
        {
            switch (arith->src[i].type)
            {
            case L_REG:
                sprintf(operands + len, ",%s", arith->src[i].data.reg.name);
                break;
            case C_VAR:
                sprintf(operands + len, ",%s", arith->src[i].data.var.name);
                break;
            case CONSTANT:
                sprintf(operands + len, ",%llu", arith->src[i].data.offset);
                break;
            default:
                sprintf(operands + len, ",UNKNOWN");
                break;
            }
        }
        break;
    }
    break;
    }
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
        get_instruction_address(instr, addr);
        char operands[300];
        get_instruction_operands(instr, operands);
        mvwprintw(win, i + 2, 1, "%-10s %-15s %-10s", addr, get_instruction_name(instr), operands);
    }

    wrefresh(win);
}

void show_function_info(Function *func) {
    wclear(main_win);
    wrefresh(input_win);

    // Get terminal size
    getmaxyx(stdscr, parent_y, parent_x);

    // Calculate positions and sizes
    instructions_win_start_y = details_win_height + 1;

    // Create windows
    details_win = newwin(details_win_height, parent_x, 0, 0);
    instructions_win = newwin(parent_y - details_win_height - 1, parent_x, instructions_win_start_y, 0);

    // Display function details
    display_function_details(details_win, func);
    
    // Display instructions
    display_instructions(instructions_win, func);

    // Cleanup
    delwin(details_win);
    delwin(instructions_win);
    //endwin();
}

void analyze_user_input(char *input);
void get_user_input(WINDOW *input_win)
{
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
    char *token = strtok(input, " ");
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
        else if (strcmp(token, kw_show) == 0)
        {
            // Handle "show" keyword
            token = strtok(NULL, " \n\0");
            // project
            if (strcmp(token, "project") == 0)
            {
                // Handle "show project" keyword
                token = strtok(NULL, " \n\0");
                if (&p == 0)
                {
                    // error message to main_win
                    mvwprintw(main_win, 1, 1, "No project loaded.");
                }
                else
                {
                    display_project_info();
                }
            }
            // function 
            else if (strcmp(token, kw_function) == 0) {
                // Handle "show function" keyword
                token = strtok(NULL, " \n\0");
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
        else if (strcmp(token, kw_edit) == 0)
        {
            // Handle "edit" keyword
        }
        else if (strcmp(token, kw_set) == 0)
        {
            // Handle "set" keyword
        }
        else if (strcmp(token, kw_delete) == 0) {

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
        get_user_input(input_win);
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
#include <stdio.h>

#include "assembly.h"

#include <signal.h>
#include <ncurses.h>

static Project p;

static Function *current_function = NULL;
Instruction *current_instruction = NULL;

#define INPUT_WINDOW_HEIGHT 3

WINDOW *main_win, *input_win;
int parent_x, parent_y;
int main_win_current_column, main_win_current_row;
char *instruction_list[][3]; // instructions but as char* in terminal in 3 parts, navigating with rows,cols and positions are the same as in function instructions

void cleanup()
{
    delwin(main_win);
    delwin(input_win);
    endwin();
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

    // Redisplay the project info or other content in the main window
    display_project_info("Project Info: Placeholder"); // Replace with actual project info
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

char *get_instruction_address(Instruction *instr)
{
    char *address = malloc(10);
    sprintf(address, "0x%04X ", instr->instruction_start_offset);
    return address;
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
char *get_instruction_operands(Instruction *instr)
{
    char *operands = malloc(100);

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
            sprintf(operands, "%s", dm->src.data.reg.name);
            break;
        case C_VAR:
            sprintf(operands, "%s", dm->src.data.var.name);
            break;
        case CONSTANT:
            sprintf(operands, "%llu", dm->src.data.offset);
            break;
        default:
            sprintf(operands, "UNKNOWN");
            break;
        }
        // switch for Node* NodeType in dm attributes --> src_bytes_to_move
        switch (dm->src_bytes_to_move.type)
        {
        case CONSTANT:
            sprintf(operands, ", %llu", dm->src_bytes_to_move.data.offset);
            break;
        default:
            sprintf(operands, ", UNKNOWN");
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
                sprintf(operands, "%s,%s", operands, arith->src[i].data.reg.name);
                break;
            case C_VAR:
                sprintf(operands, "%s,%s", operands, arith->src[i].data.var.name);
                break;
            case CONSTANT:
                sprintf(operands, "%s%,llu", operands, arith->src[i].data.offset);
                break;
            default:
                sprintf(operands, "%sUNKNOWN", operands);
                break;
            }
        }
        break;
    }
    break;
    }
    return operands;
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

char *(*construct_instruction_string(Instruction *instr))[1][3]
{ // Function declaration with pointer to 2D array return type
    static char *instruction_string[][3] = {0, 0, 0};

    instruction_string[0][0] = get_instruction_address(instr);
    instruction_string[0][1] = get_instruction_name(instr);

    char *operands = get_instruction_operands(instr);
    instruction_string[0][2] = operands;
    free(operands);

    return instruction_string;
}

Instruction *construct_instruction_from_strings(const char *address_str, const char *name_str, const char *operands_str) {
    Instruction *instr = (Instruction *)malloc(sizeof(Instruction));
    instr->instruction_start_offset = parse_instruction_address(address_str);
    instr->i_type = get_instruction_type_from_name(name_str);

    parse_instruction_operands(operands_str, instr);

    return instr;
}


void display_assembly(WINDOW *win)
{
    // Sample assembly code
    const char *instructions[][3] = {
        {"Address", "Instruction", "Operands"},
        {"0x0001", "MOV", "AX, BX"},
        {"0x0002", "ADD", "AX, 1"},
        {"0x0003", "SUB", "AX, BX"},
        {"0x0004", "JMP", "0x0001"},
    };
    int num_instructions = sizeof(instructions) / sizeof(instructions[0]);

    // Display the table header
    wattron(win, A_BOLD);
    mvwprintw(win, 1, 1, "%-10s %-15s %-10s", instructions[0][0], instructions[0][1], instructions[0][2]);
    wattroff(win, A_BOLD);
    /*
    // Display the instructions in the window
    for (int i = 1; i < num_instructions; ++i) {
        mvwprintw(win, i + 1, 1, "%-10s %-15s %-10s", instructions[i][0], instructions[i][1], instructions[i][2]);
    }
    */
    for (int i = 0; i < current_function->instructions_size; i++)
    {
        Instruction *instr = &current_function->instructions[i];
        char* (*instruction_string)[1][3] = construct_instruction_string(instr);
        mvwprintw(win, i + 1, 1, "%-10s %-15s %-10s", instruction_string[0][0], instruction_string[0][1], instruction_string[0][2]);
    }

    wrefresh(win);
    // clear window so that it will be empty:
    // wclear(win);
    // wrefresh(win);
}

void get_user_input(WINDOW *input_win)
{
    char input[80];
    mvwgetnstr(input_win, 1, 10, input, sizeof(input) - 1); // Get user input
    // Display the user input back in the input window
    mvwprintw(input_win, 1, 1, "<input># %s", input);
    wrefresh(input_win);
}

void display_project_info()
{
    box(main_win, 0, 0);
    mvwprintw(main_win, 2, 2, "Project info:");
    char file_size_str[64];
    sprintf(file_size_str, "%llu", p.file_size);
    mvwprintw(main_win, 4, 2, "File size: %s", file_size_str);
    char main_function_str[64];
    sprintf(main_function_str, "%llu", p.main_function);
    mvwprintw(main_win, 5, 2, "Main function: %s", main_function_str);
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
                if (p.main_function != 0)
                {
                    mvwprintw(main_win, 1, 1, "Main function already exists.");
                }
                else
                {
                    // add main function
                    create_function(&p, "main");
                    p.main_function = 1;

                    mvwprintw(main_win, 1, 1, "Main function added.");
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
                }
                else {
                    // get function ull offset, read it and display



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
        else
        {
            mvwprintw(main_win, 1, 1, "Invalid command.");
        }

        token = strtok(NULL, " ");
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
        int ch = wgetch(input_win);
        if (ch == '\n' || ch == KEY_ENTER)
        { // Handle Enter key
            get_user_input(input_win);
            // Reset input prompt
            mvwprintw(input_win, 1, 1, "<input># ");
            wclrtoeol(input_win);    // Clear to end of line
            wmove(input_win, 1, 10); // Move cursor back to input position
            wrefresh(input_win);
        }
        else if (ch == 27)
        {          // Handle Escape key (ASCII 27)
            break; // Exit loop
        }
        else if (isprint(ch))
        { // Handle printable characters
            waddch(input_win, ch);
            wrefresh(input_win);
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
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
char** instruction_list;    // instructions but as char* in terminal, navigating with rows,cols and positions are the same as in function instructions




void cleanup() {
    delwin(main_win);
    delwin(input_win);
    endwin();
}

void handle_resize(int sig) {
    endwin();
    refresh();
    clear();

    getmaxyx(stdscr, parent_y, parent_x);

    wresize(main_win, parent_y - INPUT_WINDOW_HEIGHT, parent_x);
    mvwin(main_win, 0, 0);
    wresize(input_win, INPUT_WINDOW_HEIGHT, parent_x);
    mvwin(input_win, parent_y - INPUT_WINDOW_HEIGHT, 0);

    box(main_win, 0, 0);
    box(input_win, 0, 0);

    mvwprintw(input_win, 1, 1, "<input># ");
    wrefresh(input_win);

    display_assembly(main_win);
}

void handle_sigint(int sig) {
    cleanup();
    exit(0);
}

char* get_instruction_name(Instruction *instr) {
    switch (instr->i_type) {
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

char* get_instruction_address(Instruction *instr) {
    char *address = malloc(10);
    sprintf(address, "0x%04X ", instr->instruction_start_offset);
    return address;
}

// construct operands from void* data, mostly specific type e.g. DataMovement_s and then Node*
char* get_instruction_operands(Instruction *instr) {
    char *operands = malloc(100);

    // switch for i_type
    switch (instr->i_type) {
        case MOV: {
            DataMovement_s *dm = (DataMovement_s*) instr->data;
            // switch for Node* NodeType in dm attributes
            switch (dm->dest.type) {
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
            switch (dm->src.type) {
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
            switch (dm->src_bytes_to_move.type) {
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
        case MUL: {
            Arithmetic_s *arith = (Arithmetic_s*) instr->data;
            switch (arith->dest.type) {
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
            for(int i=0; i< arith->src_size; i++) {
                switch (arith->src[i].type) {
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

char* construct_instruction_string(Instruction *instr) {
    char *instruction_string = malloc(100);
    char *address = get_instruction_address(instr);
    char *name = get_instruction_name(instr);
    char *operands = get_instruction_operands(instr);

    sprintf(instruction_string, "%s%-15s %-10s", address, name, operands);
    return instruction_string;
}

void display_assembly(WINDOW *win) {
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

    // Display the instructions in the window
    for (int i = 1; i < num_instructions; ++i) {
        mvwprintw(win, i + 1, 1, "%-10s %-15s %-10s", instructions[i][0], instructions[i][1], instructions[i][2]);
    }
    /*
    for(int i = 0; i < current_function->instructions_size; i++) {
        Instruction *instr = &current_function->instructions[i];
        char *instruction_string = construct_instruction_string(instr);
        mvwprintw(win, i + 1, 1, "%s", instruction_string);
    }*/

    wrefresh(win);
    // clear window so that it will be empty:
    //wclear(win);
    //wrefresh(win);
}

void get_user_input(WINDOW *input_win) {
    char input[80];
    mvwgetnstr(input_win, 1, 10, input, sizeof(input) - 1); // Get user input
    // Display the user input back in the input window
    mvwprintw(input_win, 1, 1, "<input># %s", input);
    wrefresh(input_win);
}

char* get_project_info() {
    char *project_info = malloc(2000);

    sprintf(project_info, "Project info:\n");
    sprintf(project_info, "%s\n", "File size: ");
    sprintf(project_info, "%s%llu\n", project_info, p.file_size);
    sprintf(project_info, "%s\n", "Main function: ");
    sprintf(project_info, "%s%llu\n", project_info, p.main_function);
    sprintf(project_info, "%s\n", "Files data dynamic: ");
    for (int i = 0; i < p.files_data_dynamic_size; i++) {
        sprintf(project_info, "%s%s\n", project_info, p.files_data_dynamic[i]);
    }
    sprintf(project_info, "%s\n", "Files bss constants: ");
    for (int i = 0; i < p.files_bss_constants_size; i++) {
        sprintf(project_info, "%s%s\n", project_info, p.files_bss_constants[i]);
    }    sprintf(project_info, "%s\n", "Files functions: ");
    for (int i = 0; i < p.files_functions_size; i++) {
        sprintf(project_info, "%s%s\n", project_info, p.files_functions[i]);
    }
    sprintf(project_info, "%s\n", "Opened functions: ");
    for (int i = 0; i < p.opened_functions_size; i++) {
        sprintf(project_info, "%s%s\n", project_info, p.opened_functions[i].name);
    }
    sprintf(project_info, "%s\n", "Free space file: ");
    sprintf(project_info, "%s%s\n", project_info, p.free_space_file);
    sprintf(project_info, "%s\n", "Project directory: ");
    sprintf(project_info, "%s%s\n", project_info, p.project_directory);
    

    return project_info;
}

void analyze_user_input(char *input) {
    // set keywords from user
    const char* kw_project = "project";
    const char* kw_load = "load";
    const char* kw_show = "show";
    const char* kw_save = "save";
    const char* kw_quit = "quit";
    const char* kw_help = "help";
    const char* kw_list = "list";
    const char* kw_edit = "edit";
    const char* kw_display = "display";
    const char* kw_run = "run";
    const char* kw_step = "step";
    const char* kw_next = "next";
    const char* kw_step_over = "step_over";
    const char* kw_step_out = "step_out";
    const char* kw_breakpoint = "breakpoint";
    const char* kw_delete = "delete";
    const char* kw_add = "add";
    const char* kw_main = "main";
    const char* kw_function = "function";
    const char* kw_variable = "variable";
    const char* kw_structure = "structure";
    const char* kw_set = "set";

    // TODO:
    // 1. Parse the input string
    char *token = strtok(input, " ");
    while (token != NULL) {
        if (strcmp(token, kw_project) == 0) {
            // move to next token
            token = strtok(NULL, " \n\0");
            // Handle "project" keyword
        } else if (strcmp(token, kw_load) == 0) {
            // Handle "load" keyword
        } else if (strcmp(token, kw_save) == 0) {
            // Handle "save" keyword
        } else if (strcmp(token, kw_quit) == 0) {
            // Handle "quit" keyword
        } else if (strcmp(token, kw_help) == 0) {
            // Handle "help" keyword
        } else if (strcmp(token, kw_list) == 0) {
            // Handle "list" keyword
        } else if (strcmp(token, kw_display) == 0) {
            // Handle "display" keyword
        } else if (strcmp(token, kw_run) == 0) {
            // Handle "run" keyword
        } else if (strcmp(token, kw_step) == 0) {
            // Handle "step" keyword
        } else if (strcmp(token, kw_next) == 0) {
            // Handle "next" keyword
        } else if (strcmp(token, kw_step_over) == 0) {
            // Handle "step_over" keyword
        } else if (strcmp(token, kw_step_out) == 0) {
            // Handle "step_out" keyword
        } else if (strcmp(token, kw_breakpoint) == 0) {
            // Handle "breakpoint" keyword
        } else if (strcmp(token, kw_delete) == 0) {
            // Handle "delete" keyword
        } else if (strcmp(token, kw_add) == 0) {
            token = strtok(NULL, " \n\0");
            // add main function
            if (strcmp(token, kw_main) == 0) {
                // Handle "add main" keyword
                if(p.main_function != 0) {
                    mvwprintw(main_win, 1, 1, "Main function already exists.");
                }
                else {
                    p.main_function = 1;
                    mvwprintw(main_win, 1, 1, "Main function added.");
                }
            }
            // Handle "add" keyword
        } else if (strcmp(token, kw_function) == 0) {
            // Handle "function" keyword
        } else if (strcmp(token, kw_variable) == 0) {
            // Handle "variable" keyword
        } else if (strcmp(token, kw_structure) == 0) {
            // Handle "structure" keyword
        }else if (strcmp(token, kw_show) == 0) {
            // Handle "show" keyword
            // project
            if (strcmp(token, "project") == 0) {
                // Handle "show project" keyword
                if(&p == 0) {
                    // error message to main_win
                    mvwprintw(main_win, 1, 1, "No project loaded.");
                }
                else {

                }
            }
        } else if (strcmp(token, kw_edit) == 0) {
            // Handle "edit" keyword
        } else if (strcmp(token, kw_set) == 0) {
            // Handle "set" keyword
        } else {
            mvwprintw(main_win, 1, 1, "Invalid command.");
        }

        token = strtok(NULL, " ");
    }
}



void init_terminal_windows() {
    initscr();            // Initialize the window
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input back to the terminal
    keypad(stdscr, TRUE); // Enable special keys like arrow keys
    signal(SIGWINCH, handle_resize); // Handle window resize
    signal(SIGINT, handle_sigint); // Handle Ctrl+C

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
    display_assembly(main_win);

    // Place cursor in the input window
    wmove(input_win, 1, 10);
    wrefresh(input_win);

    // Main loop for capturing user input
    while (1) {
        int ch = wgetch(input_win);
        if (ch == '\n' || ch == KEY_ENTER) { // Handle Enter key
            get_user_input(input_win);
            // Reset input prompt
            mvwprintw(input_win, 1, 1, "<input># ");
            wclrtoeol(input_win); // Clear to end of line
            wmove(input_win, 1, 10); // Move cursor back to input position
            wrefresh(input_win);
        } else if (ch == 27) { // Handle Escape key (ASCII 27)
            break; // Exit loop
        } else if (isprint(ch)) { // Handle printable characters
            waddch(input_win, ch);
            wrefresh(input_win);
        }
    }

    cleanup();
}

int main() {

    
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
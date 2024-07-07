#include <stdio.h>

#include "assembly.h"

#include <ncurses.h>


static Project p;

static Function* current_function = NULL;
Instruction* current_instruction = NULL;

int main(int argc, char** argv)
{
    printf("Hello World!\n");

    //Project p;
    //init_project(&p, "test");
    //clean_project(&p);

    deserialize_project(&p, "test/project.bin");

    Function f;
    init_function(&f);

    current_function = &f;


    clean_project(&p);

    return 0;
}

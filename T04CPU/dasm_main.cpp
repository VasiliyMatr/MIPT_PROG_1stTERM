#include "DASM.h"

int main()
{
    const char      in_name[100]        = "..\\files\\asm_g.txt";

    DASM_t DASM;

    DASM.dasm (in_name);

    return 1;
}
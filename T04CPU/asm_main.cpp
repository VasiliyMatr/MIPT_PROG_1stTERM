#include "ASM.h"

int main()
{
    const char      in_asm_name[100]    = "..\\files\\g.txt";

    ASM_t* ASM = asm_constructor ();

    asm_compile (ASM, in_asm_name);

    asm_destructor (ASM);

    return 1;
}
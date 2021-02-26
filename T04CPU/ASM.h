#pragma once

#include <math.h>

#include "utils.h"
#include "safe_stash.h"
#include "PARSED_F_funcs.h"

#define ASM_ASSERT(ASM)                               \
{                                                     \
    if (isBadReadPtr (ASM))                           \
        return MEM_ERR;                               \
}

struct ASM_t;

ASM_t* asm_constructor();

ERR_t asm_destructor( ASM_t* const ASM );

ERR_t asm_open_file( ASM_t* const ASM, const char* file_name );

ERR_t asm_compile( ASM_t* const ASM, const char* file_name );

ERR_t print_info( double* const asm_buff, char* const lst_buff, const char* name, const double argument, const int num, const int numof_readen_args, size_t* const lst_shft );
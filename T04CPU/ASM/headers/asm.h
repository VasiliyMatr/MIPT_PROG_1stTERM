
#ifndef ASM_INCL
#define ASM_INCL

#include "tokenization.h"

enum TR_ID_ : unsigned int
{
    #define DEF_TR( NAME_, STR_, TR_ID_ ) \
    TR_##NAME_##_ = TR_ID_,

    #include "DSL_TR.h"
    #undef DEF_TR
};

enum CMD_ID_ : unsigned int
{
    #define DEF_CMD( NAME_, STR_, CMD_ID_, CMD_CPU_, CMD_ASM_ ) \
    CMD_##NAME_##_ = CMD_ID_,

    #include "DSL_CMD.h"
    #undef DEF_CMD
};

struct Label_t
{
    char    name[NAME_SIZE_] = "";
    int     offset           = -1;
};

struct Proc_t
{
    char   name[NAME_SIZE_] = "";
    int    offset = -1;
    int    end    = -1;
};

const size_t NUMOF_LABELS_ = 512;
const size_t NUMOF_PROCES_ = 64;

struct AsmData_t
{
    /* files names */
    char* in_name_  = nullptr;
    char* out_name_ = nullptr;

    /* parsed tokens */
    Token_t* tokens_  = nullptr;
    size_t   tok_num_ = 0;

    /* labels and procs */
    Label_t labels_[NUMOF_LABELS_] = {};
    Proc_t  procs_ [NUMOF_PROCES_] = {};
};

err_t assemble( AsmData_t* data );
err_t firstPass( AsmData_t* data );

int getProc( Proc_t proces[NUMOF_PROCES_], char name[NAME_SIZE_] );
int getLabel( Label_t labels[NUMOF_LABELS_], char name[NAME_SIZE_] );

/* to parce complicated arguments, such as [ R00 + 10000 ] */
unsigned int prcArg( Token_t* toks, size_t* toks_shift, unsigned int* arg_const );
/* To parce next types of tok sequences: REG; REG + CONST; REG - CONST; CONST */
unsigned int getRegNCnst( Token_t* toks, size_t* toks_shift );

err_t run( AsmData_t* data );

#endif

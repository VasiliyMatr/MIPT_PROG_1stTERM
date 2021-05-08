
#ifndef TOKENIZATION_INCL
#define TOKENIZATION_INCL

#include "utils.h"

const size_t NAME_SIZE_ = 16;

/* tokens types */
enum TokenType_t
{
    /* num const */
    NUM_    = 'N',
    /* unknown identifier */
    IDENT_  = 'I',
    /* in-build cmds: if, cycles, var declarations, ariph operations */
    CMD_    = 'C',
    /* terminal symbol */
    TERM_   = 'T',

    /* init value */
    UNDF_T_ = 'U'
};

/* lexical token type */
struct Token_t
{
    /* this token type */
        TokenType_t     type = UNDF_T_;
    /* name - for identifier */
        char            name[NAME_SIZE_] = {};
    /* number - for consts */
        int             number   = 0;
    /* id - for cmds */
        size_t          id       =  0;
};

/* poison value */
const Token_t  POISON_ = { UNDF_T_, "undef", 0 };

/* func to tokenize code */
err_t   tokenize( Token_t** ptr_2_toks_ms, size_t* toks_num, const char* const code );

/* func to get symbols sequence from code buff */
int     get_sequence( char *dest, const char* const code, const int input_formats_first_id );

#endif
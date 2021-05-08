
#ifndef TOKENIZATION_H_INCL
#define TOKENIZATION_H_INCL

#include "loc_utils.hpp"

struct Compiler;

/* tokens types */
enum TokenType_t
{
    /* num const */
    NUM_    = 'N',
    /* unknown identifier */
    IDENT_  = 'I',
    /* in-build keywords: if, cycles, var declarations, ariph operations */
    KEY_    = 'O',
    /* terminal symbol */
    TERM_   = 'T',

    /* init value */
    UNDF_T_ = 'U'
};

/* data to store in token (name or number) */
union TokenData_t
{
    /* for identifiers */
    NamePart_t name[NAME_LEN_] = { UNDEF_NAME_[0], UNDEF_NAME_[1] };
    /* for consts */
    int        number;
};

/* tree node data type */
struct Token_t
{
    /* this unit type */
    TokenType_t type = UNDF_T_;
    /* for errors out */
    int         location = -1;
    /* token data */
    TokenData_t data;
};

/* typedef for tree_t struct funcs */
typedef Token_t data_t;

/* poison value */
const   data_t POISON_ = { UNDF_T_, -1, 0 };

/* func to tokinize code */
Err_t   tokenize( Compiler* cmplp, const char* const expr );

/* func to get symbols sequence from code buff */
int     getSequence( NamePart_t name[NAME_LEN_], const char* const code, const int input_formats_first_id );

#endif
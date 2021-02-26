
/* code style features :
 * exampleName      - funcs
 * ExampleName_t    - data type
 * ExampleName      - struct with many connected funcs
 * EXAMPLE_NAME_    - global vars, define consts and define args
 * EXAMPLE_NAME     - defines
 */

#ifndef COMPILER_INCLUDE
#define COMPILER_INCLUDE

/* DECLARATIONS THAT ARE NEEDED TO BE DONE BEFORE tree.h INCLUDE ( NEEDED TYPEDEFS ARE HERE )*/

/* name part type (for simplification and speed long long is used to store identifiers and keywords, 
 * also check DSL.h file)
 */
typedef         long long       NamePart_t     ;
/* num of parts in full name */
static const    size_t          NAME_LEN_ = 2  ;

/* should be defined */
#ifndef IS_LITTLE_ENDIAN_
    #error IS_LITTLE_ENDIAN_ should be defined as true or false
#endif

/* names constr algorythm */
    #if IS_LITTLE_ENDIAN_
        #define CODE( NAME_ )                                             \
            ((NamePart_t)NAME_[0]      ) | ((NamePart_t)NAME_[1] << 8 ) | \
            ((NamePart_t)NAME_[2] << 16) | ((NamePart_t)NAME_[3] << 24) | \
            ((NamePart_t)NAME_[4] << 32) | ((NamePart_t)NAME_[5] << 40) | \
            ((NamePart_t)NAME_[6] << 48) | ((NamePart_t)NAME_[7] << 56)
    #else
        #define CODE( NAME_ )                                             \
            ((NamePart_t)NAME_[7]      ) | ((NamePart_t)NAME_[6] << 8 ) | \
            ((NamePart_t)NAME_[5] << 16) | ((NamePart_t)NAME_[4] << 24) | \
            ((NamePart_t)NAME_[3] << 32) | ((NamePart_t)NAME_[2] << 40) | \
            ((NamePart_t)NAME_[1] << 48) | ((NamePart_t)NAME_[0] << 56)
    #endif

/* define to count byte shift for proper NAME_PART_ construct depending on endian type */
#if IS_LITTLE_ENDIAN_
    #define BYTE_SHT( NAME_LEN_, CH_ID_ ) (CH_ID_)
#else
    #define BYTE_SHT( NAME_LEN_, CH_ID_ ) (NAME_LEN_ - CH_ID_ - 1)
#endif

/* info about operands for DSL */
const           size_t          BIN_ = 0x3; /* 11 in binary representation */
const           size_t          UNL_ = 0x2; /* 10 in binary representation */
const           size_t          UNR_ = 0x1; /* 01 in binary representation */

#include "loc_utils.h"
#include <math.h>

/* data description str max len - for graphical dump */
const size_t    DESCR_LEN_      =  64;
/* len of operation name */
const size_t    NAME_SIZE_      =  sizeof (NamePart_t) * NAME_LEN_;
/* init num of tokens */
const size_t    NUMOF_TOC_      =  64;
/* max num of funcs */
const size_t    NUMOF_FUNCS_    =  64;
/* max num of variables */
const size_t    NUMOF_VARS_     =  64;

/* tokens types */
enum TokenType_t
{
    /* num const */
    NUM_    =   'N',
    /* unknown identifier */
    IDENT_  =   'I',
    /* in-build keywords: if, cycles, var declarations, ariph operations */
    KEY_    =   'O',
    /* terminal symbol */
    TERM_   =   'T',

    /* init value */
    UNDF_T_ =   'U'
};

/* names splited in 2 parts => 16 unic symbols for names, can be changed,
 * now all is made with unic keywords and terminal sequences first parts for fast and simple code
 */
/* keywords declaration */
#define DEF_KEY( NAME_, CODE_FT_, CODE_SD_, OPERANDS_ ) \
    static constexpr NamePart_t KEY##_##NAME_[NAME_LEN_] = { CODE_FT_, CODE_SD_ };
#include "DSL_KEY.h"
#undef DEF_KEY


/* terminal words declaration */
#define DEF_TR( NAME_, CODE_FT_, CODE_SD_ ) \
    static constexpr NamePart_t TR_##NAME_[NAME_LEN_] = { CODE_FT_, CODE_SD_ };
#include "DSL_TR.h"
#undef DEF_TR

static constexpr NamePart_t LR_         [NAME_LEN_] = { CODE ("LR\0\0\0\0\0\0") ,       0        };
static constexpr NamePart_t LEXPR_BRACE_[NAME_LEN_] = { TR_LBRACE1_[0]          , TR_LBRACE1_[1] };
static constexpr NamePart_t REXPR_BRACE_[NAME_LEN_] = { TR_RBRACE1_[0]          , TR_RBRACE1_[1] };
/* undefined name */
static constexpr NamePart_t UNDEF_NAME_ [NAME_LEN_] = { CODE ("undef\0\0\0")    ,       0        };

/* info to store in token */
union TokenInfo_t
{
    /* for identifiers */
    NamePart_t name[NAME_LEN_] = { UNDEF_NAME_[0], UNDEF_NAME_[1] };
    /* for keywords */
    NamePart_t key [NAME_LEN_];
    /* for terminal */
    NamePart_t term[NAME_LEN_];
    /* for consts */
    double     number;
};

/* tree node data type */
struct Token_t
{
    /* this unit type */
    TokenType_t   type   = UNDF_T_;
    /* token info */
    TokenInfo_t   info;
};

/* typedef for tree_t struct funcs */
typedef Token_t data_t;

/* poison value */
data_t POISON_ = { UNDF_T_, NAN };

/* func to get str with data description for graphical dump */
err_t get_data_str( const data_t data, char* const buff );

/* func to compare data */
int compare_data_f ( const data_t* const datap, const data_t* const to_compare );

/* func to copy data */
void copy_data( data_t* tocopy, data_t* dstn );

/* *********************************** */

/* !!! tree_t FUNCS INCLUDED !!! */
    #include "tree.h"

/* compiler struct */
struct Compiler
{
    /* dynamical tokens massive */
    Token_t*    tokens_ = nullptr;

    /* tree with tokens */
    tree_t      tree_;

    /* variables and functions info */
    NamePart_t  variables_[NUMOF_VARS_] [NAME_LEN_];
    NamePart_t  functions_[NUMOF_FUNCS_][NAME_LEN_];

    /* ids of last parsed var and func */
    size_t      last_var_id_ = 0;
    size_t      last_fun_id_ = 0;
};

/* shift to get to Compiler tree */
const size_t CMPL_TR_SHT_ = (size_t)&(((Compiler* )nullptr)->tree_);
/* func to get ptr to Compiler tree */
tree_t* cmpl_get_treep( const Compiler* const calc );

/* returns 0 if names are equal; +val if first name is bigger in numerical representation */
int cmpl_name_cmpr( const NamePart_t const ft_name[NAME_LEN_],
                    const NamePart_t const sd_name[NAME_LEN_] );

/* !!! recursiver_descent FUNCS INCLUDED !!! */
    #include "recursive_descent.h"

/* constructor and destructor */
err_t   cmpl_ctor( Compiler* const cmpl );
err_t   cmpl_dstr( Compiler* const cmpl );

/* func to tokinize code */
err_t   cmpl_tokinize( Compiler* const cmpl, const char* const expr );

/* func to get number from code buff */
int     cmpl_get_num( double* number, const char* const code );

/* func to get symbols sequence from code buff */
int     cmpl_get_sequence( NamePart_t name[NAME_LEN_], const char* const code, const int input_formats_first_id );

/* DEBUG STUFF */
err_t   cmpl_verify( const Compiler* const cmpl );

/* for errors output */
#define ERR_OUT( COND_, ERR_STR_ )              \
if (COND_)                                      \
{                                               \
    fprintf (stderr,                            \
    "\n\n" ERR_STR_ " in %s on line %s" "\n\n", \
    __FILE__, __LINE__);                        \
}

#ifndef NDBG

    /* verify define */
    #define verify( CMPL_, TO_RET_)                     \
    if (err_t error = cmpl_verify (CMPL_))              \
    {                                                   \
        ERR_OUT (1, "VERIFY ERROR");                    \
        fprintf (stderr, "code - %d" "\n\n", error);    \
        return TO_RET_;                                 \
    }

#else

    #define verify( CMPL_, TO_RET_ ) ;

#endif

/* INCLUDING SRC */
    #include "compiler.cpp"

#endif /* COMPILER_INCLUDE */


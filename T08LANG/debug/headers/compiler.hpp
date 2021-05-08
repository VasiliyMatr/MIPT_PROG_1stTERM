
/* code style features :
 * exampleName      - funcs
 * ExampleName_t    - data type
 * ExampleName      - struct with many connected funcs (class)
 * EXAMPLE_NAME_    - global vars, define consts and define args
 * EXAMPLE_NAME     - defines
 */

#include <cmath>

#ifndef COMPILER_H_INCL
#define COMPILER_H_INCL

#include "loc_utils.hpp"
#include "tree.hpp"
#include "tokenization.hpp"
#include "recursive_descent.hpp"
#include "translate.hpp"

/* compiler struct */
struct Compiler
{
    /* text of programm */
    char*       code_buff_   = nullptr;

    /* dynamical tokens massive */
    Token_t*    tokens_     = nullptr;

    /* tree with tokens */
    tree_t      tree_;

    /* variables and functions names */
    NamePart_t  variables_[NUMOF_VARS_ ][NAME_LEN_];
    NamePart_t  functions_[NUMOF_FUNCS_][NAME_LEN_];

    /* ids of last parsed var and func */
    size_t      last_var_id_ = 0;
    size_t      last_func_id_ = 0;

    /* last unexit cycle exit label id - used for brakes */
    int         break_label_id_ = -1;

    /* to have unique labels names */
    size_t      last_label_id_ = 0;

    /* for compile errors output */
    ErrorInfo_t error_info_ = { OK_, -1 };
};

/* shift to get to Compilers tree */
    const size_t CMPL_TR_SHT_ = (size_t)&(((Compiler* )nullptr)->tree_);
/* func to get ptr to Compiler tree */
    tree_t* cmpl_getTreep( const Compiler* const calc );

/* constructor and destructor */
    Err_t   cmpl_ctor( Compiler* const cmpl );
    Err_t   cmpl_dstr( Compiler* const cmpl );

/* DEBUG STUFF */
    Err_t   cmpl_verify( const Compiler* const cmpl );

/* All compilation stuff in one function */
    void compile( const char* inName, const char* outName, bool needTreeFlag );

#endif
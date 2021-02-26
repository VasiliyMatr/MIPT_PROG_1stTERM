
#ifndef REC_DEC_INCLUDE_
#define REC_DEC_INCLUDE_

#include <math.h>
#include <stdio.h>

#include "compiler.h"


/* compiler to work with */
Compiler*       CMPLP_     = nullptr;
/* curr token id */
size_t          POSE_     = 0;
/* for error output */
err_t           ERROR_    = OK;

/* to set up data */
err_t setData( Compiler* const calcp );

/* func to parse all code */
err_t getTree();

node_t* getFunc();

node_t* getLR();

/* EXPRESSION PARSE STUFF */
    /* func to parse expression */
    node_t* getExpr();

    /* PRIORITY LEVELS FUNCS */

        /* func to work with 3rd priority operations ( + and - ) */
        node_t* get3Prior();
        /* func to work with 2nd priority operations ( * and / ) */
        node_t* get2Prior();
        /* func to work with 1st priority operations ( ^ )*/
        node_t* get1Prior();

    /* ********************* */

    /* MAX PRIORITY FUNCS */

        /* func to check braces */
        node_t* getBraces();

    /* ****************** */

/* func to search variable by name */
int cmplSearchVar( Compiler* cmpl, NamePart_t name[NAME_LEN_] );

#include "recursive_descent.cpp"

#endif
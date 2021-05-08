
#ifndef RECURSIVE_DESCENT_H_INCLUDE_
#define RECURSIVE_DESCENT_H_INCLUDE_

#include "tree.hpp"

/* to set up data */
Err_t setData( Compiler* const calcp );

/* func to get error output */
void  getError( Compiler* const cmplp );

    /* func to parse all code */
    Err_t getTree();
    /* func to get all funcs */
    node_t* getFuncs();
    /* func to get LR */
    node_t* getLR();

/* to get syntax construction (while, if, break...) */
    node_t* getConstr();

/* EXPRESSION PARSE STUFF */
    /* func to parse expression */
    node_t* getExpr();

    /* PRIORITY LEVELS FUNCS */

        /* func to work with 4th priority operations ( assignation ) */
        node_t* get4Prior();
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
int searchVar( Compiler* cmpl, NamePart_t name[NAME_LEN_] );

#endif
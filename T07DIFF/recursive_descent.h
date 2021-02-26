
#ifndef REC_DEC_INCLUDE_
#define REC_DEC_INCLUDE_

#include <math.h>
#include <stdio.h>

#include "calctor.h"
#include "USER_STACK.h"


/* calc to parce in */
calctor_t*      CALCP_     = nullptr;


/* buff stuff */
const char*     BUFF_     = nullptr;
size_t          POSE_     = 0;

/* for error output */
err_t           ERROR_    = OK;

/* max buff len */
const size_t    MAX_BUFF_LEN_   = 500;

/* end symbol */
const char      END_            = '$';

/* to set up data */
err_t SetData( const char* newbuff, calctor_t* const calcp );

/* func to parce all expr */
err_t GetTree ();

/* PRIORITY LEVELS FUNCS */

    /* func to work with 3rd priority operations ( + and - ) */
    node_t* Get3Prior();
    /* func to work with 2nd priority operations ( * and / ) */
    node_t* Get2Prior();
    /* func to work with 1st priority operations ( ^ )*/
    node_t* Get1Prior();

/* ********************* */

/* MAX PRIORITY FUNCS */

    /* func to check braces */
    node_t* GetBraces();
    /* func to get number */
    node_t* GetNumber();
    /* func to wrok with rval funcs */
    node_t* GetFunction( NAME_ name );

/* ****************** */

/* func to parce name in needed format (LITTLE_ENDIAN or BIG_ENDIAN orders) */
NAME_ GetName();

/* func to search variable by name */
int calc_search_var( calctor_t* calc, NAME_ name );

#include "recursive_descent.cpp"

#endif
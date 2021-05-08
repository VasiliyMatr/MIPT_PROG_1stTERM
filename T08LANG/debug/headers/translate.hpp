
#ifndef TRANSLATE_H_INCL
#define TRANSLATE_H_INCL

#include "tree.hpp"

/* recoursive descent funcs to translate tree into text */
    /* to put whole tree */
        Err_t  putTree( Compiler* cmplp, const char* out_name );
    /* to put func */
        size_t putFunc( Compiler* cmplp, const node_t* nodep, char* buff );
    /* to put LR */
        size_t putLR( Compiler* cmplp, const node_t* nodep, char* buff );

    /* to put func call */
        size_t putCall( Compiler* cmplp, const node_t* nodep, char* buff );

    /* to put construction (while, if ...) */
        size_t putConstr( Compiler* cmplp, const node_t* nodep, char* buff );

    /* to put expressions */
        size_t putExpr( Compiler* cmplp, const node_t* nodep, char* buff );

    /* to get compiler var id */
        int getVarId( const Compiler* cmplp, const NamePart_t name[2] );

#endif

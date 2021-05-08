#ifndef INCLUDED_STACK_UTILS_
#define INCLUDED_STACK_UTILS_

#include <cstdio>
#include <cstdlib>
#include <cmath>

/* dbg fields types and consts */

    /* hash number type */
    typedef unsigned int                hash_t;
    /* canary      type */
    typedef const unsigned long long    canary_t;

    /* hash   init value */
    const hash_t    HASH_INIT_      = 0xA0FeeA0F;
    /* canary init value */
    const canary_t  CANARY_INIT_    = 0xC0C0C0C0C0C0C0C0;

/* ***************************** */

/* to print error info if there are no other ways */
    #define CRIT_ERROR_OUT_( MSG )          \
    fprintf (stderr, "\n\n" MSG "\n\n");

/* for addrs check */
    bool isBadPtr( const void* ptr );

/* Hash function for byte sequences */
    hash_t hashF( const void* toHash, const size_t numOfBytes );

/* funcs to get poison */
    int getPoison( int* );
    double getPoison( double* );

#endif
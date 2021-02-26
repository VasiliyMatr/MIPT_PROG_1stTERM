
#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <Windows.h>
#include <stdio.h>

typedef char* data_t;

const data_t POISON_        = nullptr;
const size_t MAX_STR_LEN_   = 100;

enum err_t
{

    /* no errors */
    OK          =  0    ,
    /* error with memory alloation */
    MEM_ERR     = -1000 ,
    /* syntax error */
    SYNT_ERR            ,
    /* bad ptr */
    BAD_PTR             ,
    /* tried to init bad tree or akin */
    BAD_INIT_ERR        ,
    /* root don't match */
    ROOT_MATCH_ERR      ,
    /* cant add node here */
    ADD_ERR             ,
    /* error with file */
    FILE_ERR            ,
    /* confurm error */
    CONF_ERR            ,
    /* search error */
    LEAF_ERR            ,
    /* prop found */
    BODY_ERR            ,
    /* bad ptr to error */
    BAD_ERR_PTR         ,

    /* VERIFICATION ERRORS */
        ROOT_ERR        ,
        LEFT_ERR        ,
        RIGH_ERR        ,
        PARENT_ERR      ,
    /* ******************* */

    /* undefined error */
    UNDEF_ERR

};

/* to check ptr validity */
bool isBadPtr( const void* ptr );

/* to get str with data description for graphical dump */
err_t get_data_str( data_t data, char* buffp );

/* func to compare data */
int compare_data_f ( const data_t* const datap, const data_t* const to_compare );

/* func to get confr from user */
err_t get_confirmation();

/* func to get line from stdin, line will be not longer than max_len and all extra symbols will be flushed */
size_t get_line( char* buff, size_t max_len );

/* func to get str form frame until user will input normal str */
size_t get_str( char* buff, size_t max_len );

/* error print util */
void error_prt( err_t error );

#endif

#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define BIG_ENDIAN 

/* BYTE ORDER CHECK */
    #ifdef  LITTLE_ENDIAN
        #define BYTE_SHT( NAME_LEN_, ch_id ) (NAME_LEN_ - ch_id - 1)
    #else
        #define BYTE_SHT( NAME_LEN_, ch_id ) (ch_id)
    #endif

/* ************* */

/* priorities */
#define MAX_PRI  1000
#define MIN_PRI -1000

/* max len of strs */
const size_t MAX_STR_LEN_   = 128;

const double Pi             = 3.1415926535;

enum err_t
{

    /* no errors */
    OK          =  0    ,
    /* error with memory alloation */
    MEM_ERR     = -1000 ,
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
    /* bad ptr to error */
    BAD_ERR_PTR         ,

    /* syntax error */
    SYNT_ERR            ,
    /* have undef data type */
    UNDF_UNIT_ERR       ,
    /* variable or number data have childs */
    CHLD_ERR            ,
    /* no such variable in diff */
    VAR_ERR             ,
    /* func not found */
    NO_FUNC             ,
    /* bad var id */
    BAD_VAR             ,

    /* VERIFICATION ERRORS */
        ROOT_ERR        ,
        LEFT_ERR        ,
        RIGH_ERR        ,
        PARENT_ERR      ,
    /* ******************* */

    
	/* stack errors */
	STACK_NULL_PRT_ERROR = -1337		,
	STACK_HASH_ERROR					,
	STACK_TOP_CANARY_DEAD				,
	STACK_BOT_CANARY_DEAD				,
	BAD_STACK							,

	/* sarray errors */
	// ptrs errors
	SARRAY_NULL_PRT_ERROR				,
	SARRAY_DATA_NULL_PTR_ERROR			,
	SARRAY_DATA_STORAGE_NULL_PTR_ERROR	,

	// hash errors
	SARRAY_HASH_ERROR					,
	SARRAY_DATA_HASH_ERROR				,
	SARRAY_DATA_STORAGE_HASH_ERROR		,
	
	// canaries errors
	SARRAY_TOP_CANARY_DEAD				,
	SARRAY_BOT_CANARY_DEAD				,

	SARRAY_DATA_STORAGE_TOP_CANARY_DEAD ,
	SARRAY_DATA_STORAGE_BOT_CANARY_DEAD ,

	SARRAY_DATA_TOP_CANARY_DEAD			,
	SARRAY_DATA_BOT_CANARY_DEAD			,

	// bad sarray used
	BAD_SARRAY							,

	// canary default errors
	TOP_CANARY_DEAD						,
	BOT_CANARY_DEAD						,

	// for DBG_MODE and log file hash errors
	SARRAY_DBG_MODE_HASH_ERROR			,
	STACK_DBG_MODE_HASH_ERROR			,
	STACK_LOG_FILE_HASH_ERROR			,

    /* empty stack */
    EMPT                                ,
    /* mior error */
    MINOR_ERROR                         ,

    /* undefined error */
    UNDEF_ERR

};

/* to check ptr validity */
bool isBadPtr( const void* ptr );

/* error print util */
void error_prt( err_t error );

#endif
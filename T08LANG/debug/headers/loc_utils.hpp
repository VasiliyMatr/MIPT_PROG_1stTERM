
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>

#ifndef LOC_UTILS_H_INCL
#define LOC_UTILS_H_INCL

/* to choose byte order depending on machine arch */
    #define IS_LITTLE_ENDIAN_ true

/* errors codes */
enum Err_t
{
    /* no errors */
    OK_          =  0   ,
    /* error with memory alloation */
    MEM_ERR_     = -0xFF ,
    /* bad ptr */
    BAD_PTR_             ,
    /* tried to init bad tree or akin */
    BAD_INIT_ERR_        ,
    /* root don't match */
    ROOT_MATCH_ERR_      ,
    /* cant add node here */
    ADD_ERR_             ,
    /* error with file */
    FILE_ERR_            ,
    /* confurm error */
    CONF_ERR_            ,
    /* search error */
    LEAF_ERR_            ,
    /* bad ptr to error */
    BAD_ERR_PTR_         ,

    /* syntax error */
    SYNT_ERR_            ,
    /* bad var id */
    BAD_VAR_             ,

    /* VERIFICATION ERRORS */
        ROOT_ERR_        ,
        LEFT_ERR_        ,
        RIGH_ERR_        ,
        PARENT_ERR_      ,

    /* undefined error */
    UNDEF_ERR_
};

struct ErrorInfo_t {

    Err_t  error_    = OK_;
    int    location_ = -1;

};

struct Compiler;

/* to check ptr validity */
    bool isBadPtr( const void* ptr );

/* for compile errors out */
    void cmplErrOut( Compiler* cmplp );

/* NAMES CONSTR ALGORYTHM AND DECLS */
    /* name part type (for simplification and speed long long is used to store identifiers and keywords, 
     * also check DSL.h file)
     */
        typedef         long long       NamePart_t     ;
    /* num of parts in full name */
        static const    size_t          NAME_LEN_ = 2  ;

    /* SIZES */
        /* data description str max len - for graphical dump */
            const int       DESCR_LEN_      =  64;
        /* len of operation name */
            const size_t    NAME_SIZE_      =  sizeof (NamePart_t) * NAME_LEN_;
        /* init num of tokens */
            const size_t    NUMOF_TOC_      =  512;
        /* max num of funcs */
            const size_t    NUMOF_FUNCS_    =  64;
        /* max num of variables */
            const size_t    NUMOF_VARS_     =  16;

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

    /* define to count byte shift for proper NAME_PART_ construct depends on endian type */
        #if IS_LITTLE_ENDIAN_
            #define BYTE_SHT( NAME_LEN_, CH_ID_ ) (CH_ID_)
        #else
            #define BYTE_SHT( NAME_LEN_, CH_ID_ ) (NAME_LEN_ - CH_ID_ - 1)
        #endif

    /* OPERANDS INFO FOR DSL */
        const           size_t          BIN_ = 3; /* 11 in binary representation */
        const           size_t          UNL_ = 2; /* 10 in binary representation */
        const           size_t          UNR_ = 1; /* 01 in binary representation */

    /* OPRS PRIORITIES */
        #define MAX_PRI  1024
        #define MIN_PRI -1024

    /* NAMES CONSTRACTION */
        /* names splited in 2 parts => 16 unic symbols for names, can be changed,
         * now all is made with unic keywords and terminal sequences first parts for fast and simple code
         */
        /* keywords declaration */
            #define DEF_KEY( NAME_, CODE_FT_, CODE_SD_, OPRS_INFO_, ASM_NAME_ ) \
                static constexpr NamePart_t KEY##_##NAME_[NAME_LEN_] = { CODE_FT_, CODE_SD_ };
            #include "DSL_KEY.h"
            #undef DEF_KEY


        /* terminal words declaration */
            #define DEF_TR( NAME_, CODE_FT_, CODE_SD_ ) \
                static constexpr NamePart_t TR_##NAME_[NAME_LEN_] = { CODE_FT_, CODE_SD_ };
            #include "DSL_TR.h"
            #undef DEF_TR

        /* this token can't be in code, but it is used in tree */
            static constexpr NamePart_t LR_         [NAME_LEN_] = { CODE ("LR\0\0\0\0\0\0") ,       0        };
        /* undefined name - used in case of errors or as init value */
            static constexpr NamePart_t UNDEF_NAME_ [NAME_LEN_] = { CODE ("undef\0\0\0")    ,       0        };

    /* returns 0 if names are equal; +val if first name is bigger in numerical representation */
        int nameCmpr( const NamePart_t ft_name[NAME_LEN_],
                      const NamePart_t sd_name[NAME_LEN_] );

#endif
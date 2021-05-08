
#ifndef UTILS_INCL
#define UTILS_INCL

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <cassert>
#include <memory.h>

enum err_t
{
    OK = 0              ,

    MEM_ERR = -4096     ,
    BAD_PTR             ,
    FILE_ERR            ,
    SYNT_ERR            ,
    CMD_ERR             ,
    STK_ERR             ,
    UNDF_CMD            ,
    SEG_FAULT           ,
};

bool isBadPtr(const void* ptr);

/* MASKS for mov cmd */
    const unsigned int FT_REG_ON_ = 1 << 15;
    const unsigned int SD_REG_ON_ = 1 << 14;

    const unsigned int FT_MEM_ON_ = 1 << 13;
    const unsigned int SD_MEM_ON_ = 1 << 12;

    const unsigned int FT_CONST_ON_ = 1 << 11;
    const unsigned int SD_CONST_ON_ = 1 << 10;

    const unsigned int FT_SIGN_PLUS_ = 1 << 9;
    const unsigned int SD_SIGN_PLUS_ = 1 << 8;

#endif
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

// for comparations //
typedef int (*cmpr_f)( const void* A, const void* B );

//  for errors  //

enum ERR_t {
	UNKNOWN_ERR = -3,
	MEM_ERR		= -1,
	FILE_ERR	= -2,
	OK			=  0
};

// struct for smart strs with length //
struct STR {
	char*		str;
	size_t		length;
};

//  to store all data from file  //
struct PARSED_FILE_t {
	FILE*  file;
	char*  data;

	STR*   strs;

	int    num_of_c;
	int    num_of_s;
};

//{----------------------------------------------------------------------------
//! for output about errors
//! @param[in]          code     - error code
//}----------------------------------------------------------------------------

int ERR_WORKER( int code );

//{----------------------------------------------------------------------------
//! compare function for qsort
//}----------------------------------------------------------------------------

int compare( const void* str_2_compare_A, const void* str_2_compare_B );

//{----------------------------------------------------------------------------
//! compare function for reversed qsort
//}----------------------------------------------------------------------------

int reversed_compare( const void* str_2_compare_A, const void* str_2_compare_B );

//{----------------------------------------------------------------------------
//! to check if the end of the str is there
//!
//! @param[in]              ch          - char to check
//!
//! @return true if there are end of str, false in opposite case
//}----------------------------------------------------------------------------

bool isend( char ch );

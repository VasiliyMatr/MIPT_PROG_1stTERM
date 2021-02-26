#pragma once

#include "utils.h"

//{============================================================================
//! function to get all data from the file
//!
//! @param[in]              in_file  - pointer to FILE
//! @param[out]             data     - pointer to array with data (for allocating and rewriting)
//!
//! @return num of chars or ERROR code
//}============================================================================

int file_reader( FILE* in_file, char* data[] );

//{============================================================================
//! function to get pointers to strs
//!
//! @param[in]              data        - char* with all file
//! @param[in]              num_of_c    - num of chars
//! @param[out]             pointers    - pointer to array of pointers
//!
//! @return num of words or ERROR code
//}============================================================================

ERR_t parser( PARSED_FILE_t* file );

//{============================================================================
//! function to count strs in PARSED_FILE_t element
//!
//! @param[in]              file        - PARSED_FILE_t element
//}============================================================================

void count_strs( PARSED_FILE_t* file );

//{============================================================================
//! function to setup pointers to strs (strs should be counted and mem should be allocated)
//!
//! @param[in]              file        - PARSED_FILE_t element
//}============================================================================

void setup_ptrs( PARSED_FILE_t* file );

//{----------------------------------------------------------------------------
//! to get file lenght (num_of_symbols)
//!
//! @param[in]              in_file     - pointer to file to count symbols in
//!
//! @return num of symbols
//}----------------------------------------------------------------------------

size_t get_length( FILE* in_file );

//{----------------------------------------------------------------------------
//! to fill all the data in PARSED_FILE_t element
//!
//! @param[in]              in_name     - name of file to read data from
//! @param[out]             file        - pointer to PARSED_FILE_t element
//!
//! @return ERROR code
//}----------------------------------------------------------------------------

ERR_t get_parsed_text( PARSED_FILE_t* file, char* in_name );

//{----------------------------------------------------------------------------
//! for properly PARSED_FILE_t element closing
//!
//! @param[in]              file - pointer to file to clear
//}----------------------------------------------------------------------------

void pf_close( PARSED_FILE_t* file );

//{----------------------------------------------------------------------------
//! for comfortable print if file
//!
//! @param[in]              out_file    - where to write
//! @param[in]              pointers    - pointers to printable strs
//! @param[in]              num_of_s    - num of strs to print
//}----------------------------------------------------------------------------

void print_in_file( FILE* out_file, const PARSED_FILE_t* in_file );

//{----------------------------------------------------------------------------
//! to sort and print in file
//!
//! @param[in]              in_file     - data
//! @param[in]              compare     - compare function for qsort
//! @param[out]             out_file    - file to print in
//}----------------------------------------------------------------------------

ERR_t sort_and_print( FILE* out_file, const PARSED_FILE_t* in_file, cmpr_f compare );
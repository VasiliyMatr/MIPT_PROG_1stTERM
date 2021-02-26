#include "TXLib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef VASYAN_DBG
    #define assert(condition) ;
#endif

//  for errors  //
const int OK       =  0;
const int MEM_ERR  = -1;
const int FILE_ERR = -2;

//  to store all data from file  //
struct PARSED_FILE_t {
    FILE* file;
    char* data;
    char** pointers;
    int num_of_c;
    int num_of_s;
};

//{============================================================================
//! function to work with files
//!
//! @param[in]          in_name  - name of file to open
//! @param[out]         out_name - name of file to write the result in
//!
//! @return ERROR code
//}============================================================================

int file_worker( char* in_name, char* out_name );

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

int parser( const char* data, char* **pointers, int num_of_c );

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

//{----------------------------------------------------------------------------
//! to get file lenght (num_of_symbols)
//!
//! @param[in]              in_file     - pointer to file to count symbols in
//!
//! @return num of symbols
//}----------------------------------------------------------------------------

int get_lenght( FILE* in_file );

//{----------------------------------------------------------------------------
//! for comfortable print if file
//!
//! @param[in]              out_file    - where to write
//! @param[in]              pointers    - pointers to printable strs
//! @param[in]              num_of_s    - num of strs to print
//}----------------------------------------------------------------------------

void print_in_file( FILE* out_file, const PARSED_FILE_t* in_file );

//{----------------------------------------------------------------------------
//! to fill all the data in PARSED_FILE_t element
//!
//! @param[in]              in_name     - name of file to read data from
//! @param[out]             file        - pointer to PARSED_FILE_t element
//!
//! @return ERROR code
//}----------------------------------------------------------------------------

int get_parsed_text( PARSED_FILE_t* file, char* in_name );

//{----------------------------------------------------------------------------
//! for properly PARSED_FILE_t element closing
//!
//! @param[in]              file - pointer to file to clear
//}----------------------------------------------------------------------------

void pf_close( PARSED_FILE_t* file );

//{----------------------------------------------------------------------------
//! to print str in file (isend () is used to check str end)
//!
//! @param[in]              str - str   - to print
//! @param[out]             out_file    - file to write in
//}----------------------------------------------------------------------------

void my_print_str( FILE* out_file, const char str[] );

//{----------------------------------------------------------------------------
//! to sort and print in file
//!
//! @param[in]              in_file     - data
//! @param[in]              compare     - compare function for qsort
//! @param[out]             out_file    - file to print in
//}----------------------------------------------------------------------------

void sort_and_print( FILE* out_file, const PARSED_FILE_t* in_file, int (*compare)( const void*, const void* ) );

//{----------------------------------------------------------------------------
//! for case insensitive compare for alphabet
//!
//! @param[in]              f_letter    - first letter to compare
//! @param[in]              s_letter    - second letter to compare
//!
//! @return f_letter > s_letter or FALSE if one of inputs isn't alphabet symbol
//}----------------------------------------------------------------------------

bool no_reg_comp( char f_letter , char s_letter );

//{----------------------------------------------------------------------------
//! ...oh, those tests...
//}----------------------------------------------------------------------------

void unit_tests(char* out_name, char* in_name);

//{----------------------------------------------------------------------------
//! ...oh, those tests...
//}----------------------------------------------------------------------------

bool alph_test( FILE* in_file, int num_of_c );

void my_swap( uint8_t* first_e, uint8_t* second_e );

void my_qsort( void* data, int num_of_e, int size_of_e, int (*compare)( const void*, const void* ) );

int compr ( const void* A, const void* B );

int main()
{
#if 1
    char in_name[]  = "h.txt";
    char out_name[] = "g.txt";

    int ERR = file_worker (in_name, out_name);

    if (ERR != OK)
    {
        printf ("ERROR!!! INDEX: %i\n", ERR);
        return -1;
    }

    printf ("OK\n");

    #ifdef VASYAN_DBG

        unit_tests (out_name, in_name);

    #endif

#endif

    return 0;
}

int file_worker( char* in_name, char* out_name )
{
    // it's acceptable if in_name == out_name //
    assert (in_name  != NULL);
    assert (out_name != NULL);

    PARSED_FILE_t in_file = {NULL, NULL, NULL, MEM_ERR, MEM_ERR};

    int ERR = get_parsed_text (&in_file, in_name);
    if (ERR != OK)
        return ERR;

    FILE* out_file = NULL;
    if ((out_file = fopen (out_name, "wb")) == NULL)
        return FILE_ERR;


    // SORTING AND PRINTING //

    sort_and_print (out_file, &in_file, reversed_compare);

    sort_and_print (out_file, &in_file, reversed_compare);

    fprintf (out_file, "%s", in_file.data);

    // SORTING AND PRINTING //


    pf_close (&in_file);

    fclose (out_file);
    out_file = NULL;

    return OK;
}

void pf_close( PARSED_FILE_t* file )
{
    assert (file != NULL);

    free (file->data);
    free (file->pointers);
    file->data = NULL;
    file->pointers = NULL;
    file->num_of_c = MEM_ERR;
    file->num_of_s = MEM_ERR;
}

int get_parsed_text( PARSED_FILE_t* file, char* in_name )
{
    assert (file != NULL);
    assert (in_name != NULL);

    if ((file->file = fopen (in_name, "rb")) == NULL)
        return FILE_ERR;

    file->num_of_c = file_reader (file->file, &(file->data));

    // don't need this file now //
    fclose (file->file);
    file->file = NULL;

    if (file->num_of_c == MEM_ERR)
        return MEM_ERR;

    file->num_of_s = parser (file->data, &(file->pointers), file->num_of_c);

    if (file->num_of_s == MEM_ERR)
        return MEM_ERR;

    return OK;
}

int file_reader( FILE* in_file, char** data )
{
    assert (in_file != NULL);

    int num_of_chars = 0;

    num_of_chars = get_lenght(in_file);

    *data = (char* )calloc (num_of_chars + 1, sizeof (char));// allocating memory for data + '\0' //

    if (*data == NULL)
        return MEM_ERR;

    fread (*data, sizeof (char), num_of_chars, in_file);// reading data //

    (*data)[num_of_chars] = '\0';// end of the string //

    return num_of_chars;
}

int get_lenght( FILE* in_file )
{
    assert (in_file != NULL);

    fseek (in_file, 0, SEEK_END);
    int num_of_chars = ftell (in_file);// getting num of chars in file //
    rewind (in_file);

    return num_of_chars;
}

int parser( const char* data, char* **pointers, int num_of_c )
{
    assert (data != NULL);
    assert (num_of_c >= 0);

    char curr_c = 0;
    int num_of_s = 0;
    bool isstr = false;

    for (int curr_c_id = 0; curr_c_id < num_of_c; curr_c_id++)// counting num of words //
    {
        if (isend ((curr_c = data[curr_c_id])))
        {
            isstr = false;
        }
        else if (!isstr)
        {
            isstr = true;
            num_of_s++;
        }
    }

    *pointers = (char**) calloc (num_of_s, sizeof (char*));// allocating memory for pointers //

    if (*pointers == NULL)
        return MEM_ERR;

    isstr = false;
    int curr_c_id = 0;

    for (int curr_s_id = 0; curr_s_id < num_of_s; curr_s_id++)// setup of the pointers //
    {
        while (isend ((curr_c = data[curr_c_id])))
        {
            curr_c_id++;
            if (curr_c_id >= num_of_c)
                return num_of_s;
        }

        (*pointers)[curr_s_id] = (char* )&(data[curr_c_id]);

        while (!isend ((curr_c = data[curr_c_id])))
        {
            curr_c_id++;
            if (curr_c_id >= num_of_c)
                return num_of_s;
        }
    }

    return num_of_s;
}

int compare( void const* str_2_compare_A, void const* str_2_compare_B )
{
    assert (str_2_compare_A != NULL);
    assert (str_2_compare_B != NULL);
    //assert (str_2_compare_A != str_2_compare_B);

    const char* A = (*(char**)str_2_compare_A);
    const char* B = (*(char**)str_2_compare_B);

    while (true)
    {
        while (!isalpha (*A))
        {
            A++;
            if (isend (*A))
                break;
        }

        while (!isalpha (*B))
        {
            B++;
            if (isend (*B))
                break;
        }

        if (isend (*A) && isend (*B))
            return 0;

        if (isend (*A) || no_reg_comp(*B, *A))
            return -1;

        if (isend (*B) || no_reg_comp(*A, *B))
            return  1;
        A++;
        B++;
    }
}

int reversed_compare( const void* str_2_compare_A, const void* str_2_compare_B )
{
    assert (str_2_compare_A != NULL);
    assert (str_2_compare_B != NULL);
    //assert (str_2_compare_A != str_2_compare_B);

    const char* A = (*(char**)str_2_compare_A);
    const char* B = (*(char**)str_2_compare_B);

    assert (A != NULL);
    assert (B != NULL);

    //to remember starting positions //
    const char* a = A;
    const char* b = B;

    printf ("%p\n", A);

    while (!isend (*A))// to find last symbol of first word //
        A++;

    while (!isend (*B))// to find last symbol of second word //
        B++;

    A--, B--;

    while (A >= a && B >= b)// reversed compare //
    {

        while (!isalpha (*A))
        {
            if (A <= a)
                break;
            A--;

        }

        while (!isalpha (*B))
        {
            if (B <= b)
                break;
            B--;
        }

        if (no_reg_comp(*B, *A))
            return -1;

        if (no_reg_comp(*A, *B))
            return  1;
        A--;
        B--;
    }

    if (A == a && B == b)
        return 0;

    if (B >= b)// first word is the beginning of the second word //
        return -1;
    return 1;// opposite case //
}

bool isend( char ch )
{
    return ch == '\n' || ch == '\r' || ch == '\0';
}

bool no_reg_comp( char f_letter , char s_letter )
{
    if (!isalpha(f_letter) || !isalpha(s_letter))// only for alphabet symbols
        return false;

    if (f_letter < 'a')
        f_letter += 'a' - 'A';

    if (s_letter < 'a')
        s_letter += 'a' - 'A';

    return f_letter > s_letter;
}

void sort_and_print( FILE* out_file, const PARSED_FILE_t* in_file, int (*compare)( const void*, const void* ) )
{
    my_qsort (in_file->pointers, in_file->num_of_s, sizeof(char* ), compare);

    print_in_file (out_file, in_file);
    fprintf (out_file, "\n\n\n\n\n\n\n\n");
}

void print_in_file( FILE* out_file, const PARSED_FILE_t* in_file )
{
    assert (out_file != NULL);
    assert (in_file->pointers != NULL);
    assert (in_file->num_of_s >= 0);

    for (int i = 0; i < in_file->num_of_s; i++)
    {
        my_print_str (out_file, in_file->pointers[i]);
        fprintf (out_file, "\n");
    }
}

void my_print_str( FILE* out_file, const char str[] )
{
    assert (str != NULL);

    int j = 0;

    while (!isend (str[j]))
        fprintf (out_file, "%c", str[j]), j++;
}

void unit_tests(char* out_name, char* in_name)
{
    assert (out_name != NULL);
    assert (in_name  != NULL);
    assert (in_name  != out_name);

    FILE* out_file = NULL;
    if ((out_file = fopen (out_name, "rb")) == NULL)
    {
        printf ("FILE ERROR!!!\n");
        return;
    }

    FILE* in_file = NULL;
    if ((in_file = fopen (in_name, "rb")) == NULL)
    {
        printf ("FILE ERROR!!!\n");
        return;
    }

    int num_of_c_in  = 0;
    int num_of_c_out = 0;
    char curr_c = 0;

    while ((curr_c = fgetc (in_file)) != EOF)
        if (!isend(curr_c))
            num_of_c_in++;

    while ((curr_c = fgetc (out_file)) != EOF)
        if (!isend(curr_c))
            num_of_c_out++;

    printf ("\n\nUNIT TESTS!!!\n");
    printf ("there are %i symbols in input file!\n"  , num_of_c_in);
    printf ("there are %i symbols in output file!\n" , num_of_c_out);

    printf ("diff (counting x3): %i\n", num_of_c_in * 3 - num_of_c_out);

    printf ("checking alph refer...");

    if (alph_test(out_file, num_of_c_in))
        printf ("OK!!!\n");
    else
        printf ("NOT OK!!!\n");

    fclose (out_file);
    fclose (in_file);
    out_file = NULL;
    in_file = NULL;
}

bool alph_test( FILE* in_file, int num_of_c )
{
    assert (in_file != NULL);
    assert (num_of_c >= 0);

    return true;
}

void my_qsort( void* data, int num_of_e, int size_of_e, int (*compare)( const void*, const void* ) )
{
    if (num_of_e < 2)
        return;

    void* left   = data;
    void* right  = ((char*)data + (num_of_e - 1) * size_of_e);

    uint8_t* center = NULL;

    center = (uint8_t* )calloc (1, size_of_e);

    assert (center != NULL);

    memcpy((void*)center, data + ((num_of_e - 1) / 2) * size_of_e, size_of_e);

    while (left <= right)
    {
        while (compare (left,  (void* )center) < 0)
            left  += size_of_e;

        while (compare (right, (void* )center) > 0)
            right -= size_of_e;

        if (left <= right)
        {
            for (int i = 0; i < size_of_e; i++)
                my_swap ((uint8_t*)(left + i), (uint8_t*)(right + i));
            left  += size_of_e;
            right -= size_of_e;
        }
    }

    free(center);

    if (num_of_e > 2)
    {
        my_qsort (data,      ((long)right - (long)data) / size_of_e + 1, size_of_e, compare);
        my_qsort (left, num_of_e - ((long)left - (long)data) / size_of_e, size_of_e, compare);
    }
}

int compr ( const void* A, const void* B )
{
    return -*((int*)A) + *((int*)B);
}

void my_swap( uint8_t* first_e, uint8_t* second_e )
{
    char tmp = *first_e;
    *first_e = *second_e;
    *second_e = tmp;
}

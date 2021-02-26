#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

#define NDBG

/* Debug memory allocation support */
#ifndef NDEBUG
    # define _CRTDBG_MAP_ALLOC
    # include <crtdbg.h>

    # define SetDbgMemHooks() \
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | \
    _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))

    static class __Dummy
    {
    public:
        /* Class constructor */
        __Dummy(VOID)
        {
            SetDbgMemHooks();
        } /* End of '__Dummy' constructor */
    } __ooppss;

#endif /* _DEBUG */

#ifndef NDEBUG
    #ifdef _CRTDBG_MAP_ALLOC
        #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #endif /* _CRTDBG_MAP_ALLOC */
#endif /* _DEBUG */

#ifndef NDEBUG
    #define VERIFICATION verification();
#elif
    #define VERIFICATION ;
#endif


/* to get poison val for needed data type */
template <typename data_t>
data_t get_poison_val();

template <>
double get_poison_val()
{
    return NAN;
}

/* This list works in dynamical memory massive of units with data and PHYSICAL ids. 
 * Ids allows to work realy fast, but addresation is quite complicated,
 * user should use PHYSICAL adrresses of units in list instead of LOGICAL adrresses.
 * Methods returns PHYSICAL adrresses of units, which was allocated and attached to data list, or freed
 *
 * Also, user can use SLOW addresation with LOGICAL ids
 *
 * User can sort units and if there will be only allocating and attaching on tail or free on tail or head operations,
 * LOGICAL adrresation will be fast.
 */

template <typename data_t>
class list_t 
{
private: /* INTERNAL PRIVATE TYPES */

    /* output type:
     * positive numbers - for ids;
     * negative numbers - for error codes 
     */
    typedef int output_t;

    /* bad id value */
    static const output_t BAD_ID_VALUE_ = -1;

    /* poison val */
    const data_t   POISON_ = get_poison_val<data_t>();

    /* structure with user data and control data */
    struct data_unit_t 
    {
        /* data */
        data_t      data      = NAN;

        /* PHYSICAL ids of prev and next data units */
        output_t    prev      = BAD_ID_VALUE_;
        output_t    next      = BAD_ID_VALUE_;

        /* is this data unit free? */
        bool        free      = true;
    };

    /* for outputs about unit type in list verificator */
    enum UNIT_TYPE{
        /* NORMAL TYPES */
        LIST_FREE_UNIT                  ,
        LIST_TAIL_UNIT                  ,
        LIST_HEAD_UNIT                  ,
        LIST_BODY_UNIT                  ,

        /* BAD TYPES */
            /* head or tail PHYSICAL id dont mutch with PHYSICAL id of this unit, but this unit structure is head or tail */
            LIST_WRONG_DATA_HEAD_UNIT       ,
            LIST_WRONG_DATA_TAIL_UNIT       ,

            /* this unit should have head or tail structure, but it don't */
            LIST_WRONG_DATA_BODY_UNIT       ,
            LIST_WRONG_FREE_BODY_UNIT       ,

            /* wrong PHYSICAL ids of next or prev units */
            LIST_WRONG_PREV_ID_UNIT         ,
            LIST_WRONG_NEXT_ID_UNIT         ,

            /* undefined unit */
            LIST_UNDF_UNIT                  ,
    };

public: /* PRIVATE FIELDS */

    /* number of allocated units */
    size_t      size_          =  0;

    /* number of units */
    size_t      capacity_      =  0;

    /* PHYSICAL ids of tail and head units */
    output_t    data_tail_     = BAD_ID_VALUE_;
    output_t    data_head_     = BAD_ID_VALUE_;

    /* free units head id */
    int    free_head_     = BAD_ID_VALUE_;

    /* ptr to massive with data units */
    data_unit_t* data_units_  = nullptr;

    /* are units sorted ? for fast access mode */
    bool sorted_ = true;

    /* name for dump file */
    const char* const dump_file_name_ = nullptr;

private: /* PRIVATE METHODS */

   /* verificator for data unit
    *
    * returns UNIT_TYPE
    */
    UNIT_TYPE verify_data_unit( const size_t unit_id );

    /* compare func for qsort
     */
    static int data_units_compare( const void* first_unit, const void* secnd_unit );

    /* verify define - fast check and dump in case of error, do nothing in release
     *
     */
    #ifndef NDBG
        #define verify()                 \
        {                                \
            ERR_t error = verificator(); \
            if (error)                   \
            {                            \
                dump();                  \
                return error;            \
            }                            \
        }
    #else
        #define verify() ;
    #endif

public:  /* PUBLIC  METHODS */

    /* constractor and destructor */
    list_t( const size_t initial_capacity, const char* const dump_file_name );
   ~list_t();

    /* " = " operator and copy constructor are deleted */
    list_t( const list_t &list )                 = delete;
    list_t & operator = ( const list_t& tocopy ) = delete;

    /* method to change capacity_
     *
     * returns new capacity_ or error code
     */
    output_t resize( const size_t new_capacity );
    
    /* method to allocate new data unit from free list, write data to this unit
     * and attach this unit next to unit with sended PHYSICAL id
     * 
     * unit_id     - PHYSICAL id of unit, after which method should attach a new unit
     * new_data    - data to write to allocated unit
     *
     * returns PHYSICAL id of unit where the data was written or error code
     */
    output_t attach_next_by_id( const size_t unit_id, const data_t new_data );
    
    /* method to allocate new data unit from free list, write data to this unit
     * and attach this unit prev to unit with sended PHYSICAL id
     * 
     * unit_id     - PHYSICAL id of unit, which should be after allocated unit
     * new_data    - data to write to allocated unit
     *
     * returns PHYSICAL id of unit where the data was written or error code
     */
    output_t attach_prev_by_id( const size_t unit_id, const data_t new_data );
    
    /* method to allocate new data unit from free list, write data to this unit
     * and attach this unit to the tail of data_units_
     * 
     * new data    - data to add
     *
     * returns PHYSICAL id of unit where the data was written or error code
     */
    output_t attach_to_data_tail( const data_t new_data );
    
    /* method to allocate new data unit from free list, write data to this unit
     * and attach this unit to the head of data_units_
     * 
     * new data    - data to add
     *
     * returns PHYSICAL id of unit where the data was written or error code
     */
    output_t attach_to_data_head( const data_t new_data );

    /* method to free data unit with sended PHYSICAL id
     * 
     * unit_id - PHYSICAL id of unit to free
     *
     * returns PHYSICAL id of freed unit or error code
     */
    output_t free_unit_by_id( const size_t unit_id );
    
    /* method to free data unit on tail
     *
     * returns PHYSICAL id of freed unit or error code
     */
    output_t free_unit_on_tail();

    /* method to free data unit on head
     *
     * returns PHYSICAL id of freed unit or error code
     */
    output_t free_unit_on_head();

    /* WORKS SLOW 
     * searches unit with needed LOGICAL id returns it's PHYSICAL id or error code
     *
     * log_id - LOGICAL id
     *
     * returns PHYSICAL id or error code
     */
    output_t slow_get_physical_unit_id_by_logical_id( const size_t log_id_2_find );

    /* to sort data units for fast access by LOGICAL id
     *
     * don't use additional memory 0(N logN)
     *
     * returns error code
     */
    output_t qsort_data_units();

    /* to sort data units for fast access by LOGICAL id
     *
     * use additional memory 0(N)
     *
     * returns or error code
     */
    output_t sort_data_units();

    /* method to get data form unit with given PHYSICAL id
     *
     * returns data or POISON val in case of bad PHYSICAL id
     */
    data_t get_data_by_id( size_t unit_id );

    /* verificator for list
     *
     * returns error code
     */
    ERR_t verificator();

    /* dump function for list
     */
    void dump();
};

#include "list.cpp"

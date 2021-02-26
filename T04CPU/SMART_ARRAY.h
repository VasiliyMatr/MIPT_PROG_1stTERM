#pragma once

#include <stdlib.h>

#include "utils.h"
#include "safe_stash.h"

#undef ERROR

#ifndef NDBG

	// all needed asserts to check stacks
	#define sarray_assert( sarray )												\
	{																			\
		assert (!isBadReadPtr (sarray));										\
		assert (!isBadReadPtr (sarray->storages));								\
	}

	// verificator
	#define sarray_verifier( sarray, to_return )		\
	{													\
		if (ERR_t ERROR = sarray_checker (sarray))		\
			return to_return;							\
	}

	// for minor errors print
	#define MINOR_ERROR_PRINT( condition, to_print, to_return )	\
	if (condition)												\
	{															\
		printf (to_print);										\
		return to_return;										\
	}

// for release version
#else

	#define MINOR_ERROR_PRINT( condition, to_print, to_return ) ;
	#define sarray_verifier( sarray, to_return ) ;
	#define sarray_verifier( sarray, to_return ) ;

#endif

struct DATA_STORAGE_t_ {
	#ifndef NDBG
		canary_t TOP_CANARY = CANARY_INIT;
	#endif

	void* data;

	#if defined FULL_DBG_ENABLE
		long hash;
		long data_hash;
	#endif

	#ifndef NDBG
		canary_t BOT_CANARY = CANARY_INIT;
	#endif
};

struct SMART_ARRAY_t{

	#ifndef NDBG
		canary_t TOP_CANARY		= CANARY_INIT;

		DBG_MODE_t DBG_MODE = NO_DBG;

		ERR_t ERROR_CODE = OK;

		long dbg_mode_hash = 0;
	#endif 

	#if defined FULL_DBG_ENABLE
		long hash;
	#endif


	DATA_STORAGE_t_* storages = NULL;

	size_t numof_storages	= 0;
	size_t sizeof_storage	= 0;
	size_t sizeof_element	= 0;
	
	#ifndef NDBG
		canary_t BOT_CANARY		= CANARY_INIT;
	#endif
};

SMART_ARRAY_t* smart_array_construct( const size_t sizeof_elem, const size_t sizeof_stor, const size_t initial_numberof_storages,  const DBG_MODE_t DBG_MODE );

ERR_t smart_array_destruct( SMART_ARRAY_t* sarray );

ERR_t add_storages_to_sarray( SMART_ARRAY_t* sarray, size_t num_of_storages_2_add );

ERR_t del_storages_from_sarray( SMART_ARRAY_t* sarray, size_t num_of_storages_2_del );

void* do_memory_stuff_with_canaries( void* realloc_ptr, const size_t sizeof_elem, const size_t numof_elem, const size_t sizeof_canary, const void* canary_init_value );

ERR_t sarray_get_data( SMART_ARRAY_t* sarray, size_t id, void* dist );

ERR_t sarray_put_data( SMART_ARRAY_t* sarray, size_t id, void* data );

ERR_t sarray_checker( SMART_ARRAY_t* sarray );

ERR_t sarray_interr_checker( SMART_ARRAY_t* sarray );

void sarray_dump( SMART_ARRAY_t* sarray, FILE* where_2_write );

bool check_sarray_hash( SMART_ARRAY_t* sarray );

ERR_t check_data_storage_hash_( DATA_STORAGE_t_* storage, size_t sizeof_storage );

void ahash_f( SMART_ARRAY_t* sarray );

void hash_sarray_storages( SMART_ARRAY_t* sarray, size_t first_storage_id, size_t second_storage_id );
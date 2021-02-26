#pragma once

#include <stdio.h>

#include "utils.h"

// all needed asserts to check stacks
#define sarray_assert(sarray)																										\
{																																	\
	assert (sarray							!= NULL);																				\
	assert (sarray->storages				!= NULL);																				\
}

enum DBG_MODE_t {
	// only canaries
	CANARY_DBG		,
	// only hash
	HASH_DBG		,
	// hash and canaries
	FULL_DBG		,
	// no debug
	NO_DBG
};

struct DATA_STORAGE_t_ {
	void* data;

	#ifndef NDBG
		long hash;
		long data_hash;
	#endif
};

struct SMART_ARRAY_t{

	#ifndef NDBG
		canary_t TOP_CANARY		= CANARY_INIT;
		
		long hash;

		ERR_t ERROR_CODE = OK;
	#endif 

	DATA_STORAGE_t_* storages = NULL;

	size_t numof_storages	= 0;
	size_t sizeof_storage	= 0;
	size_t sizeof_element	= 0;
	
	#ifndef NDBG
		size_t DBG_MODE = 0;
	
		canary_t BOT_CANARY		= CANARY_INIT;
	#endif
};

SMART_ARRAY_t* smart_array_construct( const size_t sizeof_elem, const size_t sizeof_stor, const size_t initial_numberof_storages,  const DBG_MODE_t DBG_MODE );

ERR_t smart_array_destruct( SMART_ARRAY_t* sarray );

ERR_t add_storages_to_sarray( SMART_ARRAY_t* sarray, size_t num_of_storages_2_add );

ERR_t del_storages_from_sarray( SMART_ARRAY_t* sarray, size_t num_of_storages_2_del );

void* do_memory_stuff_with_canaries( void* realloc_ptr, const size_t sizeof_elem, const size_t numof_elem, const size_t sizeof_canary, const void* canary_init_value );

void sarray_get_data( SMART_ARRAY_t* sarray, size_t storage_id, size_t data_id, void* dist );

void sarray_put_data( SMART_ARRAY_t* sarray, size_t storage_id, size_t data_id, void* data );

ERR_t sarray_checker( SMART_ARRAY_t* sarray );

ERR_t sarray_interr_checker( SMART_ARRAY_t* sarray );
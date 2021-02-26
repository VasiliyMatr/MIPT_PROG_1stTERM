#pragma once

#include "DEVE_STACK.h"

#include "SMART_ARRAY.h"

long hash_f( const void* to_hash, const size_t numof_bytes );

void shash_f( STACK_t* const stack );

void dhash_f( DATA_STORAGE_t* const storage, const size_t size );

bool check_stack_hash( STACK_t* const stack );

ERR_t check_data_storage_hash( DATA_STORAGE_t* const storage, const size_t sizeof_storage );

bool check_sarray_hash( SMART_ARRAY_t* sarray );

ERR_t check_data_storage_hash_( DATA_STORAGE_t_* storage, size_t sizeof_storage );

void ahash_f( SMART_ARRAY_t* sarray );
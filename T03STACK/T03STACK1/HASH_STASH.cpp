#include "HASH_STASH.h"
#include "DEVE_STACK.h"
#include "SMART_ARRAY.h"

bool check_stack_hash( STACK_t* const stack )
{
	stack_assert (stack);

	long stack_hash  = stack->hash;

	stack->hash = HASH_INIT;

	if (stack_hash != hash_f (stack, sizeof (STACK_t)))
		return false;
	
	stack->hash = stack_hash;

	return true;
}

ERR_t check_data_storage_hash( DATA_STORAGE_t* const storage, const size_t sizeof_storage )
{
	assert (storage != NULL);
	// assert (storage->data != NULL);
	assert (sizeof_storage > 0);

	long storage_hash	= storage->hash;
	long data_hash		= storage->data_hash;

	storage->hash = HASH_INIT;

	if (storage_hash != hash_f(storage, sizeof (DATA_STORAGE_t)))
		return DATA_STRUCT_HASH_ERROR;
	storage->hash = storage_hash;

	if (data_hash != hash_f((void*)((int)storage->data - sizeof(canary_t)), sizeof_storage + sizeof (canary_t) * 2))
		return DATA_HASH_ERROR;
	storage->data_hash = data_hash;

	return OK;
}

bool check_sarray_hash( SMART_ARRAY_t* sarray )
{
	long sarray_hash  = sarray->hash;

	sarray->hash = hash_f (sarray->storages, sarray->numof_storages * sizeof (DATA_STORAGE_t_));

	if (sarray_hash != hash_f (sarray, sizeof (SMART_ARRAY_t)))
		return false;
	
	sarray->hash = sarray_hash;

	return true;
}

ERR_t check_data_storage_hash_( DATA_STORAGE_t_* storage, size_t sizeof_storage )
{
	long storage_hash	= storage->hash;
	long data_hash		= storage->data_hash;

	storage->hash = HASH_INIT;

	if (storage_hash != hash_f(storage, sizeof (DATA_STORAGE_t_)))
		return DATA_STRUCT_HASH_ERROR;
	storage->hash = storage_hash;

	if (data_hash != hash_f(storage->data, sizeof_storage))
		return DATA_HASH_ERROR;
	storage->data_hash = data_hash;

	return OK;
}

void ahash_f( SMART_ARRAY_t* sarray )
{
	for (int i = 0; i < sarray->numof_storages; i++)
	{
		sarray->storages[i].hash = HASH_INIT;
		sarray->storages[i].data_hash = hash_f (sarray->storages[i].data, sarray->sizeof_storage * sarray->sizeof_element);
		sarray->storages[i].hash = hash_f (sarray->storages+ i * sizeof (DATA_STORAGE_t_), sizeof (DATA_STORAGE_t_));
	}

	sarray->hash = hash_f (sarray->storages, sarray->numof_storages * sizeof (DATA_STORAGE_t_));
	sarray->hash = hash_f (sarray, sizeof (SMART_ARRAY_t));
}

void shash_f( STACK_t* const stack )
{
	stack_assert (stack);

	stack->hash = HASH_INIT;

	dhash_f (stack->curr_data_storage, stack->sizeof_element * stack->sizeof_storage);

	stack->hash	= hash_f (stack, sizeof (STACK_t));
}

void dhash_f( DATA_STORAGE_t* const storage, const size_t size )
{
	assert (storage != NULL);
	assert (size > 0);

	storage->data_hash = hash_f((void*)((int)storage->data - sizeof(canary_t)), size + sizeof (canary_t) * 2);
	storage->hash = HASH_INIT;

	storage->hash = hash_f (storage, sizeof (DATA_STORAGE_t));
}

long hash_f( const void* to_hash, const size_t numof_bytes )
{
	assert (to_hash != NULL);
	assert (numof_bytes > 0);

	long hash = 1;
	const size_t byte_move = sizeof (long) - 1;


	for (int i = 0; i < numof_bytes; i++)
		hash = -hash * *((char* )to_hash + i) + hash>>byte_move ^ hash<<1;

	return hash;
}
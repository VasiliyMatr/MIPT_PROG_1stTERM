#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "SMART_ARRAY.h"

#undef SMART_ARRAY_t
#undef DATA_STORAGE_t_

#include "HASH_STASH.h"

#define ZERO_SIZE_PRINT(value, to_print)	\
if (value == 0)								\
{											\
	printf (to_print);						\
	return NULL;							\
}

SMART_ARRAY_t* smart_array_construct( const size_t sizeof_elem, const size_t sizeof_stor, const size_t initial_numberof_storages,  const DBG_MODE_t DBG_MODE )
{
	ZERO_SIZE_PRINT (sizeof_elem					,	"\nwrong element size!\n"				);
	ZERO_SIZE_PRINT (sizeof_stor					,	"\nwrong storage size!\n"				);
	ZERO_SIZE_PRINT (initial_numberof_storages		,	"\nwrong initial number of storages!\n"	);

	SMART_ARRAY_t* sarray = (SMART_ARRAY_t* )calloc (1, sizeof (SMART_ARRAY_t));
	if ((sarray) == NULL)
		return NULL;

	sarray->DBG_MODE = DBG_MODE;

	sarray->sizeof_storage = sizeof_stor;
	sarray->sizeof_element = sizeof_elem;

	sarray->BOT_CANARY = CANARY_INIT;
	sarray->TOP_CANARY = CANARY_INIT;
	

	// adding storages for data
	add_storages_to_sarray (sarray, initial_numberof_storages);

	

	return sarray;
}

ERR_t smart_array_destruct( SMART_ARRAY_t* sarray )
{
	del_storages_from_sarray (sarray, sarray->numof_storages);

	free (sarray);

	return OK;
}

ERR_t add_storages_to_sarray( SMART_ARRAY_t* sarray, size_t num_of_storages_2_add )
{
	assert (sarray != NULL);

	DATA_STORAGE_t_* tmp_storages = NULL;

	sarray->numof_storages += num_of_storages_2_add;


	// allocating or reallocating memory for storages (old and new)
	if ((sarray->DBG_MODE == CANARY_DBG) || (sarray->DBG_MODE == FULL_DBG))
		tmp_storages = (DATA_STORAGE_t_* )do_memory_stuff_with_canaries (sarray->storages, sizeof (DATA_STORAGE_t_), sarray->numof_storages, sizeof (canary_t), (void*)&CANARY_INIT);
	else
		tmp_storages = (DATA_STORAGE_t_* )realloc (sarray->storages, sarray->numof_storages * sizeof (DATA_STORAGE_t_));

	// allocation error
	if (tmp_storages == NULL)
		return MEM_ERR;
	
	// no allocation errors
	sarray->storages = tmp_storages;

	// allocating memory for data in new storages
	for (int i = sarray->numof_storages - num_of_storages_2_add; i < sarray->numof_storages; i++)
	{
		if ((sarray->DBG_MODE == CANARY_DBG) || (sarray->DBG_MODE == FULL_DBG))
			sarray->storages[i].data = (void* )do_memory_stuff_with_canaries (NULL, sarray->sizeof_element, sarray->sizeof_storage, sizeof (canary_t), (void*)&CANARY_INIT);
		else
			sarray->storages[i].data = (void* )calloc (sarray->sizeof_storage, sarray->sizeof_element);
		
		// allocation error
		if (sarray->storages[i].data == NULL)
		{ 
			
			del_storages_from_sarray (sarray, i - (sarray->numof_storages - num_of_storages_2_add));
			return MEM_ERR;
		}
	}

	return OK;
}

ERR_t del_storages_from_sarray (SMART_ARRAY_t* sarray, size_t num_of_storages_2_del)
{
	sarray_assert(sarray);

	for (int i = sarray->numof_storages - num_of_storages_2_del; i < sarray->numof_storages; i++)
	{
		if (sarray->DBG_MODE == CANARY_DBG || sarray->DBG_MODE == FULL_DBG)
			do_memory_stuff_with_canaries (sarray->storages[i].data, 0, 0, sizeof (canary_t), (void*)&CANARY_INIT);
		else
			free (sarray->storages[i].data);
	}

	sarray->numof_storages -= num_of_storages_2_del;

	if (sarray->DBG_MODE == CANARY_DBG || sarray->DBG_MODE == FULL_DBG)
		do_memory_stuff_with_canaries (sarray->storages, sizeof (DATA_STORAGE_t_), sarray->numof_storages, sizeof (canary_t), (void* )&CANARY_INIT);
	else
		realloc (sarray->storages, sarray->numof_storages * sizeof (DATA_STORAGE_t_));

	return OK;
}

void* do_memory_stuff_with_canaries( void* realloc_ptr, const size_t sizeof_elem, const size_t numof_elem, const size_t sizeof_canary, const void* canary_init_value )
{
	assert (canary_init_value != NULL);

	// moving back to the right ptr
	if (realloc_ptr != NULL)
		realloc_ptr = (void* )((long)realloc_ptr - sizeof_canary);

	// reallocating of free memory
	void* data = realloc (realloc_ptr, numof_elem * sizeof_elem + sizeof_canary * 2);
	if (data == NULL)
		return NULL;

	// init of canaries
	memcpy (data, canary_init_value, sizeof_canary);
	memcpy ((void* )((long)data + sizeof_canary + numof_elem * sizeof_elem), canary_init_value, sizeof_canary);
	
	// returning ptr to data
	return (void* )((long)data + sizeof_canary);
}

void sarray_get_data( SMART_ARRAY_t* sarray, size_t storage_id, size_t data_id, void* dist )
{
	if (storage_id > sarray->numof_storages)
	{ 
		printf ("\nstorage id is bigger than number of storages in smart array!\n");
		return;
	}

	if (data_id > sarray->sizeof_storage)
	{ 
		printf ("\ndata id is bigger than number of elements in storages of smart array!\n");
		return;
	}

	memcpy (dist, (void* )((long)sarray->storages[storage_id].data + sarray->sizeof_element * data_id), sarray->sizeof_element);
}

void sarray_put_data( SMART_ARRAY_t* sarray, size_t storage_id, size_t data_id, void* data )
{
	if (storage_id > sarray->numof_storages)
	{ 
		printf ("\nstorage id is bigger than number of storages in smart array!\n");
		return;
	}

	if (data_id > sarray->sizeof_storage)
	{ 
		printf ("\ndata id is bigger than number of elements in storages of smart array!\n");
		return;
	}

	memcpy ((void* )((long)sarray->storages[storage_id].data + sarray->sizeof_element * data_id), data, sarray->sizeof_element);
}

ERR_t sarray_checker( SMART_ARRAY_t* sarray )
{
	if (sarray							== NULL) return NULL_PTR_2_SARRAY;
	if (sarray->storages				== NULL) return NULL_PTR_2_STORAGE;
	if (sarray->ERROR_CODE				!= OK  ) return BAD_SARRAY;

	sarray->ERROR_CODE = sarray_interr_checker (sarray);

	return sarray->ERROR_CODE;
}

ERR_t sarray_interr_checker( SMART_ARRAY_t* sarray )
{
	sarray_assert (sarray);
	if (sarray->DBG_MODE == NO_DBG)
		return OK;

	if (sarray->DBG_MODE == CANARY_DBG	|| sarray->DBG_MODE == FULL_DBG)
		if (ERR_t ERROR = check_canaries (sarray, sizeof (SMART_ARRAY_t)))
			return ERROR;

	
	if (sarray->DBG_MODE == HASH_DBG	|| sarray->DBG_MODE == FULL_DBG)
		if (!check_sarray_hash (sarray))
			return STACK_HASH_ERROR;


	DATA_STORAGE_t_* data = sarray->storages;

	for (int i = 0; i < sarray->numof_storages; i++)
	{
		if (sarray->DBG_MODE == CANARY_DBG	|| sarray->DBG_MODE == FULL_DBG)
			if (ERR_t ERROR = check_canaries (&(data[i]), sizeof (DATA_STORAGE_t_)))
				return ERROR;
	
		if (sarray->DBG_MODE == HASH_DBG	|| sarray->DBG_MODE == FULL_DBG)
			if (ERR_t ERROR = check_data_storage_hash_ (&(data[i]), sarray->sizeof_element * sarray->sizeof_storage))
				return ERROR;
	}

	return OK;
}

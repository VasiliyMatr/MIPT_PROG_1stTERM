#include "SMART_ARRAY.h"


SMART_ARRAY_t* smart_array_construct( const size_t sizeof_elem, const size_t sizeof_stor, const size_t initial_numberof_storages,  const DBG_MODE_t DBG_MODE )
{
	MINOR_ERROR_PRINT (sizeof_elem == 0				,	"\nwrong element size!\n"			   , NULL);
	MINOR_ERROR_PRINT (sizeof_stor == 0				,	"\nwrong storage size!\n"			   , NULL);
	
	SMART_ARRAY_t* sarray = (SMART_ARRAY_t* )calloc (1, sizeof (SMART_ARRAY_t));
	if ((sarray) == NULL)
		return NULL;

#ifndef NDBG

	// init ERROR CODE
	sarray->ERROR_CODE = EMPT;

	// init DBG mode
	sarray->DBG_MODE = DBG_MODE;

#endif

	sarray->sizeof_storage = sizeof_stor;
	sarray->sizeof_element = sizeof_elem;

	// canaries init1
#ifndef NDBG

	if ((DBG_MODE == CANARY_DBG) || (DBG_MODE == FULL_DBG))
	{
		sarray->BOT_CANARY = CANARY_INIT;
		sarray->TOP_CANARY = CANARY_INIT;
	}

	sarray->dbg_mode_hash = hash_f (&(sarray->DBG_MODE), sizeof (DBG_MODE_t));

#endif

	// adding storages for data
	if (ERR_t ERROR = add_storages_to_sarray (sarray, initial_numberof_storages))
	{
		free (sarray);
		return NULL;
	}
		
	// hashing
#ifdef FULL_DBG_ENABLE

	if (DBG_MODE == FULL_DBG)
	{
		hash_sarray_storages (sarray, 0, sarray->numof_storages - 1);
		ahash_f (sarray);
	}

#endif

	sarray_verifier(sarray, NULL);
	
	return sarray;
}

ERR_t smart_array_destruct( SMART_ARRAY_t* const  sarray )
{
	sarray_verifier(sarray, ERROR);

	del_storages_from_sarray (sarray, sarray->numof_storages);

	free (sarray);

	return OK;
}

ERR_t add_storages_to_sarray( SMART_ARRAY_t* const sarray, const size_t num_of_storages_2_add )
{

#ifndef NDBG
	
	if (sarray->ERROR_CODE != EMPT)
	{
		sarray_verifier (sarray, ERROR);
	}

	else sarray->ERROR_CODE = OK;

#endif

	DATA_STORAGE_t_* tmp_storages = NULL;

	sarray->numof_storages += num_of_storages_2_add;

	// for fast access
	size_t numof_storages = sarray->numof_storages;
	size_t sizeof_storage = sarray->sizeof_storage;
	size_t sizeof_element = sarray->sizeof_element;


#ifndef NDBG

	DBG_MODE_t DBG_MODE = sarray->DBG_MODE;

	const bool need_hash     =  DBG_MODE == FULL_DBG;
	const bool need_canaries = (DBG_MODE == CANARY_DBG) || (DBG_MODE == FULL_DBG);

#endif

	// allocating or reallocating memory for storages (old and new)
#ifndef NDBG

	if (need_canaries)
		tmp_storages = (DATA_STORAGE_t_* )do_memory_stuff_with_canaries (sarray->storages, sizeof (DATA_STORAGE_t_), numof_storages, sizeof (canary_t), (void*)&CANARY_INIT);
	else
		tmp_storages = (DATA_STORAGE_t_* )realloc (sarray->storages, numof_storages * sizeof (DATA_STORAGE_t_));

#else

	tmp_storages = (DATA_STORAGE_t_* )realloc (sarray->storages, sarray->numof_storages * sizeof (DATA_STORAGE_t_));

#endif 

	// allocation error
	if (tmp_storages == NULL)
		return MEM_ERR;
	
	// no allocation errors
	sarray->storages = tmp_storages;
	// for fast access
	DATA_STORAGE_t_* storages = sarray->storages;

	// allocating memory for data in new storages
	for (size_t i = numof_storages - num_of_storages_2_add; i < numof_storages; i++)
	{
#ifndef NDBG

		if (need_canaries)
			storages[i].data = (void* )do_memory_stuff_with_canaries (NULL, sizeof_element, sizeof_storage, sizeof (canary_t), (void*)&CANARY_INIT);
		else
			storages[i].data = (void* )calloc (sizeof_storage, sizeof_element);

#else

		sarray->storages[i].data = (void* )calloc (sizeof_storage, sizeof_element);

#endif
		// allocation error
		if (storages[i].data == NULL)
		{ 
			del_storages_from_sarray (sarray, i - (numof_storages - num_of_storages_2_add));
			return MEM_ERR;
		}

		// setupping canaries and poisoning
#ifndef NDBG

		if (need_canaries)
		{
			storages[i].TOP_CANARY = CANARY_INIT;
			storages[i].BOT_CANARY = CANARY_INIT;
		}

		add_poison (storages[i].data, sizeof_element * sizeof_storage);

#endif

	}

#ifdef FULL_DBG_ENABLE

	if (need_hash)
	{
		hash_sarray_storages(sarray, numof_storages - num_of_storages_2_add, numof_storages - 1);
		ahash_f(sarray);
	}

#endif
	
	sarray_verifier(sarray, ERROR);

	return OK;
}

ERR_t del_storages_from_sarray( SMART_ARRAY_t* const sarray, const size_t num_of_storages_2_del )
{
	sarray_verifier(sarray, ERROR);

#ifndef NDBG

	DBG_MODE_t DBG_MODE = sarray->DBG_MODE;

	const bool need_hash     =  DBG_MODE == FULL_DBG;
	const bool need_canaries = (DBG_MODE == CANARY_DBG) || (DBG_MODE == FULL_DBG);

#endif

	// for fast access
	size_t numof_storages = sarray->numof_storages;
	DATA_STORAGE_t_* storages = sarray->storages;

	for (size_t i = numof_storages - num_of_storages_2_del; i < numof_storages; i++)
	{

#ifndef NDBG

		if (need_canaries)
			do_memory_stuff_with_canaries (storages[i].data, 0, 0, sizeof (canary_t), (void*)&CANARY_INIT);
		else
			free (storages[i].data);

#else

		free (sarray->storages[i].data);

#endif

	}

	sarray->numof_storages -= num_of_storages_2_del;
	numof_storages		   -= num_of_storages_2_del;

#ifndef NDBG

	if (need_canaries)
		do_memory_stuff_with_canaries (storages, sizeof (DATA_STORAGE_t_), numof_storages, sizeof (canary_t), (void* )&CANARY_INIT);// size can only decrease
	else
		realloc (storages, numof_storages * sizeof (DATA_STORAGE_t_));// size can only decrease

#else

	realloc (storages, numof_storages * sizeof (DATA_STORAGE_t_));// size can only decrease

#endif

#ifdef FULL_DBG_ENABLE

	if (need_hash)
		ahash_f (sarray);

#endif

	sarray_verifier(sarray, ERROR);

	return OK;
}

void* do_memory_stuff_with_canaries( void* realloc_ptr, const size_t sizeof_elem, const size_t numof_elem, const size_t sizeof_canary, const void* canary_init_value )
{
	assert (!isBadReadPtr (canary_init_value));
	assert (sizeof_canary > 0				);

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

ERR_t sarray_get_data( SMART_ARRAY_t* const sarray, const size_t id, void* const dist )
{
	sarray_verifier(sarray, ERROR);

	MINOR_ERROR_PRINT (id > sarray->numof_storages * sarray->sizeof_storage, "\nid is bigger than number of elements in smart array!\n", MINOR_ERROR);

	memcpy (dist, (void* )((long)sarray->storages[id / sarray->sizeof_storage].data + sarray->sizeof_element * (id % sarray->sizeof_storage)), sarray->sizeof_element);

	return OK;
}

ERR_t sarray_put_data( SMART_ARRAY_t* const sarray, const size_t id, const void* data )
{
	sarray_verifier(sarray, ERROR);

	MINOR_ERROR_PRINT (id > sarray->numof_storages * sarray->sizeof_storage, "\nid is bigger than number of elements in smart array!\n", MINOR_ERROR);

	memcpy ((void* )((long)sarray->storages[id / sarray->sizeof_storage].data + sarray->sizeof_element * (id % sarray->sizeof_storage)), data, sarray->sizeof_element);

#ifdef FULL_DBG_ENABLE

	DBG_MODE_t DBG_MODE = sarray->DBG_MODE;

	if (DBG_MODE == FULL_DBG)
		hash_sarray_storages (sarray, id / sarray->sizeof_storage, id / sarray->sizeof_storage);

#endif

	return OK;
}

#ifndef NDBG

	ERR_t sarray_checker( SMART_ARRAY_t* const sarray )
	{
		if (isBadReadPtr (sarray)					)									return SARRAY_NULL_PRT_ERROR;
		if (isBadReadPtr (sarray->storages)			)									return SARRAY_DATA_NULL_PTR_ERROR;
		if (sarray->dbg_mode_hash != hash_f (&(sarray->DBG_MODE), sizeof (DBG_MODE_t))) return SARRAY_DBG_MODE_HASH_ERROR;
		if (sarray->ERROR_CODE				!= OK	)									return BAD_SARRAY;
		
		sarray->ERROR_CODE = sarray_interr_checker (sarray);

		return sarray->ERROR_CODE;
	}

	ERR_t sarray_interr_checker( SMART_ARRAY_t* const sarray )
	{		
		sarray_assert (sarray);

		if (sarray->DBG_MODE == NO_DBG)
			return OK;
		
#ifndef NDBG

		DBG_MODE_t DBG_MODE = sarray->DBG_MODE;

		const bool need_hash     =  DBG_MODE == FULL_DBG;
		const bool need_canaries = (DBG_MODE == CANARY_DBG) || (DBG_MODE == FULL_DBG);

#endif

		if (need_canaries)
			if (ERR_t ERROR = check_canaries(sarray, sizeof(SMART_ARRAY_t)))
			{
				if (ERROR == BOT_CANARY_DEAD)
					return SARRAY_BOT_CANARY_DEAD;
				if (ERROR == TOP_CANARY_DEAD)
					return SARRAY_TOP_CANARY_DEAD;
				return SARRAY_NULL_PRT_ERROR;
			}

#ifdef FULL_DBG_ENABLE

		if (need_hash)
			if (!check_sarray_hash (sarray))
				return STACK_HASH_ERROR;

#endif

		DATA_STORAGE_t_* data = sarray->storages;

		for (size_t storage_id = 0; storage_id < sarray->numof_storages; storage_id++)
		{
			if (need_canaries)
			{ 
				if (ERR_t ERROR = check_canaries (&(data[storage_id]), sizeof (DATA_STORAGE_t_)))
				{ 
					if (ERROR == BOT_CANARY_DEAD)
						return SARRAY_DATA_STORAGE_BOT_CANARY_DEAD;
					if (ERROR == TOP_CANARY_DEAD)
						return SARRAY_DATA_STORAGE_TOP_CANARY_DEAD;
					return SARRAY_DATA_STORAGE_NULL_PTR_ERROR;
				}								
				if (ERR_t ERROR = check_canaries((void*)((long)data[storage_id].data - sizeof(canary_t)), sarray->sizeof_element * sarray->sizeof_storage + sizeof(canary_t) * 2))
				{
					if (ERROR == BOT_CANARY_DEAD)
						return SARRAY_DATA_BOT_CANARY_DEAD;
					if (ERROR == TOP_CANARY_DEAD)
						return SARRAY_DATA_TOP_CANARY_DEAD;
					return SARRAY_DATA_NULL_PTR_ERROR;
				}
			}

#ifdef FULL_DBG_ENABLE

			if (sarray->DBG_MODE == FULL_DBG)
				if (ERR_t ERROR = check_data_storage_hash_ (&(data[storage_id]), sarray->sizeof_element * sarray->sizeof_storage))
					return ERROR;

#endif

		}

		return OK;
	}

	void sarray_dump( SMART_ARRAY_t* const sarray, FILE* const where_2_write )
	{
		sarray_assert (sarray);

		fprintf (where_2_write,  "	ADRR            - %p\n", sarray);
		fprintf (where_2_write,  "	STORAGE SIZE    - %i\n", sarray->sizeof_storage);
		fprintf (where_2_write,  "	ELEMENT SIZE    - %i\n", sarray->sizeof_element);
		fprintf (where_2_write,  "	NUM OF STORAGES - %i\n", sarray->numof_storages);

		bool need_2_check_canaries	= sarray->DBG_MODE == FULL_DBG || sarray->DBG_MODE == CANARY_DBG;
		bool need_2_check_hashes	= sarray->DBG_MODE == FULL_DBG;
		bool corrupted_storage      = false;

		size_t numof_storages		  = sarray->numof_storages;
		size_t sizeof_storage		  = sarray->sizeof_storage;
		size_t sizeof_element		  = sarray->sizeof_element;
		DATA_STORAGE_t_* storages	  = sarray->storages;


		for (int storage_id = 0; storage_id < numof_storages; storage_id++)
		{
			DATA_STORAGE_t_* curr_storage = &(storages[storage_id]);
			void*			 data		  = curr_storage->data;

			// storage state output
			fprintf (where_2_write,  "\n   [#%05i] STORAGE: ", storage_id + 1);
			if (need_2_check_canaries)
			{
				if (ERR_t ERROR = check_canaries (curr_storage, sizeof (DATA_STORAGE_t_)))
				{
					fprintf (where_2_write,  "	BAD STORAGE!  <---\n");

					if (ERROR == TOP_CANARY_DEAD)
						fprintf (where_2_write,  "TOP CANARY IS DEAD!\n");
					if (ERROR == BOT_CANARY_DEAD)
						fprintf (where_2_write,  "BOT CANARY IS DEAD!\n");
					if (ERROR == SARRAY_DATA_STORAGE_NULL_PTR_ERROR)
						fprintf (where_2_write,  "NULL PTR TO STORAGE!\n");

					fprintf (where_2_write,  "	STRUCT IS BAD, I WONT PRINT IT!\n");
					continue;
				}

				if (ERR_t ERROR = check_canaries ((void*)((long)data - sizeof (canary_t)), sizeof_element * sizeof_storage + sizeof (canary_t) * 2))
				{
					fprintf (where_2_write,  "	BAD STORAGE!  <---\n");

					if (ERROR == TOP_CANARY_DEAD)
						fprintf (where_2_write,  "TOP CANARY IS DEAD!\n");
					if (ERROR == BOT_CANARY_DEAD)
						fprintf (where_2_write,  "BOT CANARY IS DEAD!\n");
					if (ERROR == SARRAY_DATA_NULL_PTR_ERROR)
					{ 
						fprintf (where_2_write,  "NULL PTR TO DATA!\n");
						continue;
					}
					corrupted_storage = true;
				}
			}

#ifdef FULL_DBG_ENABLE

			if (need_2_check_hashes)
			{
				if (ERR_t ERROR = check_data_storage_hash_ (curr_storage, sizeof_element * sizeof_storage))
				{				
					fprintf (where_2_write,  "	BAD STORAGE!  <---\n");
					fprintf (where_2_write,  "	HASH ERROR !	  \n");
					if (ERROR == SARRAY_DATA_STORAGE_HASH_ERROR)
					{
						fprintf (where_2_write,  "	STRUCT IS BAD, I WONT PRINT IT!\n");
						continue;
					}

					corrupted_storage = true;
				}
			}

#endif

			if (!corrupted_storage)
				fprintf (where_2_write,  "	good storage  \n");
			else
				corrupted_storage = false;

			for (size_t data_id = 0; data_id < sizeof_storage; data_id++)
			{ 
				bool isPoisoned = true;

				fprintf (where_2_write,  "		 [#%06d] |", data_id + 1);
				for (int byte_id = sizeof_element - 1; byte_id >= 0; byte_id--)
				{ 
					unsigned char symbol = *(unsigned char*)((int)data + sizeof_element * data_id + byte_id);

					if (symbol != POISON_VAL)
						isPoisoned = false;
					fprintf (where_2_write,  "%4.2X   |", symbol);
				}

				if (isPoisoned)
				{
					fprintf (where_2_write,  "  <-  POISONED!");
					isPoisoned = true;
				}

				fprintf (where_2_write,  "\n");
			}
		}
	}

	#ifdef FULL_DBG_ENABLE

		bool check_sarray_hash( SMART_ARRAY_t* const sarray )
		{
			long sarray_hash  = sarray->hash;

			sarray->hash = HASH_INIT;

			if (sarray_hash != hash_f (sarray, sizeof (SMART_ARRAY_t)))
				return false;
			sarray->hash = sarray_hash;

			return true;
		}

		ERR_t check_data_storage_hash_( DATA_STORAGE_t_* const storage, const size_t sizeof_storage )
		{
			long storage_hash	= storage->hash;

			storage->hash = HASH_INIT;

			if (storage_hash != hash_f(storage, sizeof (DATA_STORAGE_t_)))
				return SARRAY_DATA_STORAGE_HASH_ERROR;
			storage->hash = storage_hash;

			if (storage->data_hash != hash_f(storage->data, sizeof_storage))
				return SARRAY_DATA_HASH_ERROR;

			return OK;
		}

		void ahash_f( SMART_ARRAY_t* const sarray )
		{
			sarray->hash = HASH_INIT;
			sarray->hash = hash_f (sarray, sizeof (SMART_ARRAY_t));
		}

		void hash_sarray_storages( SMART_ARRAY_t* const sarray, const size_t first_storage_id, const size_t second_storage_id )
		{
			assert (sarray->numof_storages >  first_storage_id);
			assert (second_storage_id      >= first_storage_id);

			for (size_t storage_id = first_storage_id; storage_id <= second_storage_id; storage_id++)
			{
				sarray->storages[storage_id].hash = HASH_INIT;
				sarray->storages[storage_id].data_hash = hash_f (sarray->storages[storage_id].data, sarray->sizeof_storage * sarray->sizeof_element);
				sarray->storages[storage_id].hash = hash_f (&(sarray->storages[storage_id]), sizeof (DATA_STORAGE_t_));
			}
		}

	#endif 

#endif 
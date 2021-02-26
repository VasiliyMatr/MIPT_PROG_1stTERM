#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstring>

#include "utils.h"
#include "USER_STACK.h"
#include "DEVE_STACK.h"
#include "HASH_STASH.h"

STACK_t* stack_construct( const size_t sizeof_storage, const size_t sizeof_element, const char* log_name )
{
	if (sizeof_storage == 0 || sizeof_element == 0)
	{
		printf ("BAD SIZE!\n");
		return NULL;
	}

	STACK_t* stack = (STACK_t* )calloc (1, sizeof (STACK_t));
	if (stack == NULL)
		return NULL;

	stack->sizeof_element = sizeof_element;
	stack->sizeof_storage = sizeof_storage;
	stack->ERROR = OK;
	
	stack->curr_data_storage = data_storage_construct (stack->sizeof_element, stack->sizeof_storage);
	if (stack->curr_data_storage == NULL)
	{
		free (stack);
		return NULL;
	}
	if ((stack->log_file = fopen(log_name, "w")) == NULL)
		printf("\ncan't create log file\n");
	
	stack->BOT_CANARY = CANARY_INIT;
	stack->TOP_CANARY = CANARY_INIT;

	// hashing file
	long addr = (long)stack->log_file;								
	stack->file_hash = hash_f (&addr, sizeof (FILE*));

	// hashing stack
	shash_f (stack);
	VERIFI_OR_DUMP_N_RETURN (stack, NULL);

	return stack;
}

DATA_STORAGE_t* data_storage_construct( const size_t sizeof_elem, const size_t sizeof_stor )
{
	assert (sizeof_stor	> 0);
	assert (sizeof_elem	> 0);

	DATA_STORAGE_t* storage = (DATA_STORAGE_t* )calloc (1, sizeof (DATA_STORAGE_t));
	if ((storage) == NULL)
		return NULL;

	storage->data = (void* )((long)calloc (1, sizeof_elem * sizeof_stor + sizeof (canary_t) * 2) + sizeof (canary_t));
	if (((long)storage->data - sizeof (canary_t)) == NULL)
	{	
		free (storage);
		return NULL;
	}

	storage->BOT_CANARY = CANARY_INIT;
	storage->TOP_CANARY = CANARY_INIT;

	*(canary_t*)((long)storage->data - sizeof (canary_t)) = CANARY_INIT;
	*(canary_t*)((long)storage->data + sizeof_stor  ) = CANARY_INIT;
	
	return storage;
}

ERR_t stack_deconstruct( STACK_t* const stack )
{
	//verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	DATA_STORAGE_t* curr_storage = stack->curr_data_storage;

	do
	{
		stack->curr_data_storage = stack->curr_data_storage->prev;

		data_storage_deconstruct(curr_storage);

	} while ((curr_storage = stack->curr_data_storage) != NULL);

	free (stack);
	
	return OK;
}

ERR_t data_storage_deconstruct( DATA_STORAGE_t* const storage )
{
	assert (storage			!= NULL);
	assert (storage->data	!= NULL);

	free ((void* )((long)storage->data - sizeof(canary_t)));
	free (storage);

	return OK;
}

ERR_t stack_push( STACK_t* stack, void* value )
{
	assert (value != NULL);

	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	if (stack->curr_data_id >= stack->sizeof_storage)
		if (add_data_storage (stack) == MEM_ERR)
			return MEM_ERR;

	memcpy ((void* )((char* )stack->curr_data_storage->data + stack->sizeof_element * stack->curr_data_id), value, stack->sizeof_element);
	stack->curr_data_id++;

	#ifndef NDBG
		shash_f (stack);
	#endif	
	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	return OK;
}

ERR_t stack_pop( STACK_t* stack, void* dist )
{
	assert (dist != NULL);

	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	if (stack->curr_data_id == 0)
	{
		if (remove_data_storage (stack) == EMPT)
			return EMPT;
	}

	else
		stack->curr_data_id--;

	memcpy (dist, (void* )((char* )stack->curr_data_storage->data + stack->sizeof_element * stack->curr_data_id), stack->sizeof_element);
		
	#ifndef NDBG
		shash_f (stack);
	#endif
	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	return OK;
}

ERR_t add_data_storage( STACK_t* stack )
{
	stack_assert (stack);

	DATA_STORAGE_t* new_storage = data_storage_construct (stack->sizeof_element, stack->sizeof_storage);
	if (new_storage == NULL)
		return MEM_ERR;

	new_storage->prev = stack->curr_data_storage;
	stack->curr_data_storage = new_storage;
	stack->curr_data_id = 0;

	return OK;
}

ERR_t remove_data_storage( STACK_t* stack )
{
	stack_assert (stack);

	if (stack->curr_data_storage->prev == NULL)
		return EMPT;

	DATA_STORAGE_t* tmp = stack->curr_data_storage;
	stack->curr_data_storage = stack->curr_data_storage->prev;
	data_storage_deconstruct(tmp);

	stack->curr_data_id = stack->sizeof_storage - 1;

	return OK;
}

ERR_t stack_verifier( STACK_t* stack )
{
	if (stack							== NULL) return NULL_PTR_2_STACK;
	if (stack->curr_data_storage		== NULL) return NULL_PTR_2_STORAGE;
	if (stack->curr_data_storage->data	== NULL) return NULL_PTR_2_DATA;
	if (stack->ERROR					!= OK  ) return BAD_STACK;

	stack->ERROR = interr_check (stack);

	return stack->ERROR;
}

ERR_t interr_check( STACK_t* const stack )
{
	stack_assert (stack);

	if (!check_stack_hash (stack))
		return STACK_HASH_ERROR;

	DATA_STORAGE_t* data = stack->curr_data_storage;

	do {
		
		if (ERR_t ERROR = check_data_storage_hash (data, stack->sizeof_element * stack->sizeof_storage))
			return ERROR;

		data = data->prev;

	} while (data != NULL);

	return OK;
}

ERR_t check_canaries( void* data, size_t sizeof_data )
{
	assert (data != NULL);
	assert (sizeof_data > sizeof (canary_t) * 2);

	if (*(canary_t* )data != CANARY_INIT)
		return TOP_CANARY_ERROR;
	if (*(canary_t* )((int)data + sizeof_data - sizeof (canary_t)) != CANARY_INIT)
		return BOT_CANARY_ERROR;
	return OK;
}

void stack_err_print( STACK_t* stack, ERR_t ERROR )
{
	switch (ERROR)												
	{																																			
		case NULL_PTR_2_STACK:																
			printf ("NULL PTR TO STACK, NO LOG FOR U((( \n");							
			return;														
																					
		CASE_PRINT (NULL_PTR_2_STORAGE	, "STACK (NOT OK) NULL PTR TO STORAGE!  \n");		
		CASE_PRINT (NULL_PTR_2_DATA		, "STACK (NOT OK) NULL PTR TO DATA!     \n");
		case STACK_HASH_ERROR:
			if (stack->log_file != NULL)																							
			{																														
				if (!file_check (stack))																							
				{ 																													
					printf ("\nerror with log file\n\n");																			
					return;																											
				}																													
					fprintf (stack->log_file,  "STACK (NOT OK) HASH ERROR, BAD STACK!\n");	

					ERR_t CAN_ERROR = OK;
					if ((CAN_ERROR = check_canaries (stack, sizeof (STACK_t))))
					{
						if			(CAN_ERROR == BOT_CANARY_ERROR)									
							fprintf (stack->log_file,  "BOT CANARY IS DEAD!\n");		
						else if		(CAN_ERROR == TOP_CANARY_ERROR)
							fprintf (stack->log_file,  "TOP CANARY IS DEAD!\n");		
					}
					fclose (stack->log_file);																						
			}																														
			return;	
																					
		case BAD_STACK:																	
			return;	

		case DATA_STRUCT_HASH_ERROR:													
		case DATA_HASH_ERROR:															
			if (stack->log_file != NULL)												
			{																			
				if (!file_check (stack))
				{ 
					printf ("\nerror with log file\n\n");
					return;
				}											
				fprintf (stack->log_file,  "STACK (NOT OK) HASH ERROR, BAD DATA!\n");	
				stack_out (stack);// fclose is included															
			}																			
			return;
																					
		default:																		
			if (stack->log_file != NULL)												
			{																			
				if (!file_check (stack))
				{ 
					printf ("\nerror with log file\n\n");
					return;
				}
				fprintf (stack->log_file,  "STACK (NOT OK) UNKNOWN ERROR, CODE!\n");	
				fclose (stack->log_file);												
			}																			
			return;															
	}																				
}

bool file_check( const STACK_t* stack )
{
	long addr = (long)stack->log_file;												
	if (stack->file_hash != hash_f (&addr, sizeof (long)))							
		return false;																		
	return true;
}

void stack_out( const STACK_t* stack )
{
	stack_assert (stack);
	assert		 (stack->sizeof_element > 0);
	assert		 (stack->sizeof_storage > 0);

	fprintf (stack->log_file,  "	ADRR         - %p\n", stack);
	fprintf (stack->log_file,  "	STORAGE SIZE - %i\n", stack->sizeof_storage);
	fprintf (stack->log_file,  "	ELEMENT SIZE - %i\n", stack->sizeof_element);
	fprintf (stack->log_file,  "	CURR ELEMENT - %i\n", stack->curr_data_id);

	DATA_STORAGE_t* data		= stack->curr_data_storage;
	int				id			= stack->sizeof_storage		- 1;
	int				maxid		= stack->curr_data_id		- 1;
	int				storage_id  = 1;
	bool			bad_stor_f  = false;
		
	while (id >= 0 && !bad_stor_f)
	{
		// storage state output
		fprintf (stack->log_file,  "\n   [#%05i] STORAGE: ", storage_id);

		if (check_data_storage_hash (data, stack->sizeof_element * stack->sizeof_storage))
		{ 
			fprintf (stack->log_file,  "	BAD STORAGE!  <---\n");
			if (stack->ERROR == DATA_STRUCT_HASH_ERROR)
			{
				ERR_t CAN_ERROR = OK;
				if (!(CAN_ERROR = check_canaries (data, sizeof (DATA_STORAGE_t))))
				{
					if (CAN_ERROR == BOT_CANARY_ERROR)									
						fprintf (stack->log_file,  "BOT CANARY IS DEAD!\n");		
					else if (CAN_ERROR == TOP_CANARY_ERROR)
						fprintf (stack->log_file,  "TOP CANARY IS DEAD!\n");		
				}

				fprintf (stack->log_file,  "	STRUCT IS BAD, I WONT PRINT IT!\n");
				return;
			}
			bad_stor_f = true;

			ERR_t CAN_ERROR = OK;
			if ((CAN_ERROR = check_canaries ((void*)((long)data->data - sizeof (canary_t)), stack->sizeof_element * stack->sizeof_storage + sizeof (canary_t) * 2)))
			{
				if (CAN_ERROR == BOT_CANARY_ERROR)									
					fprintf (stack->log_file,  "BOT CANARY IS DEAD!\n");		
				else if (CAN_ERROR == TOP_CANARY_ERROR)
					fprintf (stack->log_file,  "TOP CANARY IS DEAD!\n");		
			}
		}
		else
			fprintf (stack->log_file,  "	good storage\n");

		fprintf (stack->log_file,  "   ADRR OF STOR - %p\n", data);
		fprintf (stack->log_file,  "   ADRR OF DATA - %p\n", data->data);

		while (id >= 0)
		{
			// data outputs
			if (id > maxid)
				fprintf (stack->log_file,  "		 [#%06d] |", id + 1);// not used space
			else
				fprintf (stack->log_file,  "		*[#%06d] |", id + 1);// used space

			for (int i = stack->sizeof_element - 1; i >= 0; i--)
				fprintf (stack->log_file,  "%3X   |", *(unsigned char*)((int)(data->data) + stack->sizeof_element * id + i));
			fprintf (stack->log_file,  "\n");

			// next data
			id--;
		}
		
		// if there are at least one more storage with data
		if (data->prev != NULL)
		{
			maxid = stack->sizeof_storage - 1;
			storage_id++;
			data = data->prev;
			id = stack->sizeof_storage - 1;
		}
	}

	fclose (stack->log_file);
}
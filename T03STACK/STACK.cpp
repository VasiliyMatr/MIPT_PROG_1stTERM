#include "USER_STACK.h"
#include "DEVE_STACK.h"
#include "utils.h"

#include "safe_stash.h"

STACK_t* stack_construct( const size_t sizeof_storage, const size_t sizeof_element, const char* log_name, const DBG_MODE_t DBG_MODE )
{

#ifndef NDBG

	if (sizeof_storage == 0 || sizeof_element == 0)
	{
		printf ("BAD SIZE!\n");
		return NULL;
	}

#endif

	STACK_t* stack = (STACK_t* )calloc (1, sizeof (STACK_t));
	if (stack == NULL)
		return NULL;

	stack->sizeof_element = sizeof_element;
	stack->sizeof_storage = sizeof_storage;

#ifndef NDBG

	stack->ERROR = OK;
	stack->DBG_MODE = DBG_MODE;

#endif

	stack->data_storage = smart_array_construct (stack->sizeof_element, stack->sizeof_storage, initial_sizeof_stack, DBG_MODE);
	if (stack->data_storage == NULL)
	{
		free (stack);
		return NULL;
	}

#ifndef NDBG

	if ((stack->log_file = fopen(log_name, "w")) == NULL)
		printf("\ncan't create log file\n");

	stack->BOT_CANARY = CANARY_INIT;
	stack->TOP_CANARY = CANARY_INIT;


	// hashing file	and dbg			
	stack->file_hash		= hash_f (&(stack->log_file), sizeof (FILE*));
	stack->dbg_mode_hash	= hash_f (&(stack->DBG_MODE), sizeof (DBG_MODE_t));


#endif

	// hashing stack
#ifdef FULL_DBG_ENABLE
	
	shash_f (stack);

#endif
	
	VERIFI_OR_DUMP_N_RETURN (stack, NULL);

	return stack;
}

ERR_t stack_deconstruct( STACK_t* const stack )
{
	//verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	smart_array_destruct (stack->data_storage);

	free (stack);
	
	return OK;
}

ERR_t stack_push( STACK_t* const stack, const void* value )
{
	MINOR_ERROR_PRINT (isBadReadPtr (value), "\nwrong ptr to pushable element\n", MEM_ERR);

	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	// for fast access
	SMART_ARRAY_t* sarray			= stack->data_storage;
	size_t		   numof_storages	= sarray->numof_storages;

	if (stack->curr_data_id >= numof_storages * stack->sizeof_storage)
		if (ERR_t ERROR = add_storages_to_sarray(sarray, numof_storages))// x2 num of storages
			return ERROR;

	sarray_put_data (sarray, stack->curr_data_id, value);
	stack->curr_data_id++;

#ifdef FULL_DBG_ENABLE

	if (stack->DBG_MODE == FULL_DBG)
		shash_f (stack);
	
#endif	
	
	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	return OK;
}

ERR_t stack_pop( STACK_t* const stack, void* const dist )
{
	MINOR_ERROR_PRINT (isBadReadPtr (dist), "\nwrong ptr to popable element\n", MEM_ERR);

	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	// for fast access
	SMART_ARRAY_t*  sarray			= stack->data_storage;
	size_t			curr_data_id	= stack->curr_data_id - 1;
	size_t			sizeof_element	= stack->sizeof_element;
	size_t			numof_storages	= sarray->numof_storages;

	if (curr_data_id == -1)
		return EMPT;

	else
		stack->curr_data_id--;

	sarray_get_data (sarray, curr_data_id, dist);

#ifndef NDBG
	
	if (stack->DBG_MODE != NO_DBG)
	{
		char* POISON = (char*)calloc (sizeof(char), sizeof_element);
		if (POISON != NULL)
		{
			for (int i = 0; i < sizeof_element; i++)
				POISON[i] = POISON_VAL;
			sarray_put_data(sarray, curr_data_id, POISON);
			free(POISON);
		}
	}

#endif

	if (numof_storages * stack->sizeof_storage > 3 * curr_data_id)
		del_storages_from_sarray (sarray, numof_storages * 2 / 3);
		
#ifdef FULL_DBG_ENABLE	
	
	if (stack->DBG_MODE == FULL_DBG)
		shash_f (stack);
	
#endif

	// verifing
	VERIFI_OR_DUMP_N_RETURN (stack, ERROR);

	return OK;
}

#ifndef NDBG

	ERR_t stack_verifier( STACK_t* const stack )
	{
		if (stack							== NULL)													return STACK_NULL_PRT_ERROR;
		if (stack->data_storage				== NULL)													return SARRAY_NULL_PRT_ERROR;
		if (stack->file_hash				!= hash_f (&(stack->log_file), sizeof (FILE*)))				return STACK_LOG_FILE_HASH_ERROR;
		if (stack->dbg_mode_hash			!= hash_f (&(stack->DBG_MODE), sizeof (DBG_MODE_t)))		return STACK_DBG_MODE_HASH_ERROR;
		if (stack->ERROR					!= OK  )													return BAD_STACK;

		stack->ERROR = interr_check (stack);

		return stack->ERROR;
	}

	ERR_t interr_check( STACK_t* const stack )
	{
		stack_assert (stack);

		ERR_t ERROR = check_canaries (stack, sizeof (STACK_t));
		if (ERROR)
		{
			if (ERROR == TOP_CANARY_DEAD)
				return STACK_TOP_CANARY_DEAD;
			if (ERROR == BOT_CANARY_DEAD)
				return STACK_BOT_CANARY_DEAD;
			return STACK_NULL_PRT_ERROR;
		}

#ifdef FULL_DBG_ENABLE		

		if (stack->DBG_MODE == FULL_DBG)
			if (!check_stack_hash (stack))
				return STACK_HASH_ERROR;

#endif

		if (ERR_t ERROR = sarray_checker (stack->data_storage))
			return ERROR;

		return OK;
	}

	void stack_err_print( STACK_t* const stack, const ERR_t ERROR )
	{
		switch (ERROR)												
		{																																			
			case STACK_NULL_PRT_ERROR:																
				printf ("NULL PTR TO STACK, NO LOG FOR U((( \n");							
				return;						
				
			CASE_PRINT (STACK_DBG_MODE_HASH_ERROR		, "STACK (NOT OK) STACK DBG MODE ERROR\n");
			CASE_PRINT (SARRAY_DBG_MODE_HASH_ERROR		, "STACK (NOT OK) SARRAY DBG MODE ERROR\n");

			CASE_PRINT (STACK_TOP_CANARY_DEAD			, "STACK (NOT OK) TOP CANARY IS DEAD\n");
			CASE_PRINT (STACK_BOT_CANARY_DEAD			, "STACK (NOT OK) BOT CANARY IS DEAD\n");
																					
			CASE_PRINT (STACK_HASH_ERROR				, "STACK (NOT OK)\n\
														   SARRAY HASH ERROR!\n");

			CASE_PRINT (SARRAY_NULL_PRT_ERROR			, "STACK (NOT OK) NULL PTR TO STORAGE!  \n");	

			CASE_PRINT (SARRAY_BOT_CANARY_DEAD			, "STACK (NOT OK)\n\
														   SARRAY BOT CANARY DEAD!\n");

			CASE_PRINT (SARRAY_TOP_CANARY_DEAD			, "STACK (NOT OK)\n\
														   SARRAY TOP CANARY DEAD!\n");
			
			CASE_PRINT (SARRAY_DATA_NULL_PTR_ERROR		, "STACK (NOT OK) NULL PTR TO DATA!     \n");

			CASE_PRINT (SARRAY_HASH_ERROR				, "STACK (NOT OK)\n\
														   SARRAY HASH ERROR!\n");
																					
			case BAD_STACK:																	
				return;	

			case SARRAY_DATA_BOT_CANARY_DEAD:
			case SARRAY_DATA_TOP_CANARY_DEAD:
			case SARRAY_DATA_STORAGE_BOT_CANARY_DEAD:
			case SARRAY_DATA_STORAGE_TOP_CANARY_DEAD:
			case SARRAY_DATA_HASH_ERROR:
			case SARRAY_DATA_STORAGE_HASH_ERROR:													
				if (file_check (stack))												
				{										
					fprintf (stack->log_file,  "STACK (NOT OK) , BAD DATA / STORAGES:!\n");	
					stack_out (stack);// fclose is included															
				}	
				else
					printf ("\nerror with log file\n\n");
	
				return;
																					
			default:																		
				if (file_check (stack))												
				{																			
					fprintf (stack->log_file,  "STACK (NOT OK) UNKNOWN ERROR, CODE!\n");	
					fclose (stack->log_file);												
				}		
				else
					printf ("\nerror with log file\n\n");				
				
				return;															
		}																				
	}

	bool file_check( const STACK_t* stack )
	{										
		if (stack->file_hash != hash_f (&(stack->log_file), sizeof (FILE*)))							
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

		sarray_dump (stack->data_storage, stack->log_file);

		fclose (stack->log_file);
	}

	#ifdef FULL_DBG_ENABLE

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

		void shash_f( STACK_t* const stack )
		{
			stack_assert (stack);

			stack->hash = HASH_INIT;

			ahash_f (stack->data_storage);

			stack->hash	= hash_f (stack, sizeof (STACK_t));
		}
	#endif

#endif
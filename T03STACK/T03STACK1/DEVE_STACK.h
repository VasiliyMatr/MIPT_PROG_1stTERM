#pragma once

#include <stdio.h>
#include <assert.h>
#include "utils.h"
#include "DEVE_STACK.h"

// all needed asserts to check stacks
#define stack_assert(stack)																											\
{																																	\
	assert (stack							!= NULL);																				\
	assert (stack->curr_data_storage		!= NULL);																				\
	assert (stack->curr_data_storage->data  != NULL);																				\
}

// for short switch
#define CASE_PRINT(ERROR, STR)																										\
{																																	\
	case ERROR:																														\
		if (stack->log_file != NULL)																								\
		{																															\
			if (!file_check (stack))																								\
			{ 																														\
				printf ("\nerror with log file\n\n");																				\
				return;																												\
			}																														\
				fprintf (stack->log_file,  STR);																					\
				fclose (stack->log_file);																							\
		}																															\
		return;																														\
}

// verification of stack
#define VERIFI_OR_DUMP_N_RETURN( stack, to_return )																					\
{																																	\
	ERR_t ERROR = stack_verifier (stack);																							\
	if (ERROR)																														\
	{																																\
		if (ERROR != BAD_STACK && ERROR != NULL_PTR_2_STACK && file_check (stack))													\
			fprintf (stack->log_file, "\nSTACK ERROR :\nLINE# %d of \"%s\" IN FUNCTION \"%s\"\n\n", __LINE__, __FILE__, __func__);	\
		stack_err_print (stack, ERROR);																								\
		return to_return;																											\
	}																																\
}

//#define NDBG 
// for release version
#ifdef NDBG
	#define assert(statement) ; 
	#define verificator(stack) ;
#endif

typedef struct tagDATA_STORAGE_t DATA_STORAGE_t;
struct tagDATA_STORAGE_t {
	canary_t TOP_CANARY			= CANARY_INIT;

	long hash				= 0;
	long data_hash			= 0;

	void* data				= NULL;

	tagDATA_STORAGE_t* prev	= NULL;

	canary_t BOT_CANARY			= CANARY_INIT;
};

struct STACK_t {
	canary_t TOP_CANARY						 = CANARY_INIT;

	long hash							 = 0;

	FILE*			log_file			 = NULL;
	long			file_hash			 = 0;

	DATA_STORAGE_t*	curr_data_storage    = NULL;

	// from 0 to sizeof_storage - 1
	size_t			curr_data_id		 = 0;

	size_t			sizeof_storage		 = 0;
	size_t			sizeof_element		 = 0;

	ERR_t			ERROR				 = OK;
	
	canary_t BOT_CANARY						 = CANARY_INIT;
 };

/*!
	storage construct func

	input - sizeof_storage	- size of data storage
	input - sizeof_element	- size of data element (in bytes)

	return ptr to created element
*/
DATA_STORAGE_t* data_storage_construct( const size_t sizeof_elem, const size_t sizeof_stor );

/*!
	to free all memory, allocated for data storage

	input - storage			- ptr to data storage

	return ERROR CODE
*/
ERR_t data_storage_deconstruct( DATA_STORAGE_t* storage );

/*!
	to check stack

	input - stack			- ptr to stack

	return ERROR CODE
*/
ERR_t stack_verifier( STACK_t* stack );

/*!
	to check is stack was interrupted

	input - stack			- ptr to stack

	return true if stack was interrupted, false in opposite case
*/
ERR_t interr_check( STACK_t* const stack );

/*!
	to output stack in error case

	input - stack			- ptr to stack
*/
void stack_out( const STACK_t* stack );

/*
	for ERROR output about stack

	input - stack			- ptr to stack
	input - ERROR			- ERROR CODE
*/
void stack_err_print( STACK_t* stack, ERR_t ERROR );

/*
	to check out file adress

	input - stack			- ptr to stack

	return true if file hash is OK
*/
bool file_check( const STACK_t* stack );

/*
	adding new data storage to stack

	input - stack			- ptr to stack

	return ERROR CODE
*/
ERR_t add_data_storage( STACK_t* stack );

/*
	removing new data storage to stack

	input - stack			- ptr to stack

	return ERROR CODE
*/
ERR_t remove_data_storage( STACK_t* stack );

/*
	to check are canaries dead

	input - data			- ptr to data with 2 canaries

	input - sizeof_data		- size of data (in bytes)

	return ERROR CODE 
*/

ERR_t check_canaries( void* data, size_t sizeof_data );
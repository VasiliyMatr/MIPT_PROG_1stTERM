#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstring>

#include "SMART_ARRAY.h"

#ifndef NDBG

	// all needed asserts to check stacks
	#define stack_assert(stack)																											\
	{																																	\
		assert (!isBadReadPtr (stack));																									\
		sarray_assert(stack->data_storage)																								\
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
					fclose  (stack->log_file);																							\
			}																															\
			return;																														\
	}

	// verification of stack
	#define VERIFI_OR_DUMP_N_RETURN( stack, to_return )																					\
	{																																	\
		ERR_t ERROR = stack_verifier (stack);																							\
		if (ERROR)																														\
		{																																\
			if (ERROR != BAD_STACK && ERROR != STACK_NULL_PRT_ERROR && file_check (stack))												\
				fprintf (stack->log_file, "\nSTACK ERROR :\nLINE# %d of \"%s\" IN FUNCTION \"%s\"\n\n", __LINE__, __FILE__, __func__);	\
			stack_err_print (stack, ERROR);																								\
			return to_return;																											\
		}																																\
	}

// for release version
#else 

	#define stack_assert( stack ) ;
	#define CASE_PRINT( ERROR, STR ) ;
	#define VERIFI_OR_DUMP_N_RETURN( stack, to_return ) ;

#endif

const size_t initial_sizeof_stack = 4;

struct STACK_t {

#ifndef NDBG
	canary_t TOP_CANARY						 = CANARY_INIT;
#endif

#ifndef NDBG
	long hash							 = 0;

	FILE*			log_file			 = NULL;
	DBG_MODE_t DBG_MODE					 = NO_DBG;

	long			dbg_mode_hash		 = 0;
	long			file_hash			 = 0;

	ERR_t			ERROR				 = OK;

#endif
	
	SMART_ARRAY_t*	data_storage		 = NULL;

	// from 0 to sizeof_storage - 1
	size_t			curr_data_id		 = 0;

	size_t			sizeof_storage		 = 0;
	size_t			sizeof_element		 = 0;

#ifndef NDBG
	canary_t BOT_CANARY						 = CANARY_INIT;
#endif

 };

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

void shash_f( STACK_t* const stack );

bool check_stack_hash( STACK_t* const stack );
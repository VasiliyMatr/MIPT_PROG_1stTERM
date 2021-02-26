#pragma once

#include "utils.h"

/*
	This is safe stack Structure. It stores data of any size (in bytes) in dynamical memory.
	Data is stored in batches, size of batches is user-specified.
	Size of data and size of batches should not be changed during work with one stack. 
	In case of ERROR, functions will return ERROR CODE (watch utils.h) and write info in log file.
	Also, these functions will not work with stacks that have encountered errors.
*/
struct STACK_t;

/*!
	stack construct func

	input - sizeof_storage	- size of stack data storage
	input - sizeof_element	- size of stack data element (in bytes)
	input - log_name		- log file name

	return ptr to created element
*/
STACK_t* stack_construct( const size_t sizeof_storage, const size_t sizeof_element, const char* log_name, DBG_MODE_t DBG_MODE );

/*!
	to free all memory, allocated for stack
	!don't work with bad stacks!

	input - stack			- ptr to stack

	return ERROR CODE
*/
ERR_t stack_deconstruct( STACK_t* const stack );

/*!
	to push element in stack
	!don't work with bad stacks!

	input - stack			- ptr to stack
	input - value			- ptr to element

	return ERROR CODE
*/
ERR_t stack_push( STACK_t* stack, void* value );

/*!
	to pop element from stack
	!don't work with bad stacks!

	input  - stack			- ptr to stack
	output - dist			- where to pop

	return ERROR CODE
*/
ERR_t stack_pop( STACK_t* stack, void* dist );
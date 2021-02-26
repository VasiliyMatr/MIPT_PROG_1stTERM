#pragma once

// ifdef HASH_DBG - HASH DEBUG; ifdef DBG_CANARIES - CANARIES DEBUG
#define DBG_HASH
#define DBG_CANARIES

// hash constant
const long HASH_INIT = 0xBEEEEBAA;

const long long CANARY_INIT = 0xBEEEEBAABEEEEBAA;

typedef long long canary_t;

// for outputs about errors
enum ERR_t {
	// no errors
	OK					=  0  ,
	// error with memory allocation
	MEM_ERR				= -228,
	// unknown error
	UNKNOWN_ERR				  ,
	// stack is empty
	EMPT					  ,


	// NULL ptr to stack
	NULL_PTR_2_STACK			=  -1337,
	// NULL ptr to data batch struct
	NULL_PTR_2_STORAGE					,
	// NULL ptr to data
	NULL_PTR_2_DATA						,
	// NULL ptr to smart array
	NULL_PTR_2_SARRAY					,


	// hash errors
	STACK_HASH_ERROR					,
	DATA_STRUCT_HASH_ERROR				,
	DATA_HASH_ERROR						,
	TOP_CANARY_ERROR					,
	BOT_CANARY_ERROR					,
	
	// tryed to use bad stack
	BAD_STACK							,

	BAD_SARRAY
};
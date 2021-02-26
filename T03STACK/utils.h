#pragma once

//#define FULL_DBG_ENABLE
#define CANARY_DBG_ENABLE
//#define NDBG

#ifdef FULL_DBG_ENABLE
	#undef CANARY_DBG_ENABLE
	#undef NDBG
#endif
#ifdef CANARY_DBG_ENABLE
	#undef HASH_DBG_ENABLE
	#undef NDBG
#endif
#ifdef NDBG
	#undef HASH_DBG_ENABLE
	#undef CANARY_DBG_ENABLE
#endif

#if !defined NDBG && !defined FULL_DBG_ENABLE && !defined CANARY_DBG_ENABLE
	#define FULL_DBG_ENABLE
#endif

// poison value
const unsigned char POISON_VAL = 0xF0;

// hash constant
const long HASH_INIT = 0xBEEEEBAA;

// canaries init value
const long long CANARY_INIT = 0xBEEEEBAABEEEEBAA;

// canaries type
typedef long long canary_t;

// for outputs about errors
enum ERR_t {
	// no errors
	OK					=  0			,
	// error with memory allocation
	MEM_ERR				= -228			,
	// unknown error
	UNKNOWN_ERR							,
	// empty
	EMPT								,
	// minor error
	MINOR_ERROR							,
	// bad ptr error
	BAD_PTR_ERROR						,

	/* stack errors */
	STACK_NULL_PRT_ERROR = 1337			,
	STACK_HASH_ERROR					,
	STACK_TOP_CANARY_DEAD				,
	STACK_BOT_CANARY_DEAD				,
	BAD_STACK							,

	/* sarray errors */
	// ptrs errors
	SARRAY_NULL_PRT_ERROR				,
	SARRAY_DATA_NULL_PTR_ERROR			,
	SARRAY_DATA_STORAGE_NULL_PTR_ERROR		,

	// hash errors
	SARRAY_HASH_ERROR					,
	SARRAY_DATA_HASH_ERROR				,
	SARRAY_DATA_STORAGE_HASH_ERROR		,
	
	// canaries errors
	SARRAY_TOP_CANARY_DEAD				,
	SARRAY_BOT_CANARY_DEAD				,

	SARRAY_DATA_STORAGE_TOP_CANARY_DEAD ,
	SARRAY_DATA_STORAGE_BOT_CANARY_DEAD ,

	SARRAY_DATA_TOP_CANARY_DEAD			,
	SARRAY_DATA_BOT_CANARY_DEAD			,

	// bad sarray used
	BAD_SARRAY							,

	// canary default errors
	TOP_CANARY_DEAD						,
	BOT_CANARY_DEAD						,

	// for DBG_MODE and log file hash errors
	SARRAY_DBG_MODE_HASH_ERROR			,
	STACK_DBG_MODE_HASH_ERROR			,
	STACK_LOG_FILE_HASH_ERROR			,
};

enum DBG_MODE_t {
	// only canaries
	CANARY_DBG		,
	// hash and canaries
	FULL_DBG		,
	// no debug
	NO_DBG
};

bool isBadReadPtr( const void* ptr );

void add_poison( void* to_poison, size_t numof_bytes_2_poison );
#include "HASH_STASH.h"
#include "DEVE_STACK.h"
#include "SMART_ARRAY.h"



long hash_f( const void* to_hash, const size_t numof_bytes )
{
	assert (to_hash != NULL);
	assert (numof_bytes > 0);

	long hash = 1;
	const size_t byte_move = sizeof (long) - 1;


	for (size_t i = 0; i < numof_bytes; i++)
		hash = -hash * *((char* )to_hash + i) + hash>>byte_move ^ hash<<1;

	return hash;
}

ERR_t check_canaries( void* data, size_t sizeof_data )
{
	assert (sizeof_data > sizeof (canary_t) * 2);

	if (isBadReadPtr(data))
		return BAD_PTR_ERROR;

	if (*(canary_t* )data != CANARY_INIT)
		return TOP_CANARY_DEAD;
	if (*(canary_t* )((int)data + sizeof_data - sizeof (canary_t)) != CANARY_INIT)
		return BOT_CANARY_DEAD;
	return OK;
}
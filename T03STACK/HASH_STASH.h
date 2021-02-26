#pragma once

#include "DEVE_STACK.h"

bool check_stack_hash( STACK_t* const stack );

ERR_t check_canaries( void* data, size_t sizeof_data );
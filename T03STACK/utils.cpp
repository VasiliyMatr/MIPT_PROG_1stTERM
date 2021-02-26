#include <stdio.h>
#include <windows.h>
#include <memoryapi.h>
#include <assert.h>

#include "utils.h"

bool isBadReadPtr( const void* ptr )
{
    MEMORY_BASIC_INFORMATION mbi = {0};
    if (::VirtualQuery(ptr, &mbi, sizeof(mbi)))
    {
        DWORD mask = (PAGE_READONLY|PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY);
        bool b = !(mbi.Protect & mask);
        // check the page is not a guard page
        if (mbi.Protect & (PAGE_GUARD|PAGE_NOACCESS)) b = true;

        return b;
    }
    return true;
}

void add_poison( void* to_poison, size_t numof_bytes_2_poison )
{
	assert (!isBadReadPtr (to_poison));

	for (size_t i = 0; i < numof_bytes_2_poison; i++)
	{
		((char* )to_poison)[i] = POISON_VAL;
	}
}
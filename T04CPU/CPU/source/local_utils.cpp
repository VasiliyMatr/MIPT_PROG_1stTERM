
#include "local_utils.h"

hash_t hashF( const void* toHash, const size_t numOfBytes )
{
    if (toHash == nullptr)
    {
        /* Bad ptr to hashable sequence. */
        CRIT_ERROR_OUT_ ("BAD toHash ptr!");
        return rand ();
    }

    if (numOfBytes == 0)
    {
        /* Zero sequence size */
        CRIT_ERROR_OUT_ ("0 size of hashable value!");
        return rand ();
    }

    /* Hashing stuff */

        const size_t byte_move = sizeof (hash_t) - 1;
        int hash = HASH_INIT_;

        for (size_t i = 0; i < numOfBytes; i++)
        {
            hash = (-hash * *((char* )toHash + i) + hash >> byte_move ^ hash << 1) + hash - i;
        }
    /* ************* */

    return hash;
}

int getPoison( int* )
{
    return 0xF00DF00D;
}

double getPoison( double* )
{
    return NAN;
}
#include "utils.h"
#include "my_sort.h"

ERR_t my_qsort( void* data, size_t num_of_e, size_t size_of_e, cmpr_f compare )
{
	assert (data    != NULL);
	assert (compare != NULL);

	// nothing to sort
	if (num_of_e < 2)
		return OK;

	// ptrs 
	char* left   =	(char* )data;
	char* right  =	(char* )data + (num_of_e - 1) * size_of_e;

	void* center =  NULL;

	center = calloc (1, size_of_e);

	if (center == NULL)
		return MEM_ERR;

	//copying central element for compare 
	memcpy (center, (char* )data + ((num_of_e - 1) / 2) * size_of_e, size_of_e);

	while (left <= right)
	{
		// skipping lesser elements
		while (compare (left, center) < 0)
			left += size_of_e;

		// skipping bigger elements
		while (compare (right, center) > 0)
			right -= size_of_e;

		if (left <= right)
		{
			ERR_t ERR = my_swap (left, right, size_of_e);
			
			if (ERR != OK)
				return ERR;
			left += size_of_e;
			right -= size_of_e;
		}
	}

	free (center);

	if (num_of_e > 2)
	{
		// left part sort
		ERR_t ERR 
			= my_qsort(data, ((int)right - (int)data) / size_of_e + 1, size_of_e, compare);
		if (ERR != OK)
			return ERR;
		
		// right part sort
		ERR		= my_qsort(left, num_of_e - ((int)left - (int)data) / size_of_e, size_of_e, compare);
		if (ERR != OK) 
			return ERR;
	}

	return OK;
}

ERR_t my_swap( void* first_e, void* second_e, size_t size_of_e )
{
	assert (first_e != NULL);
	assert (second_e != NULL);
	assert (size_of_e != 0);

	void* tmp = calloc (1, size_of_e);
	if (tmp == NULL)
		return MEM_ERR;

	memcpy (tmp, first_e, size_of_e);
	memcpy (first_e, second_e, size_of_e);
	memcpy (second_e, tmp, size_of_e);
	free (tmp);

	return OK;
}
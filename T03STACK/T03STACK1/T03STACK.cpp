#include <stdlib.h>

#include <stdio.h>
#include <cstring>

#include "utils.h"
#include "USER_STACK.h"
#include "DEVE_STACK.h"
#include "SMART_ARRAY.h"


typedef double data_t;

int main()
{
	SMART_ARRAY_t* sarray = smart_array_construct(sizeof (data_t), 100, 5, FULL_DBG);

	data_t data = 228;

	sarray_put_data(sarray, 0, 10, &data);
	sarray_get_data(sarray, 0, 10, &data);



	del_storages_from_sarray(sarray, 4);
	add_storages_to_sarray(sarray, 50);

	STACK_t* test  = stack_construct (100	, sizeof(data_t)	, "log_1");
	STACK_t* test2 = stack_construct (10	, sizeof(data_t) / 2, "log_2");
	STACK_t* test3 = stack_construct (100	, sizeof(data_t)	, "log_3");

	for (long i = 0; i < 1030; i++)
	{
		data_t val = i;
		stack_push (test,  &val);
		stack_push (test2, &i);
		stack_push (test3, &val);
		printf ("%i\n", i);
	}

	double* executor = (double*)test->curr_data_storage->prev->prev->prev->prev->prev->prev->prev;
	
	memcpy (test3, &data, 8);
	printf ("\n%i\n", stack_deconstruct(test3));
	getchar();

	//stack_out (test2);

	test2->curr_data_id = 228;

	stack_deconstruct(test2);

	while (stack_pop(test, (void*)(&data)) != EMPT)
	{
		int i = 0;
		i++;
		*((long*)((int)executor)) = 228;

		//test->BOT_CANARY = 0;

		printf("%f\n", data);
	}

	stack_deconstruct(test);
	stack_deconstruct(test2);
	stack_deconstruct(test3);

	return 1;
}
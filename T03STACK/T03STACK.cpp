#include <stdio.h>

#include "utils.h"
#include "USER_STACK.h"
#include "DEVE_STACK.h"

typedef double data_t;


int main( void )
{
    data_t data = 228;
	
	STACK_t* test1 = stack_construct (100	, sizeof(data_t)	, "log_1", FULL_DBG);
	STACK_t* test2 = stack_construct (10	, sizeof(data_t) / 2, "log_2", CANARY_DBG);
	STACK_t* test3 = stack_construct (100	, sizeof(data_t)	, "log_3", FULL_DBG);

	for (long i = 0; i < 1030; i++)
	{
		data_t val = i;
		stack_push (test1,  &val);
		stack_push (test2, &i);
		stack_push (test3, &val);
		printf ("%i\n", i);
	}
	
	test3->data_storage->storages->data = 0;

	printf ("\n%i\n", stack_deconstruct(test3));
	getchar();

	stack_out (test2);

	test2->curr_data_id = 228;

	stack_deconstruct(test2);

    *(int*)(test1->data_storage->storages->data) = 10;

	while (stack_pop(test1, (void*)(&data)) != EMPT)
	{
		int i = 0;
		i++;

		printf("%f\n", data);
	}

	stack_deconstruct(test1);
	stack_deconstruct(test3);

	return 1;
}
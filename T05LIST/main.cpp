#include "list.h"

int main()
{

    /* some testing stuff */

    list_t <double> test_list  (40, "dump_file.dot");

    int ids[400] = { -1 };

    for (int i = 0; i < 10; i++)
    {
        ids[i] = test_list.attach_to_data_head (i * 2);
    }

    for (int i = 0; i < 5; i++)
    {
        ids[i * 2] = test_list.attach_to_data_tail (-i * 2);
    }

    for (int i = 0; i < 8; i++)
    {
        ids[i * 2] = test_list.attach_next_by_id (5, 228);
    }

    for (int i = 0; i < 8; i++)
    {
        ids[i * 2] = test_list.attach_prev_by_id (12, 1337);
    }

    test_list.dump();

    for (int i = 0; i < 800000; i++)
    {
        test_list.slow_get_physical_unit_id_by_logical_id (12);
    }

    test_list.resize(30);

    for (int i = 0; i < 800000; i++)
    {
        test_list.slow_get_physical_unit_id_by_logical_id (12);
    }
    
    test_list.dump();

    //test_list.qsort_data_units();

}
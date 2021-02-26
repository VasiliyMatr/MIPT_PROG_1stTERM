#include "utils.h"
#include "my_sort.h"
#include "PARSED_F_funcs.h"

//{============================================================================
//! function to work with files
//!
//! @param[in]          in_name  - name of file to open
//! @param[out]         out_name - name of file to write the result in
//!
//! @return ERROR code
//}============================================================================

int file_worker( char* in_name, char* out_name );

//{----------------------------------------------------------------------------
//! ...oh, those tests...
//}----------------------------------------------------------------------------

int unit_tests( PARSED_FILE_t* file, char* in_name );

//{----------------------------------------------------------------------------
//! ...oh, those tests...
//}----------------------------------------------------------------------------

int alph_test( PARSED_FILE_t* file, cmpr_f compare, const char* name  );

int main()
{
	char in_name[] = "f.txt";
	char out_name[] = "g.txt";

	return ERR_WORKER (file_worker (in_name, out_name));
}


int file_worker( char* in_name, char* out_name )
{
	// it's acceptable if in_name == out_name //
	assert (in_name != NULL);
	assert (out_name != NULL);

	PARSED_FILE_t in_file = { NULL, NULL, NULL, MEM_ERR, MEM_ERR };

	if (int ERR = get_parsed_text (&in_file, in_name) != OK)
		return ERR;

	FILE* out_file = NULL;
	if ((out_file = fopen (out_name, "wb")) == NULL)
		return FILE_ERR;


	// SORTING AND PRINTING //

	if (int ERR = sort_and_print (out_file, &in_file, compare) != OK)
		return ERR;

	if (int ERR = sort_and_print (out_file, &in_file, reversed_compare) != OK)
		return ERR;

	fprintf(out_file, "%s", in_file.data);

	// SORTING AND PRINTING //


    #ifndef NDBG

		if (int ERR = unit_tests (&in_file, in_name) != OK)
			return ERR;

	#endif	


	pf_close (&in_file);

	fclose (out_file);
	out_file = NULL;

	return OK;
}

int unit_tests( PARSED_FILE_t* file, char* in_name )
{
	assert (file != NULL);
	assert (in_name != NULL);

	FILE* in_file = NULL;
	if ((in_file = fopen (in_name, "rb")) == NULL)
		return FILE_ERR;

	int num_of_c_in = 0;
	int num_of_c_out = 0;
	char curr_c = 0;

	while ((curr_c = fgetc (in_file)) != EOF)
		if (!isend (curr_c))
			num_of_c_in++;

	for (int i = 0; i < file->num_of_c; i++)
		if (!isend (file->data[i]))
			num_of_c_out++;

	printf ("\n\nUNIT TESTS!!!\n\n");
	printf ("there are %i symbols in input file!\n", num_of_c_in);
	printf ("there are %i symbols in output file!\n", num_of_c_out);

	printf ("diff : %i\n", num_of_c_in - num_of_c_out);

	fclose (in_file);
	in_file = NULL;

	if (int ERR = alph_test (file, compare, "straight") != OK)
		return ERR;
	if (int ERR = alph_test (file, reversed_compare, "reversed") != OK)
		return ERR;

	printf ("\nUNIT TESTS FINISHED!!!\n");

	return OK;
}

int alph_test( PARSED_FILE_t* file, cmpr_f compr, const char* name )
{
	assert (compr != NULL);
	assert (file != NULL);
	assert (name != NULL);

	int ERR = MEM_ERR;

	if ((ERR = my_qsort (file->strs, file->num_of_s, sizeof (STR), compr)) != OK)
		return ERR;

	printf ("checking alph refer (%s)...", name);

	for (int i = 0; i < file->num_of_s - 1; i++)
		if (compr ((void*)&((file->strs)[i]), (void*)&((file->strs)[i + 1])) > 0)
		{
			printf ("NOT OK!!!%i\n", i);
			return OK;
		}

	printf ("OK!!!\n");

	return OK;
}
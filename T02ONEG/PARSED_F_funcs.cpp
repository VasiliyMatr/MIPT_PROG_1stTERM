#include "utils.h"
#include "my_sort.h"
#include "PARSED_F_funcs.h"

ERR_t get_parsed_text( PARSED_FILE_t* file, char* in_name )
{
	assert (file != NULL);
	assert (in_name != NULL);

	if ((file->file = fopen (in_name, "rb")) == NULL)
		return FILE_ERR;

	file->num_of_c = file_reader (file->file, &(file->data));

	// don't need this file now //
	fclose (file->file);
	file->file = NULL;

	if (file->num_of_c == MEM_ERR)
		return MEM_ERR;

	parser (file);

	if (file->num_of_s == MEM_ERR)
		return MEM_ERR;

	return OK;
}

int file_reader( FILE* in_file, char** data )
{
	assert (in_file != NULL);

	int num_of_chars = 0;

	num_of_chars = get_length (in_file);

	*data = (char*)calloc (num_of_chars + 1, sizeof (char));// allocating memory for data + '\0' //

	if (*data == NULL)
		return MEM_ERR;

	fread (*data, sizeof (char), num_of_chars, in_file);// reading data //

	(*data)[num_of_chars] = '\0';// end of the string //

	return num_of_chars;
}

size_t get_length( FILE* in_file )
{
	assert (in_file != NULL);

	fseek (in_file, 0, SEEK_END);
	int num_of_chars = ftell (in_file);// getting num of chars in file //
	rewind (in_file);

	return num_of_chars;
}

ERR_t parser( PARSED_FILE_t* file )
{
	assert (file->data != NULL);
	assert (file->num_of_c >= 0);

	count_strs (file);

	file->strs = (STR*) calloc (file->num_of_s, sizeof(STR));// allocating memory for strs //

	if (file->strs == NULL)
		return MEM_ERR;

	setup_ptrs (file);

	return OK;
}

void count_strs( PARSED_FILE_t* file )
{
	assert (file->data != NULL);
	assert (file->num_of_c >= 0);

	char curr_c = 0;
	bool isstr = false;

	for (int curr_c_id = 0; curr_c_id < file->num_of_c; curr_c_id++)// counting num of words //
	{
		if (isend ((curr_c = (file->data)[curr_c_id])))
		{
			isstr = false;
		}
		else if (!isstr)
		{
			isstr = true;
			file->num_of_s++;
		}
	}	
}

void setup_ptrs( PARSED_FILE_t* file )
{
	assert (file->data != NULL);
	assert (file->num_of_c >= 0);

	char curr_c = 0;
	int curr_c_id = 0;
	int old_curr_c_id = 0;

	for (int curr_s_id = 0; curr_s_id < file->num_of_s; curr_s_id++)// setup of the pointers
	{
		// skipping trash symbols
		while (isend ((curr_c = (file->data)[curr_c_id])))
		{
			// no more strs in file
			if (curr_c_id >= file->num_of_c)
				return;
			curr_c_id++;
		}

		// str found
		(file->strs)[curr_s_id].str = (char*)&((file->data)[curr_c_id]);

		// skipping str symbols
		while (!isend ((curr_c = (file->data)[curr_c_id])))
			curr_c_id++;

		// length found
		(file->strs)[curr_s_id].length = curr_c_id - old_curr_c_id - 2;// \n and \r
		old_curr_c_id = curr_c_id;
	}
}

void pf_close( PARSED_FILE_t* file )
{
	assert (file != NULL);
	assert (file->data != NULL);
	assert (file->strs != NULL);

	free (file->data);
	free (file->strs);
	file->data = NULL;
	file->strs = NULL;
	file->num_of_c = MEM_ERR;
	file->num_of_s = MEM_ERR;
}

ERR_t sort_and_print( FILE* out_file, const PARSED_FILE_t* in_file, cmpr_f compare )
{
	assert (out_file != NULL);
	assert (in_file != NULL);
	assert (compare != NULL);

	ERR_t ERR = my_qsort (in_file->strs, in_file->num_of_s, sizeof (STR), compare);

	if (ERR != OK)
		return ERR;

	print_in_file (out_file, in_file);
	fprintf (out_file, "\n\n\n\n\n\n\n\n");

	return OK;
}

void print_in_file( FILE* out_file, const PARSED_FILE_t* in_file )
{
	assert (out_file != NULL);
	assert (in_file->strs != NULL);
	assert (in_file->num_of_s >= 0);

	for (int i = 0; i < in_file->num_of_s; i++)
	{
		fwrite (in_file->strs[i].str, sizeof (char), in_file->strs[i].length, out_file);
		fprintf (out_file, "\n");
	}
}
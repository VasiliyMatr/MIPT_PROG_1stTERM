#include "utils.h"

#define ERR_PRINT(ERR)  {\
							fprintf (stderr, "%s", ERR);\
							break;\
						} 

int ERR_WORKER( int code )
{
	fprintf (stderr, "\n\n");

	switch (code)
	{
		case OK:
			fprintf (stderr, "NO ERRORS!!!\n\n");
			return 0;

		case MEM_ERR:
			ERR_PRINT ("MEMORY ERROR!!!")

		case FILE_ERR:
			ERR_PRINT ("ERROR WHILE WORKING WITH FILE!!!")
	
		default:
			ERR_PRINT ("UNKNOWN ERROR!!!")

	}
			
	fprintf (stderr, "\n\n");

	return 1;
}

int compare( void const* str_2_compare_A, void const* str_2_compare_B )
{
	assert (str_2_compare_A != NULL);
	assert (str_2_compare_B != NULL);
	//assert (str_2_compare_A != str_2_compare_B);

	const char* A = (*(STR*)str_2_compare_A).str;
	const char* B = (*(STR*)str_2_compare_B).str;

	while (true)
	{
		while (!isalpha (*A) && !isend (*A))
			A++;

		while (!isalpha (*B) && !isend (*B))
			B++;

		if (isend (*A) && isend (*B))
			return 0;

		if (isend (*A) || toupper (*B) > toupper (*A))
			return -1;

		if (isend (*B) || toupper (*A) > toupper (*B))
			return  1;
		A++;
		B++;
	}
}

int reversed_compare( const void* str_2_compare_A, const void* str_2_compare_B )
{
	assert (str_2_compare_A != NULL);
	assert (str_2_compare_B != NULL);
	//assert (str_2_compare_A != str_2_compare_B);

	//to remember starting positions //
	const char* A = (*(STR*)str_2_compare_A).str + ((*(STR*)str_2_compare_A).length - 1) * sizeof (char);
	const char* B = (*(STR*)str_2_compare_B).str + ((*(STR*)str_2_compare_B).length - 1) * sizeof (char);

	const char* a = (*(STR*)str_2_compare_A).str;
	const char* b = (*(STR*)str_2_compare_B).str;

	assert(A != NULL);
	assert(B != NULL);

	while (A >= a && B >= b)// reversed compare //
	{

		while (!isalpha (*A) && A > a)
			A--;

		while (!isalpha (*B) && B > b)
			B--;

		if (toupper (*B) > toupper (*A))
			return -1;

		if (toupper (*A) > toupper (*B))
			return  1;

		A--;
		B--;
	}

	if (A + 1 == a && B + 1 == b)// equal words //
		return 0;

	if (B >= b)// first word is the beginning of the second word //
		return -1;

	return 1;// opposite case //
}

bool isend( char ch )
{
	return ch == '\n' || ch == '\r' || ch == '\0';
}
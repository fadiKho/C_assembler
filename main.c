/*
 ============================================================================
 Name of file: main.c
 Author      : Sapir Lipelis & Fadi Khoury
 Description : The main.c file is responsible for opening the input assembly files and call the first and second read methods.
		For each file, main opens the file, call the reading methods and close the file before moving to the next input file.
 ============================================================================
 */
#include "assembler.h"

int main(int argc, char **argv)
{
	FILE * input;
	int errorCounter = 0;
	char fileName[MAX_LENGTH];

	if(argc <= 1) /* no input file inserted */
	{
		fprintf(stderr, "ERROR: No assembly files have been entered.\n");
		return 1;
	}

	while(--argc > 0)
	{
		/* opening current assembly file */
		if((input = fopen(*++argv,"r")) == NULL)
		{
			fprintf(stderr, "ERROR: Cannot open file %s\n" , *argv);
		}
		/* file opened, starting assembler actions */
		else
		{
			strcpy(fileName, strtok(*argv, "."));
			/* first read - returns number of errors found */
			errorCounter += firstFileRead(input);
			if(errorCounter == 0) /* start second read only if there were no errors in first read */
			{
				/* second read - returns number of errors found */
				secondRead(input, (fileName));
			}
			/* close current input file */
			fclose(input);

		}
	}
	return 0;
}

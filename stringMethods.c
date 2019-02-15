/*
 ============================================================================
 Name of file: stringMethods.c
 Author      : Sapir Lipelis & Fadi Khoury
 Description : stringsMethods.c file contains functions dealing with strings (char arrays) and strings manipulations.
 ============================================================================
 */

#include "assembler.h"
#include "numbers.h"

/* check if the given string is empty(or null). returns 1 if empty, returns 0 if not. */
int isEmpty(char * currentLine)
{
	int i;
	if(currentLine == NULL)
		return 1;
	for(i = 0; i < strlen(currentLine); i++)
	{
		if(!isspace(currentLine[i]))
		{
			return 0;
		}
	}
	return 1;
}

/* check if the given string is an assembly comment line (starts with ;). returns 1 if is comment, returns 0 if not */
int isComment(char * currentLine)
{
	if(currentLine[0] == ';')
	{
		return 1;
	}
	return 0;
}

/* responsible for removing white spcaces from the beginning of the given string */
char * skipWhiteSpaces(char * line)
{
	char choppedLine[MAX_LENGTH] = {'\0'};
	int i = 0;;
	strncpy(choppedLine, line, MAX_LENGTH);
	while(choppedLine[i] != '\0')
	{
		if(isspace(choppedLine[i]))
		{
			strncpy(line, choppedLine + i + 1, MAX_LENGTH);
			i++;
		}
		else
			break;
	}
	return line;
}

/* counts the number of occurrences of given character in the given string. returns the occurrences count */
int occurrencesCount(char * line , char ch)
{
	int count = 0;
	int i;
	for (i=0; i < strlen(line); i++)
	{
		if(line[i] == ch)
		{
			count++;
		}
	}
	return count;
}

/* receives memoryWord as char array with 15 binary values (0 or 1) and convert every 3 values into a single special 8 basis value. values inserted to the second char array parameter - specialBasis */
void getSpecialBasisMemoryWord(int * memoryWord , char * specialBasis)
{
	int i = MEMORY_WORD_SIZE - 1; /* starting translation from end, memory word have 15 values */
	int j = i - 1; /* one element next to i */
	int k = j - 1; /* one element next to J */
	int index;
	int currentDigit = 0;

	for(index = 0; index < MEM_S_BASIS - 1; index++) /* specialBasis is 5 chracters array, with 1 more value for '\0' */
	{
		currentDigit = toDecimal(memoryWord[i], memoryWord[j], memoryWord[k]); /* sends every 3 digits for translation: 3 digits in binary are 1 digit in octal */
		specialBasis[index] = eightSpecialBasis[currentDigit]; /* insert matched character for current digit from the special 8 basis */
		/* move to the next 3 values in memory word */
		i = k -1;
		j = i -1;
		k = j -1;
	}
	specialBasis[index] = '\0'; /* insert to indicate end of string */
}

/* converts the given int ic into a special 8 basis string. the function is modifing the given char array - specialAddress*/
void getSpecialBasisIC(int ic, char * specialAddress)
{
	int reminder = 0;
	int i = 0;
	/* make ic into octal (8) basis */
	int icOctal = toOctal(ic);
	int numberOfDigits = countDigits(icOctal);
	icOctal = reverseNumber(icOctal);

	while(icOctal != 0)
	{
		reminder = icOctal % 10;
		specialAddress[i] = eightSpecialBasis[reminder]; /* insert special value for each digit in the reversed octal number */
		icOctal = icOctal / 10; /* move to next digit */
		numberOfDigits--;
		i++;
	}
	if(numberOfDigits > 0) /* zeroes at end of number */
	{
		while(numberOfDigits > 0)
		{
			specialAddress[i] = eightSpecialBasis[0]; /* insert value of zero in special basis until number of digits is zero */
			i++;
			numberOfDigits--;
		}
	}
	specialAddress[i] = '\0'; /* insert to indicate end of string */

}

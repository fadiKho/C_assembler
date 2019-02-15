/*
 ============================================================================
 Name of file: numbersMethods.c
 Author      : Sapir Lipelis & Fadi Khoury
 Description : The numbersMethods.c file contains functions dealing with integers and integer manipulations.
 ============================================================================
 */

#include "assembler.h"

/* counts the number of digits of the given integer parameter. returns the digits count */
int countDigits(int number)
{
	int counter = 1;
	number = number / 10;
	while(number != 0)
	{
		number = number / 10;
		counter++;
	}
	return counter;
}

/* converts the three binary values parameters into decimal integer. first parameter is most segnificant digit, last parameter is least segnificant digit.
 * return the new value */
int toDecimal(int bigDigit, int middleDigit , int smallDigit)
{
	int number = 0;
	int binaryBase = 2;

	number = smallDigit + (middleDigit * binaryBase) + (bigDigit * binaryBase * binaryBase);

	return number;
}

/* converts integer in decimal base to integer in octal base. returns the new value */
int toOctal( int value)
{
	int reminder = 0;
	int i = 1;
	int octal = 0;
	while (value != 0)
	{
	    reminder = value % 8;
	    value = value / 8;
	    octal += reminder * i;
	    i = i * 10;
	}
	return octal;
}

/* reverse the digits order of the given integer. return the reversed number */
int reverseNumber(int number)
{
   int reverse = 0;

   while (number != 0)
   {
      reverse = reverse * 10;
      reverse = reverse + number % 10;
      number = number / 10;
   }

   return reverse;
}

/*
 ============================================================================
 Name of file: numbers.h
 Author      : Sapir Lipelis & Fadi Khoury
 Description : The numbers.h header contains functions prototypes dealing with integers.
 ============================================================================
 */

#ifndef NUMBERS_H
#define NUMBERS_H

/* converts integer in decimal base to integer in octal base. returns the new value */
int toOctal( int value);
/* reverse the digits order of the given integer. return the reversed number */
int reverseNumber(int number);
/* converts the three binary values parameters into decimal integer. first parameter is most segnificant digit, last parameter is least segnificant digit.
 * return the new value */
int toDecimal(int , int , int);
/* counts the number of digits of the given integer parameter. returns the digits count */
int countDigits(int number);

#endif

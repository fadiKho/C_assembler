/*
 ============================================================================
 Name of file: strings.h
 Author      : Sapir Lipelis & Fadi Khoury
 Description : The string.h header contains functions prototypes for strings manipulations
 ============================================================================
 */

#ifndef STRINGS_H
#define STRINGS_H

/* check if the given string is empty(or null). returns 1 if empty, returns 0 if not. */
int isEmpty(char *);
/* check if the given string is an assembly comment line (starts with ;). returns 1 if is comment, returns 0 if not */
int isComment(char *);
/* responsible for removing white spcaces from the beginning of the given string */
char * skipWhiteSpaces(char *);
/* counts the number of occurrences of given character in the given string. returns the occurrences count */
int occurrencesCount(char * , char);
/* converts the given int into a special 8 basis string. the function is modifing the given char array */
void getSpecialBasisIC(int , char * );
/* receives a char array with 15 binary values (0 or 1) and convert every 3 values into a single special 8 basis value. values inserted to the second char array parameter */
void getSpecialBasisMemoryWord(int *  , char * );

#endif

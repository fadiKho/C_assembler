/*
 ============================================================================
 Name of file: validation.h
 Author      : Sapir Lipelis 311150007
 Description : The validation.h header contains functions prototypes performing validations for operands and symbols in the program.
 ============================================================================
 */
#ifndef VALIDATION_H
#define VALIDATION_H

/* check if the given line contains legal symbol. The function inserts the symbol name into the second given char array. returns number of errors found */
int checkSymbol(char * , int, char *);
/* check if the given char array token contains legal integer. The given int pointer (2nd parameter) will point to the integer value. returns number of errors found */
int checkNumber(char * , int * , int );
/* check the given dynamic operand. check the symbol and the numbers range. returns number of errors found */
int checkDynamic(char * , int * , int * , int );
/* check if the given char array is a legal register name. if the register is found - returns the register name from the registers array */
char * findRegister(char * );
/* check if the given operands types are legal for the operation in the input line. returns number of errors found */
int checkOperandsForCommand(int , int , char * , int);
/* analyze the operand type in the given char array. modify the int pointer (2nd parameter) to point at the matched type (from enumeration). returns number of errors found */
int checkOperandType(char * , int * , int );
/* check if the symbol name in entry declaration exist in the symbols table. also check for double declaration of same entry. if legal - insert new entry node to EntryList.
 * returns number of errors found */
int checkEntrySymbol(char *  , int );
/* check if the symbol in dynamic operand exist in symbols table and is not external. if is legal - modify int pointer to point to the address value of the symbol.
 * returns number of errors found */
int checkDynamicSymbol(char * , int * , int );

#endif

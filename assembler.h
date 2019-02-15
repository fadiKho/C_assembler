/*
 =========================================================================================================
 Name of file: assembler.h
 Author      : Sapir Lipelis & Fadi Khoury
 Description : The assembler.h header contains the first and second read prototypes, 
		structs used by the program, enums and externals (globals in other files in this program)
 =========================================================================================================
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LENGTH 81 /* max length of line */
#define MAX_LABEL 31 /* max length of symbol name */
#define MEMORY_WORD_SIZE 15 /* size of memory word in this program */
#define MAX_DATA 16383 /* value of biggest number that can be represented in 15 binary digits */
#define MIN_DATA -16384 /* value of smallest number that can be represented in 15 binary digits */
#define MEM_S_BASIS 6 /* length of memory word in the special 8 basis (5 characters + 1 '\0') */
#define MIN_13_NUM -4096 /* value of smallest number that can be represented in 13 binary digits */
#define MAX_13_NUM 4095 /* value of biggest number that can be represented in 13 binary digits */

/* type for extern labels references in file, each label holds list of references. if not extern or extern with no use in the input, this list is null */
struct referenceNode
{
	int IC; /* IC address */
	struct referenceNode * next;
};
typedef struct referenceNode referenceNode;

/* type for a node in linked list for the symbols table*/
struct symbolNode
{
	char name[MAX_LABEL];
	int address; /* counter of IC or DC */
	int isExtern;
	int isAttachedToOperation;
	int memoryWord[MEMORY_WORD_SIZE]; /* will hold binary value based on the IC counter and ARE matched for the label */
	struct referenceNode * references; /* list of references if the symbol is extern */
	struct symbolNode * next;
};
typedef struct symbolNode symbolNode;

/* type for a node in linked list for the Data storage */
struct dataNode
{
	int BinaryCode[MEMORY_WORD_SIZE]; /* the data in 15 digits binary code */
	int address; /*receive value from DC counter */
	struct dataNode * next;
};
typedef struct dataNode dataNode;

/* type for valid operation */
typedef struct operation
{
	char * opName;
	int opCode; /* ineter value for the operation code */
	int numberOfParameters;
} operation;

/* type for memory words */
struct memoryWordHolder
{
	int memoryWord[MEMORY_WORD_SIZE];
	int counterIC;
	struct memoryWordHolder * next;
};
typedef struct memoryWordHolder memoryWordHolder;

/* type for entry symbols list, as declared in input */
struct entryWordHolder
{
	char label[MAX_LABEL]; /* name of entry symbol */
	int address;
	struct entryWordHolder * next;
};
typedef struct entryWordHolder entryWordHolder;

/* enums*/
/* enumeration for address resolutions for operands */
enum addressResolution {IMMEDIATE = 0, DIRECT = 1, DYNAMIC = 2, REGISTER = 3};
/* enumeration for type of address , bytes 0-1 */
enum addressType {ABSOLUTE = 0, EXTERNAL = 1, RELOCATABLE = 2};
/* enumeration for the parts of the first memory word (the operation memory word) - will be used as masks for bit-wise operations */
enum memoryWordParts {ERA = 1, DESOP = 4, SOURCEOP = 16, OPCODE = 64, GROUP = 1024, FIXED = 4096};

/* externs */
extern char * validRegisters[]; /* list of the valid registers  in this program */
extern operation operationList[]; /* list of the valid operations in this program */
extern char eightSpecialBasis[]; /* the 8 special basis in this program */
extern int IC;
extern int DC;
extern symbolNode * SymbolsTable; /* the symbols table, holds the labels declared in input */
extern dataNode * DataTable; /* the data table, holds all data or string declaration in input */
extern memoryWordHolder * MemoryWordsTable; /* the memory words list, holds all memory words created for current input file */
extern entryWordHolder * EntryList; /* the entry symbols list, holds all symbols that been declared as entry in input */

/* function prototypes */
/* process the first read of the input assembly file. returns the number of errors found in input file*/
int firstFileRead(FILE *);
/* process the second read of the input assembly file. in no errors have been found in first and second read, it will create the output files */
void secondRead(FILE *, char *);

#endif

/*
 ============================================================================
 Name of file: memoryLists.h
 Author      : Sapir Lipelis & Fadi Khoury
 Description : This header contains prototypes for functions responsible for creating, 
		editing and removing the memory lists in the program.
 ============================================================================
 */

#ifndef MEMORYLISTS_H
#define MEMORYLISTS_H

/* responsible to free all memory allocated for each input file, before moving to next input file */
void freeAllMemory(void);
/* responsible for inserting the IC value to the data symbols in second read, after IC counter value is known */
void fixDataSymbolAddress(int programStart);
/* responsible for inserting the IC value to the data list items in second read, after IC counter value is known */
void fixDataNodesAddress(int programStart);
/* responsible for creating memory word for numeric operand. memory word will hold the number value in binary */
memoryWordHolder * createMemoryWordForNumber(int );
/* responsible for creating memory word for dynamic operand */
memoryWordHolder * createMemoryWordForDynamic(int * , int , int );
/* responsible for creating memory word for symbol operand, act base on symbol type (local or extern) */
memoryWordHolder * createMemoryWordForSymbol(symbolNode * );
/* responsible for creating memory word for register. memory word will hold the register number in binary */
memoryWordHolder * createMemoryWordForRegister(char *  , char * );
/* responsible to add the received memory word to the memory words list, insert the word based on IC value (at end or between two words) */
int addMemoryWord(memoryWordHolder * );
/* responsible for creating for each data node a memory word and insert it to the end of the memory words list (connect operations part with data part) */
void connectDataToMemoryList();
/* responsible for finding the received symbol name in the symbols table. indicating an error in case the symbol isn't found */
int findSymbol(char * , symbolNode ** , int );
/* responsible for adding new symbol node to the symbol table, indicating an error in case the symbol is already exist */
int addSymbolToTable(symbolNode * , int );
/* responsible for creating new symbol node with received fields values */
symbolNode * createNewSymbol(char * , int , int , int, int);
/* responsible for adding new data node to the data table */
void addDataToTable(dataNode *);
/* responsible for creating new data node with received fields values */
dataNode * createNewData(int , int );
/* responsible for creating the new memory word node from received memory word integer value and add it to the memory words list */
void addFirstMemoryWord(int, int);
/* responsible for modifing the memory word integer based on value received and memory word part received (will be used as mask for bit-wise operations */
int buildOpFirstMemoryWord(int , int, int);

#endif

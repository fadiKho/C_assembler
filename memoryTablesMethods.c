/*
 * memoryTablesMethods.c
 *
 *  Created on: Aug 5, 2016
 *  Author: Sapir Lipelis and Fadi Khoury
 */

#include "assembler.h"
#include "strings.h"

void freeSymbolTable(void)
{
	symbolNode * current;
	referenceNode * currentRef;
	while(SymbolsTable != NULL) /* if table is empty - nothing is executed */
	{
		current = SymbolsTable;

		/* free references */
		while(current->references != NULL)
		{
			currentRef = current->references;
			current->references = current->references->next;
			free(currentRef);
		}

		SymbolsTable = SymbolsTable->next;
		free(current);
	}
}

void freeDataTable(void)
{
	dataNode * current;
	while(DataTable != NULL)
	{
		current = DataTable;
		DataTable = DataTable->next;
		free(current);
	}
}

void freeMemoryWordsList(void)
{
	memoryWordHolder * current;
	while(MemoryWordsTable != NULL)
	{
		current = MemoryWordsTable;
		MemoryWordsTable = MemoryWordsTable->next;
		free(current);
	}
}

void freeEntryList(void)
{
	entryWordHolder * current;
	while(EntryList != NULL)
	{
		current = EntryList;
		EntryList = EntryList->next;
		free(current);
	}
}

void freeAllMemory(void)
{
	/* free symbol list */
	freeSymbolTable();

	/* free data list */
	freeDataTable();

	/* free memoryWords list */
	freeMemoryWordsList();

	/* free entry list */
	freeEntryList();
}

void fixDataSymbolAddress(int programStart)
{
	symbolNode * symbolTablePointer = SymbolsTable;

	while (symbolTablePointer != NULL)
	{

		if (symbolTablePointer->isExtern == 0 && symbolTablePointer->isAttachedToOperation == 0)
		{
			
			int i; /* index */
			int mask = 4;
			int memoryWordValue;
			symbolTablePointer->address = symbolTablePointer->address + IC;
			memoryWordValue = symbolTablePointer->address;
			memoryWordValue<<=2;
			for (i = 2; i < MEMORY_WORD_SIZE; i++)
			{
				if((mask & memoryWordValue) == 0)
				{
					symbolTablePointer->memoryWord[i] = 0;
				}
				else
				{
					symbolTablePointer->memoryWord[i] = 1;
				}
				mask<<=1;
			}
		}
		symbolTablePointer = symbolTablePointer->next;
	}
}

void fixDataNodesAddress(int programStart)
{
	dataNode * currentData = DataTable;

	while(currentData != NULL)
	{
		currentData->address = currentData->address + IC;
		currentData = currentData->next;
	}
}

memoryWordHolder * createMemoryWordForNumber(int value)
{
	int i; /* index */
	int mask = 1;
	memoryWordHolder * newWord = (memoryWordHolder *)malloc(sizeof(memoryWordHolder));
	if(newWord == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}
	newWord->counterIC = IC + 1;
	newWord->next = NULL;
	value<<=2;

	for(i = 0; i < MEMORY_WORD_SIZE; i++)
	{
		if(i == 0 || i == 1) /* insert type */
		{
			if((mask & ABSOLUTE) == 0)
			{
				newWord->memoryWord[i] = 0;
			}
			else
				newWord->memoryWord[i] = 1;
		}
		if((mask & value) == 0)
		{
			newWord->memoryWord[i] = 0;
		}
		else
			newWord->memoryWord[i] = 1;
		mask<<=1;
	}

	return newWord;
}

memoryWordHolder * createMemoryWordForDynamic(int * memoryWord, int startBit, int endBit)
{
	int i, j; /* index */
	int mask = 1;
	memoryWordHolder * newWord = (memoryWordHolder *)malloc(sizeof(memoryWordHolder));
	if(newWord == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}
	newWord->counterIC = IC + 1;
	newWord->next = NULL;

	for(i = 0; i < 2; i++) /* insert type */
	{
		if((mask & ABSOLUTE) == 0)
		{
			newWord->memoryWord[i] = 0;
		}
		else
			newWord->memoryWord[i] = 1;
		mask<<=1;
	}

	for(i = 2, j = startBit; i < MEMORY_WORD_SIZE; i++)
	{
		if(j >= endBit)
		{
			newWord->memoryWord[i] = (memoryWord[endBit]);
		}
		else
		{
			newWord->memoryWord[i] = (memoryWord[j]);
			j++;
		}
	}
	return newWord;
}

memoryWordHolder * createMemoryWordForSymbol(symbolNode * symbol)
{
	int i; /* index */
	memoryWordHolder * newWord = (memoryWordHolder *)malloc(sizeof(memoryWordHolder));
	if(newWord == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}
	newWord->counterIC = IC + 1;
	newWord->next = NULL;

	if(symbol != NULL) /* symbol exist */
	{
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			newWord->memoryWord[i] = symbol->memoryWord[i];
		}

		/* write newWord IC into the symbol references fro extern file */
		if(symbol->isExtern == 1)
		{
			referenceNode * newRef = (referenceNode *)malloc(sizeof(referenceNode));
			referenceNode * current = symbol->references;
			if(newRef == NULL)
			{
				fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
				exit(1);
			}
			newRef->IC = newWord->counterIC;
			newRef->next = NULL;
			if(current == NULL) /* empty references list */
			{
				symbol->references = newRef;
			}
			else
			{
				while(current->next != NULL)
				{
					current = current->next;
				}
				current->next = newRef;
			}
		}

	}
	else /* symbol does not exist (error is notified and counted before, value isn't matter */
	{
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			newWord->memoryWord[i] = 0;
		}
	}

	return newWord;
}

memoryWordHolder * createMemoryWordForRegister(char * sourceRegister , char * destinationRegister)
{
	int startBit = 0, endBit = 0;
	int registerNumber = 0;
	int registerNumber2 = 0;
	int mask = 1;
	int i; /* index */
	memoryWordHolder * newWord = (memoryWordHolder *)malloc(sizeof(memoryWordHolder));
	if(newWord == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}

	newWord->counterIC = IC + 1;
	newWord->next = NULL;

	if(sourceRegister == NULL) /* insert only destination to bits 2 - 7 */
	{
		startBit = 2;
		endBit = 7;
		registerNumber = atoi(destinationRegister + 1);
		registerNumber<<=startBit;
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			if(i == 0 || i == 1)
			{
				if((mask & ABSOLUTE) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else if(i >= startBit && i <= endBit) /* in range, insert register value */
			{
				if((mask & registerNumber) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else
				newWord->memoryWord[i] = 0;
			mask<<=1;
		}

	}
	else if(destinationRegister == NULL) /* insert only source to bits 8 - 13 */
	{
		startBit = 8;
		endBit = 13;
		registerNumber = atoi(sourceRegister + 1);
		registerNumber<<=startBit;
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			if(i == 0 || i == 1)
			{
				if((mask & ABSOLUTE) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else if(i >= startBit && i <= endBit) /* in range, insert register value */
			{
				if((mask & registerNumber) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else
				newWord->memoryWord[i] = 0;
			mask<<=1;
		}
	}
	else /* both registers, insert values to one word */
	{
		registerNumber = atoi(sourceRegister + 1);
		registerNumber2 = atoi(destinationRegister + 1);
		registerNumber<<=8;
		registerNumber2<<=2;
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			if(i == 0 || i == 1)
			{
				if((mask & ABSOLUTE) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else if(i >= 2 && i <= 7) /* in range, insert register2 value */
			{
				if((mask & registerNumber2) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else if(i >= 8 && i <= 13) /* in range, insert register1 value */
			{
				if((mask & registerNumber) == 0)
				{
					newWord->memoryWord[i] = 0;
				}
				else
					newWord->memoryWord[i] = 1;
			}
			else
				newWord->memoryWord[i] = 0;
			mask<<=1;
		}
	}
	return newWord;
}

void connectDataToMemoryList()
{
	dataNode * currentData = DataTable;
	memoryWordHolder * currentWord = MemoryWordsTable;
	memoryWordHolder * newWord;
	int i; /* index */

	if(currentData == NULL) /* empty list, nothing to connect */
	{
		return;
	}

	/* get to last element in memory words table */
	if(currentWord != NULL) /* if is NULL , empty memory words table (meaning no commands in program) */
	{
		while(currentWord->next != NULL)
		{
			currentWord = currentWord->next;
		}
	}

	while(currentData != NULL) /* create memory word for data and add it to end of memory words table */
	{
		/* create memory word */
		newWord = (memoryWordHolder *)malloc(sizeof(memoryWordHolder));
		newWord->counterIC = currentData->address;
		newWord->next = NULL;
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			newWord->memoryWord[i] = currentData->BinaryCode[i];
		}

		/* add memory word to last element */
		if(currentWord == NULL) /* empty List */
		{
			MemoryWordsTable = newWord;
			currentWord = newWord;
		}
		else /* currentWord pointing to last element */
		{
			currentWord->next = newWord;
			currentWord = currentWord->next;
		}

		currentData = currentData->next; /* move to next data node */
	}
}

int addMemoryWord(memoryWordHolder * newWord)
{
	int errorCount = 0;
	memoryWordHolder * current = MemoryWordsTable;
	if(current == NULL) /* empty memory words list, cannot add operands memory word without first memory word for command */
	{
		return 1;
	}

	while(current != NULL && current->counterIC != IC) /* get to memory word before the one we are adding */
	{
		current = current->next;
	}
	/* got to desired point */
	if(current->next == NULL) /* last in list */
	{
		current->next = newWord;
	}
	else /* insert in middle */
	{
		newWord->next = current->next;
		current->next = newWord;
	}

	IC++; /* new memory word is added */

	return errorCount;
}

int findSymbol(char * symbolName, symbolNode ** symbolOperand, int lineCounter)
{
	int errorCount = 0;
	char * end;
	symbolNode * current = SymbolsTable;

	/* skip white spaces at start */
	symbolName = skipWhiteSpaces(symbolName);
	/* skip white spaces at end */
	end = symbolName + strlen(symbolName) - 1;
	while(end > symbolName && isspace(*end)) end--;
	/* Write new null terminator */
	*(end+1) = '\0';

	while(current != NULL && strcmp(current->name, symbolName) != 0)
	{
		current = current->next;
	}

	if(current == NULL)
	{
		fprintf(stderr, "ERROR: line %d: symbol %s does not exist\n", lineCounter, symbolName);
		errorCount++;
		*symbolOperand = NULL;
		return errorCount;
	}
	else /* get this symbol */
	{
		*symbolOperand = current;
	}

	return errorCount;
}

int addSymbolToTable(symbolNode * newSymbol, int lineCount)
{
	symbolNode * tablePointer = SymbolsTable;
	if(tablePointer == NULL) /* empty list */
	{
		SymbolsTable = newSymbol;
	}
	else /* add to the end of the symbols table */
	{
		while((tablePointer->next != NULL) && (strcmp(tablePointer->name, newSymbol->name) != 0)) /* get to the last node in the list, check if this symbol is already defined */
		{
			tablePointer = tablePointer->next;
		}
		if(strcmp(tablePointer->name, newSymbol->name) == 0) /* double definition of symbol -> ERROR */
		{
			if(tablePointer->isExtern == 1 && newSymbol->isExtern == 1) /* its ok to double declare externs */
			{
				free((void *)newSymbol); /* this symbol already defined, so it's not inserted to the symbols table */
				return 0;
			}
			fprintf(stderr, "ERROR: line %d: symbol %s already declared in program\n", lineCount, newSymbol->name);
			free((void *)newSymbol); /* this symbol already defined, so it's not inserted to the symbols table */
			return 1;
		}

		tablePointer->next = newSymbol;
	}

	return 0;

}

symbolNode * createNewSymbol(char * name, int address, int isExtern, int isAttachedToOperation, int memoryWord)
{
	symbolNode * new = (symbolNode *)malloc(sizeof(symbolNode));
	int mask = 1;
	int i; /* index */
	if(new == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}
	else
	{
		new->address = address;
		new->isAttachedToOperation = isAttachedToOperation;
		new->isExtern = isExtern;
		strncpy(new->name, name, MAX_LABEL);
		new->next = NULL;
		new->references = NULL;
		memoryWord<<=2; /* value get inserted from 3rd bit to 15th bit */
		/* add memory word, 15 bytes only */
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			if(i == 0 || i == 1) /* insert EXTERNAL or RELOCATABLE */
			{
				if(isExtern)
				{
					if((mask & EXTERNAL) == 0)
					{
						new->memoryWord[i] = 0;
					}
					else
					{
						new->memoryWord[i] = 1;
					}
				}
				else /* relocatable */
				{
					if((mask & RELOCATABLE) == 0)
					{
						new->memoryWord[i] = 0;
					}
					else
					{
						new->memoryWord[i] = 1;
					}
				}
			}
			else
			{
				if((mask & memoryWord) == 0)
				{
					new->memoryWord[i] = 0;
				}
				else
				{
					new->memoryWord[i] = 1;
				}
			}
			mask<<=1;
		}
	}
	return new;

}

void addFirstMemoryWord(int counterIC, int memoryWord)
{
	int i; /* index */
	int mask = 1;
	memoryWordHolder * tablePointer = MemoryWordsTable;
	memoryWordHolder * newMemoryWord = (memoryWordHolder *)malloc(sizeof(memoryWordHolder));
	if(newMemoryWord == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}

	/* create memory word */
	newMemoryWord->counterIC = counterIC;
	/* create memory word */
	for(i = 0; i < MEMORY_WORD_SIZE; i++)
	{
		if((mask & memoryWord) == 0)
		{
			newMemoryWord->memoryWord[i] = 0;
		}
		else
		{
			newMemoryWord->memoryWord[i] = 1;
		}
		mask<<=1;
	}
	newMemoryWord->next = NULL;

	if(tablePointer == NULL) /* empty list */
	{
		MemoryWordsTable = newMemoryWord;
	}
	else
	{
		while(tablePointer->next != NULL)
		{
			tablePointer = tablePointer->next;
		}
		tablePointer->next = newMemoryWord;;
	}
}

void addDataToTable(dataNode * newData)
{
	dataNode * tablePointer = DataTable;
	if(tablePointer == NULL) /* empty list */
	{
		DataTable = newData;
	}
	else
	{
		while(tablePointer->next != NULL)
		{
			tablePointer = tablePointer->next;
		}
		tablePointer->next = newData;
	}
}

dataNode * createNewData(int value, int address)
{
	dataNode * new = (dataNode *)malloc(sizeof(dataNode));
	int i;
	int mask = 1;
	if(new == NULL)
	{
		fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
		exit(1);
	}
	else
	{
		new->address = address;
		new->next = NULL;
		/* add value in binary code , 15 bytes only */
		for(i = 0; i < MEMORY_WORD_SIZE; i++)
		{
			if((mask & value) == 0)
			{
				new->BinaryCode[i] = 0;
			}
			else
			{
				new->BinaryCode[i] = 1;
			}
			mask<<=1;
		}
	}
	return new;
}

int buildOpFirstMemoryWord(int value, int memoryWordPart, int memoryWord)
{
	int i; /* will represent number of bytes in the part of memory word being modified */
	int mask = memoryWordPart;
	if(memoryWordPart == ERA || memoryWordPart == DESOP || memoryWordPart == SOURCEOP || memoryWordPart == GROUP)
	{
		i = 2; /* two bytes in those parts */
		/* move value to match the part of the memory word */
		if(memoryWordPart == DESOP)
		{
			value<<=2;
		}
		else if(memoryWordPart == SOURCEOP)
		{
			value<<=4;
		}
		else if(memoryWordPart == GROUP)
		{
			value<<=10;
		}
	}
	else if(memoryWordPart == OPCODE)
	{
		i = 4;
		value<<=6;
	}
	else /* 3 last bytes that are always fixed to 101 */
	{
		i = 3;
		value<<=12;
	}

	/* modify the memory word part with value */
	while(i)
	{
		memoryWord |= (mask & value);
		mask<<=1;
		i--;
	}

	return memoryWord;
}

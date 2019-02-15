/*
 * secondRead.c
 *
 *  Created on: Aug 1, 2016
 *  Author: Sapir Lipelis and Fadi Khoury
 */
#include "assembler.h"
#include "validation.h"
#include "memoryLists.h"
#include "strings.h"

void createFiles(char *);
void createEntFile(char *);
void createExtFile(char *);
void createObjFile(char *);
int getOperands(char * , int ,  int );

/* Globals */
entryWordHolder * EntryList = NULL;
char eightSpecialBasis[8] = {'!','@','#','$','%','^','&','*'};

void secondRead(FILE * input, char * inputName)
{
	int errorCount = 0;
	int PROGRAM_START = 100;
	int lineCounter = 0;
	char currentLine[MAX_LENGTH] = {'\0'};
	int i; /* index */
	int numberOfOperands;

	fixDataSymbolAddress(PROGRAM_START);
	fixDataNodesAddress(PROGRAM_START);
	connectDataToMemoryList();

	IC = 100;
	rewind(input);
	while(fgets(currentLine, MAX_LENGTH - 1, input) != NULL)
	{
		lineCounter++;
		/* skip empty line or comment line */
		if(isEmpty(currentLine) || isComment(currentLine))
		{
			continue;
		}
		/* skip .extern, .string and .data lines */
		if(strstr(currentLine, ".extern") != NULL || strstr(currentLine, ".string") != NULL || strstr(currentLine, ".data") != NULL)
		{
			continue;
		}

		/* if line is .entry - check if symbol is exist and if this symbol is not already declared as entry */
		if(strstr(currentLine, ".entry") != NULL)
		{
			strcpy(currentLine, strstr(currentLine, ".entry") + strlen(".entry"));
			errorCount += checkEntrySymbol(strtok(currentLine, "\n"), lineCounter);
			continue;
		}

		/* if reached here - line can be only command */
		/* find the command */
		for(i = 0; strcmp(operationList[i].opName, "NULL") != 0; i++)
		{
			if(strstr(currentLine, operationList[i].opName) != NULL) /* legal operation found */
			{
				break;
			}
		}
		if(strcmp(operationList[i].opName, "NULL") == 0) /* illegal operation */
		{
			errorCount++; /* already notified in first read on illegal command, just skip to next line */
			IC++;
			continue;
		}

		/* get rest of line, after command to end */
		strcpy(currentLine, strstr(currentLine, operationList[i].opName) + strlen(operationList[i].opName));
		strcpy(currentLine, skipWhiteSpaces(currentLine));
		if(operationList[i].numberOfParameters == 0) /* skip to next line, memory word already exist and syntax is checked */
		{
			IC++;
			continue;
		}
		else
			numberOfOperands = operationList[i].numberOfParameters;

		errorCount += getOperands(strtok(currentLine, "\n"), numberOfOperands, lineCounter);
		IC++;
	}

	if(errorCount == 0)
	{

		createFiles(inputName);
	}

	/* reset for next assembly */
	freeAllMemory();
	IC = 100;
	DC = 0;
}

void createFiles(char * fileName)
{
	createEntFile(fileName);
	createExtFile(fileName);
	createObjFile(fileName);
}

void createEntFile(char * fileName)
{
	if(EntryList != NULL) /* if entry list is empty - don't create file */
	{
		FILE * file;
		entryWordHolder * current = EntryList;
		char name[MAX_LENGTH];
		char entryAddress[MEMORY_WORD_SIZE];
		strcpy(name, fileName);
		strcat(name, ".ent");
		file = fopen(name, "w");

		if(file == NULL) /* fail to open file for some reason */
		{
			fprintf(stderr , "ERROR: failed to generate Entry file.\n");
			return;
		}

		while(current != NULL)
		{
			getSpecialBasisIC(current->address, entryAddress);
			fprintf(file, "%s\t%s\n", current->label, entryAddress);
			current = current->next;
		}

		/* finish printing */
		fclose(file);
	}
}

void createExtFile(char * fileName)
{
	symbolNode * currentSymbol = SymbolsTable;
	FILE * file= NULL;
	char name[MAX_LENGTH];
	char externIC[MEMORY_WORD_SIZE];
	strcpy(name, fileName);
	strcat(name, ".ext");

	while(currentSymbol != NULL)
	{
		if(currentSymbol->isExtern == 1 && currentSymbol->references != NULL) /* the symbol is extern and have been used in the input file */
		{
			referenceNode * currentRef = currentSymbol->references;
			if(file == NULL) /* file didn't opened yet */
			{
				file = fopen(name, "w");
				if(file == NULL) /* fail to open file for some reason */
				{
					fprintf(stderr , "ERROR: failed to generate Entry file.\n");
					return;
				}
			}

			while(currentRef != NULL)
			{
				getSpecialBasisIC(currentRef->IC , externIC);
				fprintf(file, "%s\t%s\n", currentSymbol->name, externIC);
				currentRef = currentRef->next;
			}
		}
		currentSymbol = currentSymbol->next;
	}

	/* finish - if file is opened -> close */
	if(file != NULL)
	{
		fclose(file);
	}
}

void createObjFile(char * fileName)
{
	FILE * file = NULL;
	memoryWordHolder * currentMemoryWord = MemoryWordsTable;
	char specialBasismemoryWord[MEM_S_BASIS];
	char specialBasisAddress[MEMORY_WORD_SIZE];
	char name[MAX_LENGTH];

	strcpy(name, fileName);
	strcat(name, ".ob");

	file = fopen(name, "w");

	if(file == NULL) /* fail to open file for some reason */
	{
		fprintf(stderr , "ERROR: failed to generate Entry file.\n");
		return;
	}

	/* print number of IC and DC in first line */
	getSpecialBasisIC(IC - 100 , specialBasisAddress);
	fprintf(file, "%s\t", specialBasisAddress);
	getSpecialBasisIC(DC , specialBasisAddress);
	fprintf(file, "%s\n", specialBasisAddress);

	/* print memory words from memory words table */
	while(currentMemoryWord != NULL)
	{
		getSpecialBasisIC(currentMemoryWord->counterIC, specialBasisAddress);
		getSpecialBasisMemoryWord(currentMemoryWord->memoryWord, specialBasismemoryWord);
		fprintf(file, "%s\t%s\n", specialBasisAddress , specialBasismemoryWord);
		currentMemoryWord = currentMemoryWord->next;
	}

	/* finish printing */
	fclose(file);

}

int getOperands(char * operandToken, int numberOfOperands,  int lineCounter)
{
	/* no need to check if the type of operand is legal for the command - this is already checked in first read */
	/* no need to check for legality of label - this is already checked in first read */
	char copyToken[MAX_LENGTH] = {'\0'};
	char firstOperand[MAX_LABEL] = {'\0'};
	char secondOperand[MAX_LABEL] = {'\0'};
	int errorCount = 0;

	if(isEmpty(operandToken)) /* illegal empty operand - already notified in first read */
	{
		IC++;
		return 1;
	}

	strcpy(copyToken, operandToken);
	copyToken[strlen(copyToken)] = '\n';

	if(numberOfOperands == 1)
	{
		strcpy(firstOperand, strtok(copyToken, "\n"));
	}
	if(numberOfOperands == 2)
	{
		if(isEmpty(strstr(copyToken, ","))) /* missing operand */
		{
			errorCount++;
			strcpy(secondOperand, ""); /* empty operand */
		}
		else
		{
			strcpy(secondOperand, strstr(copyToken, ",") + 1);
			strcpy(secondOperand, skipWhiteSpaces(secondOperand)); /* remove white spaces from second operand */
		}
		strcpy(firstOperand, strtok(copyToken, ",")); /* get first operand token */
	}

	while(numberOfOperands > 0)
	{
		char * currentOperand;
		if(numberOfOperands == 2)
		{
			currentOperand = secondOperand;
		}
		if(numberOfOperands == 1)
		{
			currentOperand = firstOperand;
		}

		/* find operand type*/
		if(isEmpty(currentOperand)) /* error is notified in first read */
		{
			errorCount++;
			addMemoryWord(createMemoryWordForNumber(0)); /* create empty memory word and continue. value doesn't matter - no files will be created */
		}
		else if(strchr(currentOperand, '#') != NULL) /* Immediate */
		{
			int value = 0;
			char * ptr;
			strcpy(currentOperand, currentOperand + 1);
			/* analyze the numeric parameter */
			value = (int)(strtol(currentOperand, &ptr, 10));
			if((strlen(ptr) != 0) && (!isEmpty(ptr))) /* illegal integer */
			{
				errorCount++;
				value = '0';
			}
			if((strlen(ptr) != 0) && value == 0) /* missing number */
			{
				errorCount++;
				value = '0';
			}
			if(value > MAX_13_NUM || value < MIN_13_NUM)
			{
				errorCount++;
				fprintf(stderr, "ERROR: line %d: immediate operand cannot fit 13 bits representation\n", lineCounter);
				value = '0';
			}

			addMemoryWord(createMemoryWordForNumber(value));
		}
		else if(strchr(currentOperand, '[') != NULL) /*Dynamic */
		{
			int startBit = 0;
			int endBit = 0;
			char * ptr;
			char symbolName[MAX_LABEL];
			int symbolAddress;
			memoryWordHolder * current = MemoryWordsTable;
			int memoryWord[MEMORY_WORD_SIZE] = {0};
			int * memoryWordPtr = memoryWord;

			strncpy(symbolName, currentOperand, MAX_LABEL);
			strcpy(symbolName, strtok(symbolName, "["));

			/* find symbol in list, also check if its extern */
			errorCount += checkDynamicSymbol(symbolName, &symbolAddress, lineCounter);

			/* find memory word with the symbol address (IC) in MemoryWordsTable */
			if(symbolAddress != 0) /* legal symbol was found */
			{
				while(current != NULL && current->counterIC != symbolAddress)
				{
					current = current->next;
				}
				memoryWordPtr = current->memoryWord;
			}

			/* analyze first bit in range , if illegal number, it is already checked in first read*/
			strcpy(currentOperand, strchr(currentOperand, '[') + 1);
			startBit = (int)(strtol(currentOperand, &ptr, 10));
			if(startBit > MEMORY_WORD_SIZE -1 || startBit < 0)
			{
				errorCount++;
				startBit = 0;
			}
			/* analyze second bit in range */
			if(strchr(currentOperand, '-') == NULL)
			{
				errorCount++;
				endBit = startBit;
			}
			else
			{
				strcpy(currentOperand, strchr(currentOperand, '-') + 1);
				endBit = (int)(strtol(currentOperand, &ptr, 10));
				if(endBit > MEMORY_WORD_SIZE -1 || endBit < 0 || endBit < startBit)
				{
					errorCount++;
					endBit = startBit;
				}
			}

			if(endBit - startBit >= 13)
			{
				errorCount++;
				startBit = 0;
				endBit = 0;
				fprintf(stderr , "ERROR: line %d: dynamic range is too big, need 13 bits at max.\n", lineCounter);
			}

			addMemoryWord(createMemoryWordForDynamic(memoryWordPtr, startBit, endBit));
		}
		else if(findRegister(currentOperand) != NULL) /* Register */
		{
			char * register1 = findRegister(secondOperand);
			char * register2 = findRegister(firstOperand);

			if(register1 != NULL && register2 != NULL) /* both registers */
			{
				numberOfOperands = 0; /* processing both operands in one word */
			}
			addMemoryWord(createMemoryWordForRegister(register1 , register2));
		}
		else /* DIRECT */
		{
			symbolNode * symbolOperand = NULL;
			symbolNode ** symbolOperandPtr;
			symbolOperandPtr = &symbolOperand;
			errorCount += findSymbol(currentOperand, symbolOperandPtr, lineCounter);
			addMemoryWord(createMemoryWordForSymbol(symbolOperand));
		}

		numberOfOperands--;
	}

	return errorCount;
}


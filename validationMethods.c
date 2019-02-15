/*
 * validationMethods.c
 *
 *  Created on: Jul 6, 2016
 *  Author: Sapir Lipelis and Fadi Khoury
 */

#include "assembler.h"
#include "strings.h"

int checkSymbol(char * currentLine, int lineCounter, char * nameHolder) /* returns 1 if there are errors in symbol name */
{
	int errorCount = 0;
	int i; /* index */
	char * end;
	if(isEmpty(currentLine))
	{
		fprintf(stderr, "ERRPR: line %d: illegal symbol - symbol cannot be empty\n", lineCounter);
		strcpy(nameHolder, " ");
		return 1;
	}

	currentLine = skipWhiteSpaces(currentLine); /* skip white spaces at start */

	/* skip white spaces at end */
	  end = currentLine + strlen(currentLine) - 1;
	  while(end > currentLine && isspace(*end)) end--;
	  /* Write new null terminator */
	  *(end+1) = '\0';

	if(strlen(currentLine) > MAX_LABEL - 1) /* check if label is more than 30 characters */
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: illegal symbol - symbol is more than 30 characters\n", lineCounter);
	}

	for(i = 0; strcmp(validRegisters[i], "NULL") != 0; i++) /* check if label have register name */
	{
		if(strcmp(currentLine, validRegisters[i]) == 0)
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: illegal symbol - symbol cannot have register name\n", lineCounter);
			break;
		}
	}

	for(i = 0; strcmp(operationList[i].opName, "NULL") != 0; i++) /* check if label have operation name */
	{
		if(strcmp(currentLine, operationList[i].opName) == 0)
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: illegal symbol - symbol cannot have operation name\n", lineCounter);
			break;
		}
	}

	/* check if label start with alphabetic letter */
	i = 0;
	if(!isalpha(currentLine[i]))
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: illegal symbol - symbol should start with alphabetic letter\n", lineCounter);
	}

	/* check if label have only letters and digits */
	for(i = 1; i < strlen(currentLine); i++)
	{
		if(!isalpha(currentLine[i]) && !isdigit(currentLine[i]))
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: illegal symbol - \"%c\" is not legal character\n", lineCounter, currentLine[i]);
		}
	}

	/* copy max of 30 chars into nameHolder */
	strncpy(nameHolder, currentLine, MAX_LABEL - 1);
	return errorCount;
}

int checkNumber(char * currentToken, int * currentValue, int lineCounter)
{
	char * ptr; /* for strtol function */
	char copyToken[MAX_LENGTH];
	int errorCount = 0;

	strcpy(copyToken, currentToken);

	/* check if currentToken is empty */
	if(isEmpty(copyToken))
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: number is missing (empty).\n", lineCounter);
		(*currentValue) = 0;
		return errorCount;
	}

	/* skip white spaces */
	strcpy(copyToken, skipWhiteSpaces(copyToken));

	(*currentValue) = (int)(strtol(currentToken, &ptr, 10));
	if((strlen(ptr) != 0) && (!isEmpty(ptr))) /* illegal integer */
	{
		errorCount++;
		(*currentValue) = 0;
		fprintf(stderr,"ERROR: line %d: illegal integer: %s\n", lineCounter, currentToken);
	}
	if((strlen(ptr) != 0) && currentValue == 0) /* missing number */
	{
		errorCount++;
		(*currentValue) = 0;
		fprintf(stderr,"ERROR: line %d: missing integer between two commas\n", lineCounter);
	}
	if((*currentValue) > MAX_DATA || (*currentValue) < MIN_DATA)
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: data overflow, number %d binary value is too large\n" , lineCounter, (*currentValue));
		(*currentValue) = 0;
	}

	return errorCount;
}

int checkDynamic(char * currentOperand, int * startBit, int * endBit, int lineCounter)
{
	int errorCount = 0;
	char parameterPart[MAX_LENGTH];
	char symbolInDynamic[MAX_LABEL];

	strcpy(parameterPart, currentOperand);
	/* check symbol before range */
	strtok(parameterPart, "[");
	errorCount += checkSymbol(parameterPart, lineCounter, symbolInDynamic);
	/* check range */
	if(strstr(currentOperand, "-") == NULL) /* illegal range syntax */
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: illegal dynamic operand range.\n", lineCounter);
		(*startBit) = 0;
		(*endBit) = 0;
		return errorCount;
	}
	errorCount += checkNumber(strtok(NULL, "-"), startBit, lineCounter);
	errorCount += checkNumber(strtok(NULL, "]"), endBit, lineCounter);
	if((*startBit) < 0 || (*startBit) > MEMORY_WORD_SIZE -1)
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: first range integer is out of range\n", lineCounter);
	}
	if((*endBit) < 0 || (*endBit) > MEMORY_WORD_SIZE -1 || (*endBit) < (*startBit))
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: second range integer is out of range\n", lineCounter);
	}
	return errorCount;
}

char * findRegister(char * currentToken)
{
	int i;
	char copyToken[MAX_LENGTH] = {'\0'};
	strcpy(copyToken, currentToken);
	strcpy(copyToken, skipWhiteSpaces(copyToken));
	for(i = 0; strcmp(validRegisters[i], "NULL") != 0; i++)
	{
		if(strstr(copyToken, validRegisters[i]) != NULL)
		{
			strcpy(copyToken, copyToken + strlen(validRegisters[i])); /* check if rest of token after the register is empty */
			if(isEmpty(copyToken))
			{
				return validRegisters[i];
			}
		}
	}
	return NULL;
}

int checkOperandsForCommand(int sourceOperand, int destinationOperand, char * operationName, int lineCounter)
{
	int errorCount = 0;

	/* check source operand */
	if(strcmp(operationName, "lea") == 0 ) /* lea command can get only DIRECT (symbol) operand */
	{
		if(sourceOperand != DIRECT)
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: unmatched source operand for command\n", lineCounter);
		}
	}

	/* check destination operand */
	if(strcmp(operationName, "cmp") != 0 && strcmp(operationName, "prn") != 0 && strcmp(operationName, "rts") != 0 && strcmp(operationName, "stop") != 0)
	{
		if(destinationOperand != DIRECT && destinationOperand != REGISTER)
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: unmatched destination operand for command\n", lineCounter);
		}
	}

	return errorCount;
}

int checkOperandType(char * currentToken, int * operand, int lineCounter)
{
	int errorCount = 0;

	if(currentToken == NULL || isEmpty(currentToken)) /* missing operand */
	{
		fprintf(stderr, "ERROR: line %d: missing operator for command\n", lineCounter);
		return 1;
	}

	if(strchr(currentToken, '#') != NULL) /* Immediate */
	{
		int value = 0;
		(*operand) = IMMEDIATE;
		strcpy(currentToken, currentToken + 1);
		/* analyze the numeric parameter */
		errorCount += checkNumber(currentToken, &value, lineCounter);
	}
	else if(strchr(currentToken, '[') != NULL) /*Dynamic */
	{
		int startBit = 0;
		int endBit = 0;
		(*operand) = DYNAMIC;

		if(strchr(currentToken, ']') == NULL)
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: missing \"]\" for range. cannot process range\n", lineCounter);
		}
		else
			errorCount += checkDynamic(currentToken, &startBit, &endBit, lineCounter);

	}
	else if(findRegister(currentToken) != NULL) /* register */
	{
		(*operand) = REGISTER;
	}
	else /* direct */
	{
		char symbol[MAX_LABEL];
		(*operand) = DIRECT;
		errorCount += checkSymbol(currentToken, lineCounter, symbol);
	}
	return errorCount;
}

int checkEntrySymbol(char * symbol , int lineCounter)
{
	int errorCounter = 0;
	char name[MAX_LABEL] = {'\0'};
	char * end;
	symbolNode * current;
	entryWordHolder * currentEntry;
	entryWordHolder * newEntry;
	if(isEmpty(symbol))
	{
		return 1; /* error on entry name is already notified in first read */
	}
	strcpy(name, skipWhiteSpaces(symbol));

	/* skip white spaces at end */
	end = name + strlen(name) - 1;
	while(end > name && isspace(*end)) end--;
	/* Write new null terminator */
	*(end+1) = '\0';

	/* find in symbol table */
	current = SymbolsTable;
	while(current != NULL)
	{
		if(strcmp(name, current->name) == 0) /* found */
		{
			if(current->isExtern == 1) /* extern cannot be also entry */
			{
				fprintf(stderr, "ERROR: line %d: symbol %s is extern. cannot declared as entry\n", lineCounter, name);
				errorCounter++;
				return errorCounter;
			}
			newEntry = (entryWordHolder *)malloc(sizeof(entryWordHolder));
			if(newEntry == NULL)
			{
				fprintf(stderr, "ERROR: Memory Allocation Failed. Ending Program\n");
				exit(1);
			}
			newEntry->address = current->address;
			strcpy(newEntry->label, current->name);
			newEntry->next = NULL;
			break;
		}
		current = current->next;
	}
	if(current == NULL) /* not found */
	{
		fprintf(stderr, "ERROR: line %d: symbol %s does not exist\n", lineCounter, name);
		errorCounter++;
		return errorCounter;
	}

	/* search in entry list */
	currentEntry = EntryList;
	if(currentEntry == NULL) /* empty list */
	{
		EntryList = newEntry;
	}
	else
	{
		while(currentEntry->next != NULL && strcmp(currentEntry->label, newEntry->label) == 0) /* get to last element in entries. if same label found -> ERROR */
		{
			currentEntry = currentEntry->next;
		}

		if(strcmp(currentEntry->label, newEntry->label) == 0)
		{
			fprintf(stderr, "ERROR: line %d: symbol %s already declared as entry\n", lineCounter, name);
			errorCounter++;
			return errorCounter;
		}
		/* insert new entry */
		currentEntry->next = newEntry;
	}

	return errorCounter;
}

int checkDynamicSymbol(char * symbolName, int * symbolAddress, int lineCounter)
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
		(*symbolAddress) = 0;
		return errorCount;
	}

	if(current->isExtern == 1) /* check if extern -> ERROR */
	{
		fprintf(stderr, "ERROR: line %d: symbol %s is extern\n", lineCounter, symbolName);
		errorCount++;
		(*symbolAddress) = 0;
		return errorCount;
	}

	(*symbolAddress) = current->address;

	return errorCount;
}

/*
 ============================================================================
 Name of file: firstRead.C
 Author      : Sapir Lipelis & Fadi Khoury
 Description : The firstRead file responsible for the first read of the input file,
		build the symbols table, data table and first memory words (operations memory words).
		Also responsible for indicating syntax errors, illegal operations and operands.
 ============================================================================
 */

#include "assembler.h"
#include "memoryLists.h"
#include "strings.h"
#include "validation.h"

/* Globals */
/* array of valid operations */
operation operationList[] = {
		{"mov" , 0, 2},
		{"cmp" , 1, 2},
		{"add" , 2, 2},
		{"sub" , 3, 2},
		{"not" , 4, 1},
		{"clr" , 5, 1},
		{"lea" , 6, 2},
		{"inc" , 7, 1},
		{"dec" , 8, 1},
		{"jmp" , 9, 1},
		{"bne" , 10, 1},
		{"red" , 11, 1},
		{"prn" , 12, 1},
		{"jsr" , 13, 1},
		{"rts" , 14, 0},
		{"stop" , 15, 0},
		{"NULL", 0, 0}
};

/* array of valid registers */
char * validRegisters[] = { "r0" , "r1" , "r2" , "r3" , "r4" , "r5" , "r6" , "r7", "NULL" };

symbolNode * SymbolsTable = NULL; /* the symbols table for labels as a linked list */
dataNode * DataTable = NULL; /* the data table for .data and .string as a linked list */
memoryWordHolder * MemoryWordsTable = NULL; /* the memory words list */
int IC = 100;
int DC = 0;

/*prototypes */
/* responsible for getting the data (numbers or string) from .string and .data input lines. returns number of erros found */
int getData(char * , int * , int, int, int *);
/* responsible for getting the operands from operation input lines. returns number of errors found */
int getParameters(char *, char *, int *, int, int *, int);

/* performs the first read of the input file.
 * The function read the input line by line, analyzing it's type and build the memory lists accordingly.
 * The function analyzing the data and operation lines with thr getData and getParameters functions. 
 * The function counts the number of errors found, and indicate those errors to strerr, with description and line number.
 */
int firstFileRead(FILE * input)
{
	char currentLine[MAX_LENGTH] = {'\0'}; /* current input line */
	char copyLine[MAX_LENGTH] = {'\0'}; /* copy current input line - for strtok actions */
	int errorCount = 0;
	int lineCounter = 0;
	int symbolFlag = 0; /* indicates whether the current line have symbol declaration (symbol: ... ) */
	int operationFlag; /* indicates whether the current line is operation line */
	int dataFlag; /* indicates whether the current line is .data line */
	int stringFlag; /* indicates whether the current line is .string line */

	/* variables for optional symbol in this line */
	char name [MAX_LABEL];
	int address;
	int isExtern;
	int isAttachedToOperation;
	int memoryWord;

	/* read line by line */
	while(fgets(currentLine, MAX_LENGTH - 1, input) != NULL)
	{
		operationFlag = 0;
		dataFlag = 0;
		stringFlag = 0;
		memoryWord = 0;
		lineCounter++;

		/* get copy of current line for strtok operations */
		strcpy(copyLine, currentLine);

		/* check if line is empty or comment */
		if(isEmpty(currentLine) || isComment(currentLine))
		{
			continue;
		}

		/* check for symbol */
		if(strcmp(currentLine, strtok(copyLine, ":")) == 0) /* no ":", meaning no symbol in this line */
		{
			symbolFlag = 0;
			strcpy(currentLine, skipWhiteSpaces(currentLine));
		}
		else /* symbol or part of .string line */
		{
			if(strstr(copyLine, ".string") != NULL) /* the ":" is part of .string line, not symbol */
			{
				symbolFlag = 0;
			}
			else
			{
				char * symbolHolder = copyLine;
				char restOflineHolder[MAX_LENGTH];
				strcpy(restOflineHolder , strtok(NULL, "\n"));
				if(isspace(copyLine[0])) /* symbol do not declared at first column */
				{
					symbolFlag = 0;
					fprintf(stderr, "Warning: line %d: symbol is declared out of first column. The symbol is ignored.\n", lineCounter);
				}
				else
				{
					symbolFlag = 1;
					errorCount += checkSymbol(symbolHolder, lineCounter, name); /* check if symbol is legal */
					isExtern = 0; /* this symbol is not extern */
				}
				
				/* check if rest of line is empty */
				if(isEmpty(restOflineHolder))
				{
					errorCount++;
					fprintf(stderr, "ERROR: line %d: illegal declaration of symbol, missing data or operation line\n", lineCounter);
					continue; /* move to next line */
				}
				strcpy(currentLine, restOflineHolder);
				/* find unexpected ":" characters after symbol, print error and clear from rest of line */
				strcpy(currentLine, skipWhiteSpaces(currentLine));
				if(currentLine[0] == ':')
				{
					while(currentLine[0] == ':')
					{
						fprintf(stderr, "ERROR: line %d: unexpected \":\"\n", lineCounter);
						errorCount++;
						strcpy(currentLine, currentLine + 1);
						strcpy(currentLine, skipWhiteSpaces(currentLine));
					}
				}
			}
		}

		/* get copy of current line for strtok operations*/
		strcpy(copyLine , currentLine);

		/* check if line is entry */
		if(strcmp(".entry", strtok(copyLine, " \t\n")) == 0) /* this line is entry */
		{
			symbolNode * current = SymbolsTable;
			char name[MAX_LABEL];

			if(symbolFlag) /* label before .entry declaration */
			{
				fprintf(stderr, "WARNING: line %d: symbol before .entry is meaningless\n", lineCounter);
			}

			if(isEmpty(currentLine + strlen(copyLine))) /* missing label */
			{
				errorCount++;
				fprintf(stderr, "ERROR: line %d: missing lable for .entry line\n", lineCounter);
				continue;
			}

			strcpy(name , strtok(NULL, " \t\n"));
			strcpy(name, skipWhiteSpaces(name));
			errorCount += checkSymbol(name, lineCounter, name);
			
			/* check if symbol is already declared as extern in symbol table */
			while(current != NULL)
			{
				if(strcmp(current->name, name) == 0 && current->isExtern == 1)
				{
					errorCount++;
					fprintf(stderr, "ERROR: line %d: symbol %s is already declared as extern\n", lineCounter, current->name);
					break;
				}
				current = current->next;
			}
			
			if(!isEmpty(strtok(NULL, "\n"))) /* check the rest of the line, after the symbol */
			{
				errorCount++;
				fprintf(stderr, "ERROR: illegal entry declaration in line %d\n", lineCounter);
			}
			continue; /* move to next line */
		}
		/* check if line is extern */
		if(strcmp(".extern", copyLine) == 0) /* this line is extern */
		{
			if(symbolFlag) /* label before .extern declaration */
			{
				fprintf(stderr, "WARNING: line %d: symbol before .extern is meaningless\n", lineCounter);
			}

			if(isEmpty(currentLine + strlen(copyLine))) /* missing label */
			{
				errorCount++;
				fprintf(stderr, "ERROR: line %d: missing lable for .extern line\n", lineCounter);
				continue;
			}

			errorCount += checkSymbol(strtok(NULL, " \t\n"), lineCounter, name);
			/* prepare symbol details for adding to the symbols table */
			address = 0; /* fictitious address for extern symbols */
			isExtern = 1;
			isAttachedToOperation = 0;
			memoryWord = 0;
			/* create symbol and add to symbols table */
			errorCount += addSymbolToTable(createNewSymbol(name, address, isExtern, isAttachedToOperation, memoryWord), lineCounter);

			if(!isEmpty(strtok(NULL, "\n"))) /* check the rest of the line, after the symbol */
			{
				errorCount++;
				fprintf(stderr, "ERROR: line %d: illegal extern declaration\n", lineCounter);
			}

			continue; /* move to next line */
		}

		/* decide if this line contains operation or data/string */
		if(strcmp(".data", copyLine) == 0)
		{
			dataFlag = 1;
		}
		else if((strcmp(".string", copyLine) == 0))
		{
			stringFlag = 1;
		}
		else /* must be operation */
		{
			operationFlag = 1;
		}

		if(dataFlag || stringFlag) /* process for .data or .string line */
		{
			int data[MAX_LENGTH] = {0}; /* array of values of .data line. max length is 80, same as line length */
			int dataCounter = 0;
			int i = 0;
			errorCount += getData(strtok(NULL, "\n"), data, lineCounter, dataFlag, &dataCounter); /* send rest of line after ".data" to getData */

			/* add each value extracted from the line to the data table */
			while(i < dataCounter)
			{
				if(i == 0 && symbolFlag) /* first DC value need to be insert also to memoryWord of symbol, then symbol is added to table */
				{
					address = DC;
					memoryWord = 0; /* real value will be inserted in the second read, after merging DC with IC */
					isAttachedToOperation = 0;
					errorCount += addSymbolToTable(createNewSymbol(name, address, isExtern, isAttachedToOperation, memoryWord), lineCounter);
				}

				/* insert value to the data table */
				addDataToTable(createNewData(data[i], DC));

				/* move to next address in data table */
				DC++;
				i++; /* move to next value in data line */
			}

			if(stringFlag) /* insert after values a \0 to indicate end of string in data table */
			{
				addDataToTable(createNewData(0, DC));
				DC++;
			}
		}

		if(operationFlag) /* process for operation line , at this point, currentLine holds the whole operation line (without the symbol). copyLine holds the operation name */
		{
			int i; /* index */
			int operandsMemoryWords = 0; /* number of memory words needed for the operands */

			if(isspace(copyLine[strlen(copyLine) - 1])) /* remove white space at end of oeration name */
			{
				copyLine[strlen(copyLine) - 1] = '\0';
			}

			/* find the command */
			for(i = 0; strcmp(operationList[i].opName, "NULL") != 0; i++)
			{
				if(strcmp(copyLine, operationList[i].opName) == 0) /* legal operation found */
				{
					break;
				}
			}
			if(strcmp(operationList[i].opName, "NULL") == 0) /* illegal operation */
			{
				errorCount++;
				fprintf(stderr,"ERROR: line %d: operation %s is illegal. cannot check operands.\n", lineCounter, copyLine);
			}
			else /* legal operation */
			{
				memoryWord = buildOpFirstMemoryWord(operationList[i].opCode, OPCODE, memoryWord); /* insert the opcode to the memory word for optional symbol */
				memoryWord = buildOpFirstMemoryWord(operationList[i].numberOfParameters, GROUP, memoryWord); /* insert group type to memory word */
				memoryWord = buildOpFirstMemoryWord(0, ERA, memoryWord); /* insert E,R,A value to memory word. it's always zero*/
				memoryWord = buildOpFirstMemoryWord(5, FIXED, memoryWord); /* insert value for the last 3 bytes, always set to 101 (5) */
				/* check operands */
				errorCount += getParameters(operationList[i].opName, strtok(NULL, "\n"), &memoryWord, operationList[i].numberOfParameters, &operandsMemoryWords, lineCounter);
			}

			/* if there is a symbol - add it to the symbols table with current IC as memory word value */
			if(symbolFlag)
			{
				address = IC;
				isExtern = 0;
				isAttachedToOperation = 1;
				addSymbolToTable(createNewSymbol(name, address, isExtern, isAttachedToOperation, IC), lineCounter);
			}

			/* add memory word to list */
			addFirstMemoryWord(IC, memoryWord);

			/* update IC and continue to next line */
			IC += operandsMemoryWords + 1; /* 1 memoryWord for the command, more words for operands are in operandsMemoryWords */
		}
	}

	return errorCount;
}

/* This function gets the data of .string and .data lines
 * Returns the number of errors found in the data, printing to stderr the errors found.
 * The function act base on the dataFlag - if its 1-> the line is .data, else -> the line is .string
 * each value is inserted to the dataArray array, received as parameter from the calling method.
 * if this is .data line, the method calling the checkNumber function for each number extracted from line, to check if the number is legal and integer.
 */

int getData(char * line, int * dataArray, int lineCount, int dataFlag, int * dataCounter)
{
	int errorCount = 0;
	char * currentToken;
	int currentValue;
	int i = 0; /* index */

	if(isEmpty(line)) /* missing data in data line */
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: missing data values.\n", lineCount);
		return errorCount;
	}

	if(dataFlag) /* expecting numbers with commas */
	{
		/* check for illegal insertion of commas, like ",," */
		int numberOfCommas = occurrencesCount(line, ','); /* count number of commas in data line */
		if(strstr(line, ",,") != NULL)
		{
			errorCount++;
			fprintf(stderr,"ERROR: line %d: illegal insertion of commas\n", lineCount);
		}
		currentToken = strtok(line, ",\n");
		while(currentToken != NULL) /* get each number from the line */
		{
			errorCount += checkNumber(currentToken, &currentValue, lineCount);

			dataArray[i] = currentValue;
			i++;
			(*dataCounter)++;
			currentToken = strtok(NULL, ",\n");
		}

		if((*dataCounter) <= numberOfCommas) /* too many commas were inserted */
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: unexpected comma.\n", lineCount);
		}
	}
	else /* expecting string with "" */
	{
		if(occurrencesCount(line, '"') < 2) /* missing " */
		{
			errorCount++;
			fprintf(stderr,"ERROR: line %d: missing \" for .string declaration\n", lineCount);
			return errorCount;
		}
		if(occurrencesCount(line, '"') > 2) /* too many " */
		{
			errorCount++;
			fprintf(stderr,"ERROR: line %d: %d unexpected \" in .string declaration\n", lineCount, occurrencesCount(line, '"')-2);
		}
		currentToken = skipWhiteSpaces(line); /* remove all white spaces before first " */
		currentToken = currentToken + 1; /* skip first " */
		currentValue = (*currentToken);
		while(currentValue != '"') /* insert values until reaching the second " */
		{
			dataArray[i] = currentValue;
			i++;
			(*dataCounter)++;
			currentToken = currentToken + 1;
			currentValue = (*currentToken);
		}

		if(!isEmpty(currentToken + 1)) /* check if line after second " is empty */
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: undefined characters after string.\n", lineCount);
		}
	}

	return errorCount;
}

/* This function get the operands from operation line in input, and build the operands part in the first memory word for the operation (received as parameter from calling function.
 * The function check the number of parameters expected,  check their type (using checkOperandType function,
 * and whether those operands are legal for the operation (using the checkOperandsForCommand function).
 * The function decide how many memory words should be created for the operands (for example , for 2 registers -> one memory word) and changes numberOfParameters accordingly. 
 * The function returns number of errors found.
 */

int getParameters(char * operationName, char * parametersLine, int * memoryWord, int numberOfParameters, int * operandsMemoryWords, int lineCounter)
{
	int errorCount = 0;
	char currentToken[MAX_LENGTH] = {'\0'};
	char firstOperand[MAX_LENGTH];
	char secondOperand[MAX_LENGTH];
	int sourceOperand = -1; /* holds type of operand */
	int destinationOperand = -1; /* holds type of operand */

	if(numberOfParameters == 0) /* no parameters, parametersLine should be empty */
	{
		(*memoryWord) = buildOpFirstMemoryWord(0, SOURCEOP, (*memoryWord));
		(*memoryWord) = buildOpFirstMemoryWord(0, DESOP, (*memoryWord));
		(*operandsMemoryWords) = 0;
		if(!isEmpty(parametersLine))
		{
			fprintf(stderr, "ERROR: line %d: unexpected operands for command with no operands\n", lineCounter);
			errorCount++;
		}
		return errorCount;
	}

	if(isEmpty(parametersLine)) /* missing parameters */
	{
		errorCount++;
		fprintf(stderr, "ERROR: line %d: missing parameters to operation\n", lineCounter);
		(*operandsMemoryWords) = numberOfParameters; /* number of memory words as should be if the operands weren't missing */
		return errorCount;
	}

	strcpy(currentToken, parametersLine);
	strcpy(currentToken, skipWhiteSpaces((currentToken))); /* skip white spaces */
	currentToken[strlen(parametersLine)] = '\n';

	if(numberOfParameters == 1) /* only destination operand, no source operand */
	{
		(*memoryWord) = buildOpFirstMemoryWord(0, SOURCEOP, (*memoryWord));
		if(occurrencesCount(parametersLine, ',') != 0) /* find illegal commas */
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d, unexpected \",\"\n", lineCounter);
		}

		strcpy(firstOperand, strtok(currentToken, "\n")); /* get first operand token */
		strcpy(firstOperand, strtok(firstOperand, " \t")); /* separate rest of line from second operand */
		/* check if rest of line is empty after operand */
		if(!isEmpty(strtok(NULL, "\n")))
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d: too many operands for command\n", lineCounter);
		}
		errorCount += checkOperandType(firstOperand, &destinationOperand, lineCounter);
		(*memoryWord) = buildOpFirstMemoryWord(destinationOperand, DESOP, (*memoryWord));
		(*operandsMemoryWords)++;
	}

	else if(numberOfParameters == 2)
	{
		if(occurrencesCount(parametersLine, ',') > 1) /* find illegal commas */
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d, unexpected \",\"\n", lineCounter);
		}

		if(occurrencesCount(parametersLine, ',') < 1) /* indicate missing comma */
		{
			errorCount++;
			fprintf(stderr, "ERROR: line %d, missing \",\" between operands\n", lineCounter);
		}

		/* get second operand token */
		if(isEmpty(strstr(currentToken, ","))) /* missing operand */
		{
			errorCount++;
			strcpy(secondOperand, ""); /* empty operand */
		}
		else
		{
			char * end;
			strcpy(secondOperand, strstr(currentToken, ",") + 1);
			strcpy(secondOperand, skipWhiteSpaces(secondOperand)); /* remove white spaces from second operand */
			/* skip white spaces at end */
			end = secondOperand + strlen(secondOperand) - 1;
			while(end > secondOperand && isspace(*end)) end--;
		    /* Write new null terminator */
		    *(end+1) = '\0';
		    
		    /* check if user inserted more operands after second operand */
		    if(strstr(secondOperand, ",") != NULL)
		    {
		    	if(strstr(secondOperand, ",") - secondOperand == 0) /* comma is at beginning -> double commas */
		    	{
		    		strcpy(secondOperand, strstr(secondOperand, ",") + 1);
		    		strcpy(secondOperand, skipWhiteSpaces(secondOperand));
		    	}
		    	else
		    	{
		    		strtok(secondOperand, ","); /* remove the unexpected operands from second operand */
		    		errorCount++;
		    		fprintf(stderr, "ERROR: line %d: too many operands for command\n", lineCounter);
		    	}
		    }
		}
		strcpy(firstOperand, strtok(currentToken, ",")); /* get first operand token */

		/* check if rest of line is empty after operands */
		if(!isEmpty(secondOperand))
		{
			if(!isEmpty(strstr(parametersLine, secondOperand) + strlen(secondOperand)))
			{
				errorCount++;
				fprintf(stderr, "ERROR: line %d: undefined characters after operation's operands.\n", lineCounter);
			}
		}

		/* get source operand */
		errorCount += checkOperandType(firstOperand , &sourceOperand, lineCounter);
		(*memoryWord) = buildOpFirstMemoryWord(sourceOperand, SOURCEOP, (*memoryWord));

		/* get destination operand */
		errorCount += checkOperandType(secondOperand, &destinationOperand, lineCounter);
		(*memoryWord) = buildOpFirstMemoryWord(destinationOperand, DESOP, (*memoryWord));

		(*operandsMemoryWords) = 2;
	}

	/* check if both operands or registers - if so, only 1 memory word for both of them */
	if(sourceOperand == REGISTER && destinationOperand == REGISTER)
	{
		(*operandsMemoryWords) = 1;
	}

	/* check if operands are legal for the command */
	errorCount += checkOperandsForCommand(sourceOperand, destinationOperand, operationName, lineCounter);

	return errorCount;
}

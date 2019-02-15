myprog: main.o firstRead.o secondRead.o stringMethods.o validationMethods.o memoryTablesMethods.o numbersMethods.o
	gcc -g -ansi -Wall -pedantic main.o firstRead.o secondRead.o stringMethods.o validationMethods.o memoryTablesMethods.o numbersMethods.o -o myprog

main.o: main.c
	gcc -c -ansi -Wall -pedantic main.c -o main.o

firstRead.o: firstRead.c
	gcc -c -ansi -Wall -pedantic firstRead.c -o firstRead.o

secondRead.o: secondRead.c
	gcc -c -ansi -Wall -pedantic secondRead.c -o secondRead.o

stringMethods.o: stringMethods.c
	gcc -c -ansi -Wall -pedantic stringMethods.c -o stringMethods.o

validationMethods.o: validationMethods.c
	gcc -c -ansi -Wall -pedantic validationMethods.c -o validationMethods.o

memoryTablesMethods.o: memoryTablesMethods.c
	gcc -c -ansi -Wall -pedantic memoryTablesMethods.c -o memoryTablesMethods.o

numbersMethods.o: numbersMethods.c
	gcc -c -ansi -Wall -pedantic numbersMethods.c -o numbersMethods.o

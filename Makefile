CC = g++
testOS : main.o test.o
	gcc -o testOS main.o test.o
main.o : ./src/main.c
	gcc -c -o main.o ./src/main.c
test.o : ./src/test.c
	gcc -c -o test.o ./src/test.c
clean:
	rm *.o
all:
	rm *.o testOS
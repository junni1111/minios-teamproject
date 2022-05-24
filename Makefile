CC = g++
test : main.o directory.o instruction.o stack.o time.o user.o
	gcc -o ./build/test ./build/main.o ./build/directory.o ./build/instruction.o ./build/stack.o ./build/time.o ./build/user.o -lpthread
main.o : ./src/main.c
	gcc -c -o ./build/main.o ./src/main.c -lpthread
directory.o : ./src/directory.c
	gcc -c -o ./build/directory.o ./src/directory.c -lpthread
instruction.o : ./src/instruction.c
	gcc -c -o ./build/instruction.o ./src/instruction.c -lpthread
stack.o : ./src/stack.c
	gcc -c -o ./build/stack.o ./src/stack.c -lpthread
time.o : ./src/time.c
	gcc -c -o ./build/time.o ./src/time.c -lpthread
user.o : ./src/user.c
	gcc -c -o ./build/user.o ./src/user.c -lpthread
obj:
	rm ./build/*.o
clean:
	rm ./build/*.o ./build/test
run :
	./build/test
CC = g++
test : main.o directory.o command.o init.o stack.o time.o user.o thread.o
	gcc -o ./build/test ./build/main.o ./build/directory.o ./build/command.o ./build/init.o ./build/stack.o ./build/time.o ./build/user.o ./build/thread.o -lpthread
main.o : ./src/main.c
	gcc -c -o ./build/main.o ./src/main.c -lpthread
directory.o : ./src/directory.c
	gcc -c -o ./build/directory.o ./src/directory.c -lpthread
command.o : ./src/command.c
	gcc -c -o ./build/command.o ./src/command.c -lpthread
init.o : ./src/init.c
	gcc -c -o ./build/init.o ./src/init.c -lpthread
stack.o : ./src/stack.c
	gcc -c -o ./build/stack.o ./src/stack.c -lpthread
time.o : ./src/time.c
	gcc -c -o ./build/time.o ./src/time.c -lpthread
user.o : ./src/user.c
	gcc -c -o ./build/user.o ./src/user.c -lpthread
thread.o : ./src/thread.c
	gcc -c -o ./build/thread.o ./src/thread.c -lpthread
obj:
	rm ./build/*.o
clean:
	rm ./build/*.o ./build/test
run :
	./build/test
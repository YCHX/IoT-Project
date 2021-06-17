Test: main.o synchronize.o
	gcc -o Test main.o synchronize.o
main.o: main.c
	gcc -c main.c
synchronize.o: synchronize.c
	gcc -c synchronize.c
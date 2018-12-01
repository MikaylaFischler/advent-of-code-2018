sol : ./bin/main.o
	gcc -o sol ./bin/main.o

./bin/main.o : ./src/main.c ./src/main.h
	gcc -c ./src/main.c -o ./bin/main.o

clean :
	rm sol ./bin/*.o


all : sol

sol : ./bin/main.o
	gcc -Wall -o sol ./bin/main.o

optimized : ./src/main.c ./src/main.h
	gcc -O3 -Wall ./src/main.c -o sol

./bin/main.o : ./src/main.c ./src/main.h
	gcc -Wall -c ./src/main.c -o ./bin/main.o

clean :
	rm sol ./bin/*.o

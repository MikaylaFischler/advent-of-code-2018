all : sol

sol : ./bin/main.o
	gcc -Wall -o sol ./bin/main.o

optimized : ./src/main.c ./src/main.h
	gcc -O3 -Wall ./src/main.c -o sol

# rev_eng : ./src/rev_eng.c
# 	gcc -O3 -Wall ./src/rev_eng.c -o p2
#
# rev_eng2 : ./src/rev_eng.c
# 	gcc -O3 -Wall ./src/rev_eng2.c -o p2

./bin/main.o : ./src/main.c ./src/main.h
	gcc -Wall -c ./src/main.c -o ./bin/main.o

clean :
	rm sol ./bin/*.o

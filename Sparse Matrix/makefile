FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: ex2

clean:
	rm -rf *.o ex2

ex2: main.o spmat.o
	gcc main.o spmat.o -o ex2 $(LIBS)

spmat.o: spmat.c
	gcc $(FLAGS) -c spmat.c

main.o: main.c spmat.h
	gcc $(FLAGS) -c main.c


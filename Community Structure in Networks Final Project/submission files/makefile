FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: cluster

clean:
	rm -rf *.o cluster

cluster: cluster.o spmat.o mat.o vec_group.o division.o errors.o temp_vectors.o
	gcc cluster.o spmat.o mat.o vec_group.o division.o errors.o temp_vectors.o -o cluster $(LIBS)

spmat.o: spmat.c
	gcc $(FLAGS) -c spmat.c

mat.o: mat.c
	gcc $(FLAGS) -c mat.c

vec_group.o: vec_group.c
	gcc $(FLAGS) -c vec_group.c

division.o: division.c
	gcc $(FLAGS) -c division.c

errors.o: errors.c
	gcc $(FLAGS) -c errors.c

temp_vectors.o: temp_vectors.c
	gcc $(FLAGS) -c temp_vectors.c

cluster.o: cluster.c spmat.h mat.h vec_group.h division.o errors.o
	gcc $(FLAGS) -c cluster.c
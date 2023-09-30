CC = gcc
LD = gcc

# -O0 désactive les optimisations à la compilation
# C'est utile pour débugger, par contre en "production"
# on active au moins les optimisations de niveau 2 (-O2).
CFLAGS = -Wall -Wextra -std=c99 -I../include -O0 -g
LDFLAGS = -lm -pg

OBJ_FILES = $(filter-out ../obj/ppm2jpeg.o, $(wildcard ../obj/*.o))
OBJ_PROF_FILES = ../obj_prof/htables_prof.o

# Fichiers de test
SRC_TESTS = $(wildcard *-test.c)
OBJ_TESTS = $(patsubst %.c,%.o,$(SRC_TESTS))
TESTS = $(patsubst %-test.c,%-test,$(SRC_TESTS))

all: $(TESTS)

%-test: %-test.o
	$(CC) -g $(OBJ_FILES) $(OBJ_PROF_FILES) $(OBJ_TEST) $^ -o $@ $(LDFLAGS)

%-test.o: %-test.c
	$(CC) $(CFLAGS) -lm -c $^

clean:
	rm -rf *.o *~ $(TESTS)

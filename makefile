CC = clang
P_NAME = main
FLAGS = -Wall -g 
LIBS = -l raylib 
EXEC_NAME = raycaster

all: compile run

compile:
	$(CC) src/$(P_NAME).c -o bin/$(EXEC_NAME) $(LIBS)

run:
	./bin/$(EXEC_NAME)

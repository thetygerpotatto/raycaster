CC = clang
P_NAME = main
FLAGS = -g --extra-warnings -Wall
LIBS = -l raylib -l m
EXEC_NAME = raycaster

all: compile run

compile:
	$(CC) $(FLAGS) src/$(P_NAME).c -o bin/$(EXEC_NAME) $(LIBS)

run:
	./bin/$(EXEC_NAME)

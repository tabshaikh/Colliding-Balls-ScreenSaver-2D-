# This makefile is used to compile the project.
# You can run the project by running the makefile using the make command from the terminal.

CC=g++ 
CFLAGS=-lglut -lGLU -lGL -lpthread

all: 
	$(CC) -std=c++11 ./src/main.cpp ./src/ball.cpp $(CFLAGS) -o ./output/collide	


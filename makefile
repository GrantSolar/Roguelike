CC=g++
FLAGS=-o RL -Iinclude -L./lib -ltcod -ltcodxx -Wl,-rpath=.

all:
	$(CC) src/*.cpp $(FLAGS)
	cp lib/*.so.1 .

EXECUTABLE=RL
CC=g++
FLAGS=-o $(EXECUTABLE) -g -Iinclude -L./lib -ltcod -ltcodxx -Wl,-rpath=.

all:
	$(CC) src/*.cpp $(FLAGS)
	cp lib/*.so.1 .

clean:
	rm $(EXECUTABLE)

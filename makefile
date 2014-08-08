EXECUTABLE=RL
CC=g++
FLAGS=-o $(EXECUTABLE) -g -Iinclude -L./lib -ltcod -ltcodxx -lstdc++ -lm -Wl,-rpath=.

all:
	$(CC) src/*.cpp $(FLAGS)
	cp lib/*.so .

clean:
	rm $(EXECUTABLE)

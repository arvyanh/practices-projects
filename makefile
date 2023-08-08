CC=g++
CFLAGS=-std=c++2a

DEPS = hash.hpp
OBJ = hash.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hash: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


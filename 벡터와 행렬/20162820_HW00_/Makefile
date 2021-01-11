HEADERS = vec.hpp mat.hpp operator.hpp
SOURCES = main.cpp
CC = g++
CFLAGS = -std=c++11
EXECUTABLE = Math
RM = rm -rf

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES)

clean:
	$(RM) *.o $(EXECUTABLE)
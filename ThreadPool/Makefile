CC=g++
CFLAGS=-c -Wall
LDFLAGS= -lpthread 
SOURCES=main.cpp TaskQueue.cpp ThreadPool.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=hello

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o




CC=g++
CFLAGS=-c -Wall 
LDFLAGS= -lpthread 
SOURCES=test.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
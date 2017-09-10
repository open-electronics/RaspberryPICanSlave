CC=gcc
CXX=g++
CFLAGS=-c -Wall
CXXFLAGS=-c -Wall

LDFLAGS=-lmicrohttpd
INCLUDES=-Isrc/include


CSOURCES=src/WebServer.c
CXXSOURCES= 
OBJECTS=$(CXXSOURCES:.cpp=.o) $(CSOURCES:.c=.o)
EXECUTABLE=CANSlave

all: $(CXXSOURCES) $(CSOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf src/*o $(EXECUTABLE)

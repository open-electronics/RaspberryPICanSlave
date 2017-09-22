CC=gcc
CXX=g++
CFLAGS=-c -Wall
CXXFLAGS=-c -Wall

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
LDFLAGS=-lmicrohttpd -lpthread
INCLUDES=-Isrc/include
endif
ifeq ($(UNAME), Darwin)
CXXFLAGS +=  -std=c++11  -stdlib=libc++
LDFLAGS=-L/usr/local/lib -lmicrohttpd
INCLUDES=-Isrc/include -I/usr/local/include
endif

CSOURCES=src/WebServer.c
CXXSOURCES=src/CfgLoader.cpp
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

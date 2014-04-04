CXX     = g++
AR	    = ar
LIBMISC	= libcustomserver.a
RANLIB  = ranlib
HEADER  = -I./core -I. -I./utils
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = core/*.cpp

tests = test_squeue test_buffer test_callback test_log test_slice test_tostring

PROGS = server client

all: clean prepare ${PROGS}
	
server: tests/echo_server.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

client: tests/echo_client.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

prepare:
	mkdir bin

clean: 
	-rm -rf bin
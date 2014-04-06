CXX     = g++
AR	    = ar
LIBMISC	= libcustomserver.a
RANLIB  = ranlib
HEADER  = -I./src/core -I. -I./src/utils -I./src
CHEADER = -I.src/ccomet
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = src/core/*.cpp

tests = test_squeue test_buffer test_callback test_log test_slice test_tostring

PROGS = server client

all: clean prepare ${PROGS}

server: tests/echo_server.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

client: tests/echo_client.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

ccomet: clean prepare comet

comet: src/ccomet/Server.cpp src/ccomet/Subscriber.cpp src/ccomet/CComet.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${CHEADER} ${HEADER} $^ -o $@ 
	mv $@ bin

prepare:
	mkdir bin

clean: 
	-rm -rf bin comet
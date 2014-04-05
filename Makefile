CXX     = g++
AR	    = ar
LIBMISC	= libcustomserver.a
RANLIB  = ranlib
HEADER  = -I./core -I. -I./utils
CHEADER = -I./ccomet
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = core/*.cpp

tests = test_squeue test_buffer test_callback test_log test_slice test_tostring

PROGS = server client

all: clean prepare ${PROGS}

server: tests/echo_server.cpp utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

client: tests/echo_client.cpp utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

comet: ccomet/Server.cpp ccomet/Subscriber.cpp ccomet/CComet.cpp utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${CHEADER} ${HEADER} $^ -o $@ 
	mv $@ bin

prepare:
	mkdir bin

clean: 
	-rm -rf bin comet
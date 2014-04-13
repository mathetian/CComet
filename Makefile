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

BENCHS = bench_comet bench_c100k

PROGS  = echo_server echo_client comet ${BENCHS}

all: clean prepare ${PROGS}

echo_server: tests/echo_server.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

echo_client: tests/echo_client.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

comet: src/ccomet/Server.cpp src/ccomet/Subscriber.cpp src/ccomet/CComet.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${CHEADER} ${HEADER} $^ -o $@ 
	mv $@ bin

bench_comet: tests/bench_comet.cpp src/utils/Log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin
	
bench_c100k: tests/bench_c100k.cpp
	$(CXX) ${CXXFLAGS}  $^ -o $@ 
	mv $@ bin

EPollServer: extras/EPollServer.cpp
	$(CXX) ${CXXFLAGS} ${CHEADER} ${HEADER} $^ -o $@ 

prepare:
	mkdir bin

clean: 
	-rm -rf bin comet bench*
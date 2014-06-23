CXX     = g++
AR	    = ar
LIBMISC	= libsealedserver.a
RANLIB  = ranlib
HEADER  = -I./deps/SealedServer/include -I./deps/SealedServer/utils -I./src/core
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = src/core/*.cpp

tests = test_squeue test_buffer test_callback test_log test_slice test_tostring

BENCHS = bench_comet bench_c100k

PROGS  = echo_server echo_client comet ${BENCHS}

all: clean prepare ccomet

ccomet: ${SOURCES}
	${CXX} ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LIBMISC}
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
	-mkdir bin
	cd deps/SealedServer; make; cp *.a ../..; cd ../..

clean: 
	-rm -rf bin comet bench*
CXX     = g++
AR	    = ar
LIBMISC	= libeventserver.a
RANLIB  = ranlib
HEADER  = -I./deps/EventServer/include -I./deps/EventServer/utils -I./deps/EventServer/http -I./src/core
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = src/core/*.cpp src/*.cpp

tests = test_squeue test_buffer test_callback test_log test_slice test_tostring

BENCHS = bench_comet bench_c100k

PROGS  = echo_server echo_client comet ${BENCHS}

all: clean prepare ccomet

ccomet: ${SOURCES}
	${CXX} ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LIBMISC}
	mv $@ bin

bench_comet: tests/bench_comet.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LIBMISC}
	mv $@ bin
	
prepare:
	-mkdir bin
	cd deps/EventServer; make; cp *.a ../..; cd ../..

clean: 
	-rm -rf bin comet bench*
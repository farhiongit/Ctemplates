CC=clang
#CC=gcc
CFLAGS += -g -Wall -Wextra -pedantic -Werror -I..
CFLAGS += -fmacro-backtrace-limit=0
#CFLAGS += -fdebug-cpp
LDFLAGS += -pthread

all: examples

list_example.o: list_example.c ../defops.h ../list_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../list.h
set_example.o: set_example.c ../defops.h ../set_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../set.h
map_example.o: map_example.c ../defops.h ../map_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../map.h

.PHONY: examples
examples: list_example set_example map_example
	valgrind -q ./list_example || :
	valgrind -q ./set_example || :
	valgrind -q ./map_example || :
